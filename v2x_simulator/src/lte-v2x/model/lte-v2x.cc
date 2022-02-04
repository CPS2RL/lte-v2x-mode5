/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/log.h>

#include <ns3/core-module.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/node-container.h>
#include <ns3/mobility-model.h>
#include <ns3/mobility-module.h>
#include "ns3/constant-velocity-mobility-model.h"

#include "lte-v2x.h"
#include "ns3/lte-v2x-helper.h"

#include <stdlib.h> 
#include <vector>
#include <math.h>
#include <unordered_map> 
#include <complex>
#include <string>
#include <regex>
#include <iostream>
#include <utility>
#include <functional>



namespace ns3 {

// mhasan: implementation of LTE-V2X

NS_LOG_COMPONENT_DEFINE ("LTE-V2XModule");


double
ChannelInfo::GetPSSCHThermalNoise ()
{
  // returns pssch thermal noise in dBm
  return  m_noiseSd + 10 * std::log10 (m_psschBandwidth) + m_noiseFigure;
}

double
ChannelInfo::GetThermalNoise ()
{
  // returns thermal noise in dBm
  return  m_noiseSd + 10 * std::log10 (m_subChannelBwMHz * 1000000) + m_noiseFigure;
}


double
ChannelInfo::GetFadingCoefByDist (double dist)
{
  // returns fading co-efficients by distance
  if (dist < 50)
    return 3.0;
  else if (dist >= 50 && dist < 150)
    return 1.5;
  else
    return 1.0;
}

// returns the PSCCH power in dBm
double 
LTEV2X_System::GetPSSCHPowerDbmFromFactor (double txPower)
{
  double txPowerW = LTEV2X_Helper::DbmToWatt (txPower); // change to watt
  double pPSSCH = txPowerW/m_dataControlPowerFactor; // PSSCH power in watt
  return LTEV2X_Helper::WattToDbm(pPSSCH);  // return the value in dBm
}

/* returns interpolated PER for a given SINR */
// code from: https://stackoverflow.com/questions/11396860/better-way-than-if-else-if-else-for-linear-interpolation
double 
LTEV2X_System::DoGetInterpolatePER(double sinr, std::vector<std::pair<double, double>> &sinr2perTable) 
{
    // Assumes that "table" is sorted by .first (SINR)
    // Check if SINR is out of bound
    if (sinr > sinr2perTable.back().first) return 0.0;  // no error
    if (sinr < sinr2perTable[0].first) return 1.0;
    std::vector<std::pair<double, double>>::iterator it, it2;
    
    it = std::lower_bound(sinr2perTable.begin(), sinr2perTable.end(), std::make_pair(sinr, 1.0));
    // Corner case
    if (it == sinr2perTable.begin()) return it->second;
    it2 = it;
    --it2;
    return it2->second + (it->second - it2->second)*(sinr - it2->first)/(it->first - it2->first);
}

/* return distance dependent PER */
double 
LTEV2X_System::GetInterpolatePERnoHARQ(double sinr, double dist) 
{
  return DoGetInterpolatePER(sinr, m_sinr2perTableNOHARQ__AWGN);
}


double 
LTEV2X_System::GetInterpolatePERwithHARQ(double sinrKey, double dist) 
{
  if (sinrKey < 0 || sinrKey > __MAXKEY_HARQ_INDX)
    return 1.0;  // return pakcet error

  
  return m_sinr2perTableWITHHARQ__AWGN.at ((int)sinrKey).second;

}




// returns avrage SINR per RX antenna
double
LTEV2X_System::GetAvgSINRPerRxAntennaDb (double sinrDb)
{
  double sinrLinear = std::pow (10,sinrDb/10.0);
  double avgSinr = sinrLinear/2;

  // return dB unit
  return 10 * std::log10 (avgSinr);
}

// returns the interference received power using MRC
double LTEV2X_System::GetIntfReceivedPowerWatt (std::complex<double> h1TxComplex,
                                                std::complex<double> h2TxComplex,
                                                double infTxPowerLinear,
                                                double infPlLinear,
                                                std::complex<double> h1InfComplex,
                                                std::complex<double> h2InfComplex)
{
  std::complex<double> h1TxConjugate = std::conj(h1TxComplex);
  std::complex<double> h2TxConjugate = std::conj(h2TxComplex);

  // std::complex<double> tmp = (h1TxConjugate * h1InfComplex) + (h2TxConjugate * h2InfComplex);

  double ffMrc = abs ((h1TxConjugate * h1InfComplex) + (h2TxConjugate * h2InfComplex));
  double infRxPowerWatt = infPlLinear * ffMrc * infTxPowerLinear;

  // NS_LOG_INFO ("Interference received power (W): " << infRxPowerWatt << " Dbm: " << LTEV2X_Helper::WattToDbm (infRxPowerWatt));

  return infRxPowerWatt;
}


/* returns the loss in dB */ 
double
ChannelInfo::GetPathLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double dist, double carrierFrequency)
{
  // carrierFrequency is m_sysBwMHz * 1000000 (in Hz)
  double lambda = 299792458.0 / carrierFrequency;

  // NS_ASSERT_MSG (a->GetPosition ().z > 0 && b->GetPosition ().z > 0, "Antenna height must be greater than 0");

  double Lbp = std::fabs (20 * std::log10 ((lambda * lambda) / (8 * M_PI *  m_vehicleAntennaHeight *  m_vehicleAntennaHeight)));
  double Rbp = (4 *  m_vehicleAntennaHeight *  m_vehicleAntennaHeight) / lambda;

  double loss = 0.0;

  if (dist <= Rbp)
    loss = Lbp + 6 + (20 * std::log10 (dist / Rbp));
  else
    loss = Lbp + 6 + (40 * std::log10 (dist / Rbp));

  return loss;
}

void
LTEV2X_System::InitSimParam (SimParam &simP) 
{

  m_simDuration = simP.m_simDuration;  // update simulation duration
  /* set experiment name and log file */
  m_expName = simP.m_expName;  // name of the experiment;
  m_outLogFileName = simP.m_outLogFileName; // log file name to save the traces 

  m_isHARQEnabled = simP.m_isHARQEnabled; // HARQ info
  m_isPowerControlEnabled = simP.m_isPowerControlEnabled; // power control info
  m_isCongestControlEnabled = simP.m_isCongestControlEnabled; // enable/disable congestion control
  m_sysBwMHz = simP.m_sysBwMHz; // set system bandwidth
  m_nSubChannels = m_sysBwMHz/ChannelInfo::m_subChannelBwMHz; // set number of subchannels
  m_selectionWindowT1 = simP.m_selectionWindowT1;  // set the selection window size (T1)
  m_pKeep = simP.m_pKeep;  // probablity of keeping same resource
  m_defaultMsgArrivalMS = simP.m_defaultMsgArrivalMS;  // set the default message arrival rate
  m_MCS = simP.m_MCS;  // set the modulation and coding info
  m_nSubChPSSCHTx = simP.m_nSubChPSSCHTx;  // number of selected subchannel for tansmission (should be contigious)

  /* info about T2 selection window */
  m_isFixedT2Window = simP.m_isFixedT2Window; // whether fixed T2 window size is used 
  m_selectionWindowFixedT2 = simP.m_selectionWindowFixedT2;  // used when fixed T2 window (ms)

  // m_pPSSCH = GetPSSCHPowerDbmFromFactor (); // set the PSSCH power (dBm)

  /* area of interest */
  m_aoiX1 = simP.m_aoiX1;
  m_aoiX2 = simP.m_aoiX2;
  m_rxPad = simP.m_rxPad;

  // set start logging time
  m_logStartTime = simP.m_logStartTime;

}

/* checks if the vehicle is in TX mode */
TXStateWithPckIDInfo
LTEV2X_System::IsVehicleInTXMode (double timeMS, int vid)
{
  VehicleTXStateMap vtx = m_vehicleList.at (vid).m_isTXinSF;
  VehicleTXStateMap::const_iterator it = vtx.find(timeMS);
  if (it == vtx.end())
    return {-1, false};  // not in list, vehicle is not in TX mode
  else
  { 
    // NS_LOG_INFO ("vid:" << vid << " in TX mode. TX time:" << timeMS << " Packet id: " << it->second.packetID);
    return it->second;
  }
   

  return {-1, false}; /* return false anyways */
}

/* return channel usage for the given SF. return -1 if not found */
int
LTEV2X_System::GetChIDBySFforTXVID (double timeMS, int vid)
{
  VehicleChUsageMap vum = m_vehicleList.at (vid).m_chUsageMap;
  int chid = -1;
  
  VehicleChUsageMap::const_iterator it = vum.find(timeMS);
  if (it == vum.end())
    return -1;  // not in list, vehicle is not in TX mode
  else
    {
      chid = it->second;
      return chid;  /* entry found -> return chid */
    }
   
}

/* set the vehicle is in TX mode for that SF */
void 
LTEV2X_System::SetVehicleInTXMode (double timeMS, int vid, double packetid)
{
  TXStateWithPckIDInfo t = {.packetID = packetid, .isTX=true}; 
  m_vehicleList.at (vid).m_isTXinSF[timeMS] = t;
}

/* set the vehicle channel usage for that SF */
void 
LTEV2X_System::SetVehicleChannelUsageMap (double timeMS, int vid, int chid)
{
  m_vehicleList.at (vid).m_chUsageMap[timeMS] = chid;
}



void
LTEV2X_System::AddVehicleToChannel (double timeMS, int vid, int chid)
{
  m_SFReserveAllChannelList.at (chid)[timeMS].push_back (vid);
}


// void 
// LTEV2X_System::CheckBound (Ptr<ConstantVelocityMobilityModel> mob)
// {
  
//   Vector m_position = mob->GetPosition();
//   Vector m_velocity = mob->GetVelocity();
//   if (m_position.x > m_highwayLength) 
//     {
//       // NS_LOG_INFO ("BOUND CHECKING CALLED!! Reset to Zero.");
//       m_position.x = 0.0;
//       mob->SetPosition(m_position);
//       mob->SetVelocity(m_velocity);
//     }
//   else if (m_position.x < 0) 
//     {
//       // NS_LOG_INFO ("BOUND CHECKING CALLED!! Reset to 5KM.");
//       m_position.x = m_highwayLength;
//       mob->SetPosition(m_position);
//       mob->SetVelocity(m_velocity);
//     }
// }

// void 
// LTEV2X_System::CheckRoadBoundForVID (int vid)
// {
//   Ptr<ConstantVelocityMobilityModel> vMobility = m_vehicleContainer->Get (vid)->GetObject<ConstantVelocityMobilityModel> ();
//   CheckBound (vMobility);
// }


void
LTEV2X_System::SetVehicleInfo (int nVehicle, NodeContainer &vehicleContainer)
{
  m_nVehicle = nVehicle; // set the number of vehicles used in the simulation
  m_vehicleContainer = &vehicleContainer;  // save the pointer to the vehicle container
  m_vehicleList.resize (m_nVehicle);  // allocate memory for all the vehicles

  m_SFReserveAllChannelList.resize (m_nSubChannels); // allocate memory [all channles] for the vector

  NS_LOG_INFO ("Number of Vehicles: " << m_nVehicle);

  for (int i = 0; i < m_nVehicle; i++) 
    {
      VehicleInfo v;
      v.m_vehicleId = i;  // set id
      v.m_currentTxPower = m_txPowerDefault; // set the TX power to default value
      v.m_curPckArivRateMS = m_defaultMsgArrivalMS; // set the packet arrival frequency
      
      /* how often check vehicle density */
      v.m_vehDenCheckSF = LTEV2X_Helper::GetRandomNumberInRange(1, m_vTxRateCntrlInt);  // randomize it for each vehicle

      if (!m_isFixedT2Window)
        v.m_selectionWindowT2 = 20; // selection window size (initialize to 20)

      /* initialize smooth density */
      v.m_smoothVehicleDensity = (double) GetNeighborVehicleForVID (i, m_neighborVehicleSearchTh);
      
      // NS_LOG_INFO ("vid: " << i << " smooth vDensity: " << v.m_smoothVehicleDensity);



      /* set the first packet arrival time */
      // if (m_isCongestControlEnabled)
      //   v.m_PckArivSF = m_sensingDuration + GetMaxITT (v.m_smoothVehicleDensity);
      // else
      //   v.m_PckArivSF = LTEV2X_Helper::GetRandomNumberInRange (m_sensingDuration, m_sensingDuration+m_defaultMsgArrivalMS-1);
      
      // from 1 sec to 2 sec [1000, 1999]
      v.m_PckArivSF = LTEV2X_Helper::GetRandomNumberInRange (m_sensingDuration, (2*m_sensingDuration) - 1);
      
      v.m_PrevTXSF = v.m_PckArivSF;

      v.m_localSFCSI.resize (m_nSubChannels);  // allocate memory [all channles] for the vector
      
      v.m_lastTxPosition =  Vector3D (LARGEVALUE, LARGEVALUE, LARGEVALUE);  // last TX position -> initialize to a invalid large value

      m_vehicleList.at (i) = v; // set the vehicle info
    }

  /* set SINR-to-PER table */
  PopulateSINR2PERTable ();
  
 
}


// retrun the combined received power from two antenna
double
LTEV2X_System::GetCombinedAntennaRxPowerWatt (double txPowerWatt, double h1FastFadingLinear, double h2FastFadingLinear, double pathlossLinear)
{
  double rxPowerWatt = (h1FastFadingLinear*h1FastFadingLinear + h2FastFadingLinear*h2FastFadingLinear) * txPowerWatt * pathlossLinear;
  return rxPowerWatt;
}


void
LTEV2X_System::SimulateSubframe () 
{
  // NS_LOG_INFO ("In simulate function. Num Veh:" << m_nVehicle);
  // NS_LOG_INFO ("Exp: " << m_expName << " -> Current time: " << Simulator::Now ().GetMilliSeconds ());
  // NS_LOG_INFO ("HARQ Enabled:" << m_isHARQEnabled);
  // NS_LOG_INFO ("Number of Subchannels: " << m_nSubChannels);
  // NS_LOG_INFO ("Experiment Name: " << m_expName);
  // NS_LOG_INFO ("Output file name: " << m_outLogFileName);


  double currentTime = Simulator::Now ().GetMilliSeconds ();

  NS_LOG_INFO ("Exp: " << m_expName << " -> Simulating SF: " 
              << currentTime << " (" << (currentTime/m_simDuration)*100.00 << "%)..");
  
  // std::cout << "Exp: " << m_expName << " -> Simulating SF: " 
  //             << currentTime << " (" << (currentTime/m_simDuration)*100.00 << "%)..\n";
  

  for (int i = 0; i < m_nVehicle; i++) 
    {
      // CheckRoadBoundForVID (i);  // check if the vehicle is within 5 KM highway
      
      Ptr<Node> vDevice = m_vehicleContainer->Get (i);
      // int vid = m_vehicleList.at (i).m_vehicleId;
      // NS_LOG_INFO ("VID:" << m_vehicleList.at (i).m_vehicleId);
      // NS_LOG_INFO ("VID:" << vid << " current packet: " << m_vehicleList.at (i).m_PckArivSF);
      // LTEV2X_Helper::PrintPositionByNode (vDevice);


      UpdateCSIbyVID (currentTime, i); // calculate RSSI/RSRP etc.


      
      if (currentTime == m_vehicleList.at (i).m_vehDenCheckSF)
        {
          /* update and smooth vehicle density */
          UpdateSmoothVehicleDensity (currentTime, i);

          
          // NS_LOG_INFO (" vid: " << vid << " current time: " 
          //             << currentTime << " next pArrival  " 
          //             << m_vehicleList.at (i).m_PckArivSF 
          //             << " flag: " <<  m_vehicleList.at (i).m_txDecisionMaxITT);

          /* update counter for next time */
          m_vehicleList.at (i).m_vehDenCheckSF += m_vTxRateCntrlInt;

         /* also update CBR */
         UpdateCBRForVID (currentTime, i);

         /* update power */
         UpdatePowerForVID (currentTime, i);

         /* save this info into internal data structure */
         SaveCBRandPcktArrivalRateforVID (currentTime, i);

          
        }


      /* we start after 1000 ms */
      if (m_vehicleList.at (i).m_PckArivSF >= m_sensingDuration &&  currentTime == m_vehicleList.at (i).m_PckArivSF)
        {
          // NS_LOG_INFO ("VID:" << vid << " current time: " << currentTime << " packet arrival: " << m_vehicleList.at (i).m_PckArivSF);
          ReserveResourceForVID (currentTime, i);
        }

      /* check if there is a packet TX and save all TX-RX SINR */
      TXStateWithPckIDInfo tsinfo = IsVehicleInTXMode (currentTime, i);
      bool isTXMode = tsinfo.isTX;
      double packetid = tsinfo.packetID;
      // if (IsVehicleInTXMode (currentTime, i))
      if (isTXMode)
        {
          SaveLastTXPostionForVID (i); // save the position during last TX
          SaveTXRXSINRInfoForTXVID (currentTime, i, packetid); // save this for KPI evaluation
        }


    }
  

  Simulator::Schedule (MilliSeconds (1.0), &LTEV2X_System::SimulateSubframe, this);
}




void
LTEV2X_System::SimulateSubframeSingleTX () 
{
  // NS_LOG_INFO ("In simulate function. Num Veh:" << m_nVehicle);
  // NS_LOG_INFO ("Exp: " << m_expName << " -> Current time: " << Simulator::Now ().GetMilliSeconds ());
  // NS_LOG_INFO ("HARQ Enabled:" << m_isHARQEnabled);
  // NS_LOG_INFO ("Number of Subchannels: " << m_nSubChannels);
  // NS_LOG_INFO ("Experiment Name: " << m_expName);
  // NS_LOG_INFO ("Output file name: " << m_outLogFileName);


  double currentTime = Simulator::Now ().GetMilliSeconds ();


  NS_LOG_INFO ("Exp: " << m_expName << " -> Simulating SF (Single TX): " 
              << currentTime << " (" << (currentTime/m_simDuration)*100.00 << "%)..");
  

  int i = 0;  // the (only) single TX id
  Ptr<Node> vDevice = m_vehicleContainer->Get (i);
  UpdateCSIbyVID (currentTime, i); // calculate RSSI/RSRP etc.

  if (currentTime == m_vehicleList.at (i).m_vehDenCheckSF)
    {
      /* update and smooth vehicle density */
      UpdateSmoothVehicleDensity (currentTime, i);

      
      // NS_LOG_INFO (" vid: " << i << " current time: " 
      //             << currentTime << " next pArrival  " 
      //             << m_vehicleList.at (i).m_PckArivSF 
      //             << " flag: " <<  m_vehicleList.at (i).m_txDecisionMaxITT);

      /* update counter for next time */
      m_vehicleList.at (i).m_vehDenCheckSF += m_vTxRateCntrlInt;

      /* also update CBR */
      UpdateCBRForVID (currentTime, i);

      /* update power */
      UpdatePowerForVID (currentTime, i);

      /* save this info into internal data structure */
      SaveCBRandPcktArrivalRateforVID (currentTime, i);

      
    }


  /* we start after 1000 ms */
  if (m_vehicleList.at (i).m_PckArivSF >= m_sensingDuration &&  currentTime == m_vehicleList.at (i).m_PckArivSF)
    {
      // NS_LOG_INFO ("VID:" << i << " current time: " << currentTime << " packet arrival: " << m_vehicleList.at (i).m_PckArivSF);
      ReserveResourceForVID (currentTime, i);
    }

  /* check if there is a packet TX and save all TX-RX SINR */
  TXStateWithPckIDInfo tsinfo = IsVehicleInTXMode (currentTime, i);
  bool isTXMode = tsinfo.isTX;
  double packetid = tsinfo.packetID;
  // if (IsVehicleInTXMode (currentTime, i))
  if (isTXMode)
    {
      SaveTXRXSINRInfoForTXVID (currentTime, i, packetid); // save this for KPI evaluation
    }

  Simulator::Schedule (MilliSeconds (1.0), &LTEV2X_System::SimulateSubframeSingleTX, this);
}


void
LTEV2X_System::DoLogResourceSelectionEvents ()
{
  std::ofstream file;

  /* set filename */
  std::regex txtExtenstion(".txt");
  std::string fname = std::regex_replace(m_outLogFileName, txtExtenstion, "");
  fname = fname + "_ressel.txt";  // add a suffix


  file.open (fname, std::ios::out | std::ios::trunc);
   if (file.is_open ())
    {
       for (int vid = 0; vid < m_nVehicle; vid++) 
        {
          for (auto val:m_vehicleList.at (vid).m_logResourceSelectionSFInfo)
            {
              std::string outputstring = std::to_string ((int) vid) +  "; " +
                std::to_string ((int) val) +  "\n";
            
            // NS_LOG_INFO (outputstring);
            file << outputstring;  // write to file

            }
        }
    }
  else
    NS_LOG_ERROR ("ERROR Logging Reseource Reselection Info!" );


}


void
LTEV2X_System::DoLogCBRnArrivalRateResults ()
{
  std::ofstream file;

  /* set filename */
  std::regex txtExtenstion(".txt");
  std::string fname = std::regex_replace(m_outLogFileName, txtExtenstion, "");
  fname = fname + "_cbrNarrivalNpower.txt";  // add a suffix


  file.open (fname, std::ios::out | std::ios::trunc);
   if (file.is_open ())
    {
       for (int vid = 0; vid < m_nVehicle; vid++) 
        {
          for (auto val:m_vehicleList.at (vid).m_logCBRnArrivalRateInfo)
            {
              std::string outputstring = std::to_string ((int) val.logSF) +  "; " +
                      std::to_string ((int) val.vid) +  "; " +
                      std::to_string (val.cbr) +  "; " +
                      std::to_string (val.arrivalRate) +  "; " +
                      std::to_string (val.txPower) +  "\n";
            
            // NS_LOG_INFO (outputstring);
            file << outputstring;  // write to file

            }
        }
    }
  else
    NS_LOG_ERROR ("ERROR Logging CBR and Packet Arrival/Power Info!" );


}


void
LTEV2X_System::DoLogSINRResults (double simDuration)
{
  std::ofstream file;

  /* set filename */
  std::regex txtExtenstion(".txt");
  std::string fname = std::regex_replace(m_outLogFileName, txtExtenstion, "");
  fname = fname + "_sinr.txt";  // add a suffix


  file.open (fname, std::ios::out | std::ios::trunc);
  // File format: SF, CHID, TXID, RXID, DIST, SINR

  if (file.is_open ())
    {
      for (int vid = 0; vid < m_nVehicle; vid++) 
      {
        for (auto val:m_vehicleList.at (vid).m_logSINRInfo)
          {
            std::string outputstring = std::to_string ((int) val.arrivalSF) +  "; " +
                      std::to_string ((int) val.txSF) +  "; " +
                      std::to_string (val.chid) +  "; " +
                      std::to_string (val.txid) +  "; " +
                      std::to_string (val.rxid) +  "; " +
                      // "(" + std::to_string (val.txPos.x) + ", " + std::to_string (val.txPos.y) + ", " + std::to_string (val.txPos.z) + "); " +
                      // "(" + std::to_string (val.txSpeed.x) + ", " + std::to_string (val.txSpeed.y) + ", " + std::to_string (val.txSpeed.z) + "); " +
                      // "(" + std::to_string (val.rxPos.x) + ", " + std::to_string (val.rxPos.y) + ", " + std::to_string (val.rxPos.z) + "); " +
                      // "(" + std::to_string (val.rxSpeed.x) + ", " + std::to_string (val.rxSpeed.y) + ", " + std::to_string (val.rxSpeed.z) + "); " +
                      std::to_string (val.dist) +  "; " +
                      std::to_string (val.sinr) + "\n";
            
            // NS_LOG_INFO (outputstring);
            file << outputstring;  // write to file

          }
      }
      
      file.close ();
    }
  else
    NS_LOG_ERROR ("ERROR Logging Distance vs SINR!" );



}


void
LTEV2X_System::DoLogPERResults (double simDuration)
{
  std::ofstream file;

  /* set filename */
  std::regex txtExtenstion(".txt");
  std::string fname = std::regex_replace(m_outLogFileName, txtExtenstion, "");
  fname = fname + "_per.txt";  // add a suffix


  file.open (fname, std::ios::out | std::ios::trunc);
  // File format: ARRIVALSF, TXSF, TXID, RXID, DIST, PER

  if (file.is_open ())
    {
      for (int vid = 0; vid < m_nVehicle; vid++) 
      {
        for (auto val:m_vehicleList.at (vid).m_logPERInfo)
          {
            /* log only if simulation is saturated */
            if  (val.arrivalSF >= m_logStartTime)
              {
                std::string outputstring = std::to_string ((int) val.arrivalSF) +  "; " +
                      std::to_string ((int) val.txSF) +  "; " +
                      std::to_string (val.txid) +  "; " +
                      std::to_string (val.rxid) +  "; " +
                      // "(" + std::to_string (val.txPos.x) + ", " + std::to_string (val.txPos.y) + ", " + std::to_string (val.txPos.z) + "); " +
                      // "(" + std::to_string (val.txSpeed.x) + ", " + std::to_string (val.txSpeed.y) + ", " + std::to_string (val.txSpeed.z) + "); " +
                      // "(" + std::to_string (val.rxPos.x) + ", " + std::to_string (val.rxPos.y) + ", " + std::to_string (val.rxPos.z) + "); " +
                      // "(" + std::to_string (val.rxSpeed.x) + ", " + std::to_string (val.rxSpeed.y) + ", " + std::to_string (val.rxSpeed.z) + "); " +
                      std::to_string (val.dist) +  "; " +
                      std::to_string ((int) val.per) + "\n";
            
                // NS_LOG_INFO (outputstring);
                file << outputstring;  // write to file

              }

            

          }
      }
      
      file.close ();
    }
  else
    NS_LOG_ERROR ("ERROR Logging Distance vs SINR!" );



}



/* placeholder function to log simulation results */
void
LTEV2X_System::LogSimulationResults (double simDuration)
{
  // /* log SINR info */
  // DoLogSINRResults (simDuration);
  
  /* log PER info */
  DoLogPERResults (simDuration);

  /* log CBR and arrival info */
  DoLogCBRnArrivalRateResults ();

  // /* resource selection events */
  // DoLogResourceSelectionEvents ();

  
}


/* ... */



}
