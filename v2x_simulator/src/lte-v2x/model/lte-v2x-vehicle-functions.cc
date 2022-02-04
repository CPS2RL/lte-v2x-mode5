/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/log.h>

#include <ns3/core-module.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/node-container.h>

#include "lte-v2x.h"
#include "ns3/lte-v2x-helper.h"

#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <initializer_list>
#include <math.h>
#include <unordered_set>

namespace ns3 {

// mhasan: implementation of LTE-V2X

NS_LOG_COMPONENT_DEFINE ("LTE-V2X-VManager");

/* retrun PER for a given SINR [NO HARQ] */
int LTEV2X_System::GetSINR2PERnoHARQ (double sinr, double dist)
{
  double per = GetInterpolatePERnoHARQ (sinr, dist);

  if (per == 0.0 || per == 1.0)
    {
      // NS_LOG_INFO ("SINR: " << sinr << " PER: " << per);
      return (int) per;
    }

  /* if the PER is [0, 1] */
  double rv = LTEV2X_Helper::GetRandomRealNumberInRange (0.0, 1.0);

  // NS_LOG_INFO ("SINR: " << sinr << " PER: " << per << " rv: " << rv);

  if (rv > per)
    return 0;
  else
    return 1;

}

/* return PER and txtime for a given SINR and TX time [With HARQ] */
auto LTEV2X_System::GetSINR2PERwithHARQ (double sinr1, double sinr2, double tt1, double tt2, double dist)
{
  struct result
  {
    // combined per and packet tx time
    int per; double txtime;
  };

  /* check if we can decode the first packet [not sure we really need this] */
  int isFirstPacketFailed = GetSINR2PERnoHARQ (sinr1, dist);
  if (!isFirstPacketFailed)
    {
      return result {0, tt1}; // no error
    }

  int keySinr = GetHARQKeyFromTwoSINR (sinr1, sinr2);

  double per = GetInterpolatePERwithHARQ((double) keySinr, dist);


  /* if the PER is [0, 1] */
  double rv = LTEV2X_Helper::GetRandomRealNumberInRange (0.0, 1.0);

  
  // NS_LOG_INFO ("SINR 1: " << sinr1 << " SINR 2: " << sinr2 << " key: " << keySinr <<  " PER: " << per << " rv: " << rv);


  if (rv > per)
    return result {0, tt2};
  else
    return result {1, tt2};

}

int
LTEV2X_System::GetNeighborCountBySuccReceptionForVID (double timeMS, int vid, double distTh)
{

  // if less than 1000 ms or no packet transmitted yet return raw count from NS3
  if (timeMS <= m_vPERSubInterval || m_vehicleList.at (vid).m_PckArivSF ==  m_vehicleList.at (vid).m_PrevTXSF)
    return GetNeighborVehicleForVID (vid, distTh); 


  /* check in the sliding window as in the SAE J3161 [See section 6.3.7.1 and Fig. 23] */
  int begTime = (int) (timeMS - m_vPERSubInterval);
  std::unordered_set<int> txid_set;

  for (int t=begTime; t<(int)timeMS; t++)
    {
      // check all the window

      std::pair<succReceptionMMIterator, succReceptionMMIterator > it = m_vehicleList.at (vid).m_succPckRecepMMap.equal_range((double) t);
      // save (successful) TX ids to a set
      for (succReceptionMMIterator mmi = it.first; mmi != it.second; mmi++)
        txid_set.insert (mmi->second); // save unique values
    }

  // check who are within distance threshold
  int count = 0;
  for (const auto& txid: txid_set) {
    double dist = GetDistanceByTxRxVID (txid, vid);
    if (dist <= distTh)
      count++;  // increase the counter

  }

  // NS_LOG_INFO ("time: " << timeMS << " vid: " << vid << " Neigh Size in th:" << count);
  return count;
}

/* returns number of vehicles within the given distance threshold */
int
LTEV2X_System::GetNeighborVehicleForVID (int vid, double distTh)
{
  int count = 0;
  for (int nid=0; nid < m_nVehicle; nid++)
    {
      if (nid != vid)
        {
          // get the distance
          double dist = GetDistanceByTxRxVID (vid, nid);
          if (dist <= distTh)
            count++;  // increase the counter

        }
    }

  return count;
}



/* returns the distance by TX and RX ID */
double
LTEV2X_System::GetDistanceByTxRxVID (int txid, int rxid)
{
  Ptr<MobilityModel> txMobility = m_vehicleContainer->Get (txid)->GetObject<MobilityModel> ();
  Ptr<MobilityModel> rxMobility = m_vehicleContainer->Get (rxid)->GetObject<MobilityModel> ();

  // get the distance between vehicles
  double dist = LTEV2X_Helper::GetDistanceByMobilityModelPair (txMobility, rxMobility);

  return dist;

}

/* returns power in dBm */
double
LTEV2X_System::GetPowerMappingByCBR (double cbr)
{
  if (cbr >= 0.8)
    return 10.0;
  else if (0.5 < cbr && cbr < 0.8)
    return 20.0 - ((20-10)/(0.8-0.5))*(cbr - 0.5);
  else
    return 20.0;

}

// do the power control
void
LTEV2X_System::UpdatePowerForVID (double timeMS, int vid)
{
  if (m_isPowerControlEnabled)
    {
      // double cbr = GetCBR (timeMS, vid);

      double cbr = m_vehicleList.at (vid).m_currentCBR;
      double fuk = GetPowerMappingByCBR (cbr);
      double pk = m_vehicleList.at (vid).m_currentTxPower;

      /* now update power with smoothing -> see Eq. (10) in SAE J2945 */
      m_vehicleList.at (vid).m_currentTxPower = pk + m_vSUPRAGain * (fuk-pk);

      // NS_LOG_INFO (" vid: " << vid << " time: " << timeMS <<
      //             " power:" << m_vehicleList.at (vid).m_currentTxPower <<
      //             " fuk: " << fuk << " pk: " << pk);

    }

}


double 
LTEV2X_System::GetTrackingErrorForVID (double timeMS, int vid)
{
  Vector3D currPos = GetPositionByVID (vid);
  Vector3D lastPos = m_vehicleList.at (vid).m_lastTxPosition;

  if (lastPos.x == LARGEVALUE || lastPos.y == LARGEVALUE || lastPos.z == LARGEVALUE)
    return 0.0; // first packet not transmitted yet

  /* return the difference of two postions */
  return std::hypot(currPos.x-lastPos.x, currPos.y-lastPos.y);

}

/* returns transmission probability p(k) from eq (5) */
double 
LTEV2X_System::GetTransmissionProbability (double ek)
{
  if (m_vTrackingErrMin <= ek && ek < m_vTrackingErrMax)
    {
      double t1 = std::abs (ek - m_vTrackingErrMin);
      return 1 - std::exp (-m_vErrSensitivity * (t1 * t1));
    }
  else if (ek >= m_vTrackingErrMax)
    return 1.0;
  else
    return 0.0;

}



void 
LTEV2X_System::SaveLastTXPostionForVID (int vid)
{
  m_vehicleList.at (vid).m_lastTxPosition = GetPositionByVID (vid);
}



Vector3D
LTEV2X_System::GetPositionByVID (int vid)
{

  Ptr<MobilityModel> vMobility = m_vehicleContainer->Get (vid)->GetObject<MobilityModel> ();

  /* return position */
  return vMobility->GetPosition ();

}

auto
LTEV2X_System::GetPositionNSpeedByVID (int vid)
{

  // struct to contain vehicle position and speed
  struct result {
    Vector3D pos;
    Vector3D speed;
  };

  Ptr<MobilityModel> vMobility = m_vehicleContainer->Get (vid)->GetObject<MobilityModel> ();

  /* return position and speed */
  return result {vMobility->GetPosition (), vMobility->GetVelocity ()};

}

/* save resource reselection info as Hongseng mentioned */

void
LTEV2X_System::SaveResourceSelectionInfo (double timeMS, int vid)
{
  // get position and speed
  auto vPS = GetPositionNSpeedByVID (vid);

  /* save only if the vehicle is within area-of-interest */
  if (vPS.pos.x >= m_aoiX1 && vPS.pos.x <= m_aoiX2)
    {
      /* save this info to internal data structure */
      m_vehicleList.at (vid).m_logResourceSelectionSFInfo.push_back (timeMS);
    }


}

/* returns the pathloss (dB) by TX and RX id */
double
LTEV2X_System::GetPathLossByTXnRXID (int txid, int rxid)
{
  Ptr<MobilityModel> txMobility = m_vehicleContainer->Get (txid)->GetObject<MobilityModel> ();
  Ptr<MobilityModel> rxMobility = m_vehicleContainer->Get (rxid)->GetObject<MobilityModel> ();

  // get the distance between vehicles
  double dist = LTEV2X_Helper::GetDistanceByMobilityModelPair (txMobility, rxMobility);

  // // set antenna height if not set
  // Vector tv = txMobility->GetPosition ();
  // if (tv.z != m_vehicleAntennaHeight)
  //   {
  //     tv.z = m_vehicleAntennaHeight;
  //     txMobility->SetPosition (tv);
  //   }

  // Vector rv = rxMobility->GetPosition ();
  // if (rv.z != m_vehicleAntennaHeight)
  //   {
  //     rv.z = m_vehicleAntennaHeight;
  //     rxMobility->SetPosition (rv);
  //   }


  double pathlossDb = ChannelInfo::GetPathLoss (txMobility, rxMobility, dist, m_carrierFrequency); // dB
  return pathlossDb;

}

/* returns fast fading complex number */
std::complex<double>
LTEV2X_System::GetFastFadingComplex (double dist)
{
  double gamma =  ChannelInfo::m_nakagamiGamma;
  double m =   ChannelInfo::GetFadingCoefByDist (dist);

  double fastFadingMagLinear = LTEV2X_Helper::GetNakagamiRandVar (m, gamma); // linear

  //double fastFadingMagDb = 10 * std::log10 (fastFadingMagLinear); // dB


  double randAngleDegree;
  randAngleDegree = LTEV2X_Helper::GetRandomNumberInRange (0, 360);
  double hffTheta = degreesToRadians (randAngleDegree);

  std::complex<double> fastFadingComplex = std::polar (abs (fastFadingMagLinear), hffTheta);
  return fastFadingComplex;

}



/* Get common coefficients */
auto
LTEV2X_System::GetCommonFFCoef (int chid, int txid, int rxid)
{
  /* used to return the data */
  struct result
  {
    // fading coeeficients
    std::complex<double> h1FastFadingComplex;
    std::complex<double> h2FastFadingComplex;
  };

  Ptr<MobilityModel> txMobility = m_vehicleContainer->Get (txid)->GetObject<MobilityModel> ();
  Ptr<MobilityModel> rxMobility = m_vehicleContainer->Get (rxid)->GetObject<MobilityModel> ();

  // get the distance between vehicles
  double dist = LTEV2X_Helper::GetDistanceByMobilityModelPair (txMobility, rxMobility);

  // double pathlossDb = ChannelInfo::GetPathLoss (txMobility, rxMobility, dist, m_carrierFrequency); // dB
  double gamma =  ChannelInfo::m_nakagamiGamma;
  double m =   ChannelInfo::GetFadingCoefByDist (dist);

  double h1FastFadingMagLinear = LTEV2X_Helper::GetNakagamiRandVar (m, gamma); // linear
  double h2FastFadingMagLinear = LTEV2X_Helper::GetNakagamiRandVar (m, gamma); // linear
  // double h1FastFadingMagDb = 10 * std::log10 (h1FastFadingMagLinear); // dB
  // double h2FastFadingMagDb = 10 * std::log10 (h2FastFadingMagLinear); // dB


  double randAngleDegree;
  randAngleDegree = LTEV2X_Helper::GetRandomNumberInRange (0, 360);
  double h1ffTheta = degreesToRadians (randAngleDegree);

  randAngleDegree = LTEV2X_Helper::GetRandomNumberInRange (0, 360);
  double h2ffTheta = degreesToRadians (randAngleDegree);

  std::complex<double> h1FastFadingComplex = std::polar (abs (h1FastFadingMagLinear), h1ffTheta);
  std::complex<double> h2FastFadingComplex = std::polar (abs (h2FastFadingMagLinear), h2ffTheta);

  // return params (in dBm/dB)
  return result{h1FastFadingComplex, h2FastFadingComplex};

}



CSIComponents
LTEV2X_System::GetChannelQuality (int chid, int txid, int rxid)
{
  Ptr<MobilityModel> txMobility = m_vehicleContainer->Get (txid)->GetObject<MobilityModel> ();
  Ptr<MobilityModel> rxMobility = m_vehicleContainer->Get (rxid)->GetObject<MobilityModel> ();

  // get the distance between vehicles
  double dist = LTEV2X_Helper::GetDistanceByMobilityModelPair (txMobility, rxMobility);

  // // set antenna height if not set
  // Vector tv = txMobility->GetPosition ();
  // if (tv.z != m_vehicleAntennaHeight)
  //   {
  //     tv.z = m_vehicleAntennaHeight;
  //     txMobility->SetPosition (tv);
  //   }

  // Vector rv = rxMobility->GetPosition ();
  // if (rv.z != m_vehicleAntennaHeight)
  //   {
  //     rv.z = m_vehicleAntennaHeight;
  //     rxMobility->SetPosition (rv);
  //   }

  double pathlossDb = ChannelInfo::GetPathLoss (txMobility, rxMobility, dist, m_carrierFrequency); // dB
  double gamma =  ChannelInfo::m_nakagamiGamma;
  double m =   ChannelInfo::GetFadingCoefByDist (dist);

  double h1FastFadingMagLinear = LTEV2X_Helper::GetNakagamiRandVar (m, gamma); // linear
  double h2FastFadingMagLinear = LTEV2X_Helper::GetNakagamiRandVar (m, gamma); // linear
  double h1FastFadingMagDb = 10 * std::log10 (h1FastFadingMagLinear); // dB
  double h2FastFadingMagDb = 10 * std::log10 (h2FastFadingMagLinear); // dB


  double randAngleDegree;
  randAngleDegree = LTEV2X_Helper::GetRandomNumberInRange (0, 360);
  double h1ffTheta = degreesToRadians (randAngleDegree);

  randAngleDegree = LTEV2X_Helper::GetRandomNumberInRange (0, 360);
  double h2ffTheta = degreesToRadians (randAngleDegree);

  std::complex<double> h1FastFadingComplex = std::polar (abs (h1FastFadingMagLinear), h1ffTheta);
  std::complex<double> h2FastFadingComplex = std::polar (abs (h2FastFadingMagLinear), h2ffTheta);

  // dBm
  double txPower = m_vehicleList.at (txid).m_currentTxPower;
  double pPSSCH = GetPSSCHPowerDbmFromFactor (txPower);  // PSSCH power (dBm)
  double pRx1 = txPower - (pathlossDb + h1FastFadingMagDb);
  double pRx2 = txPower - (pathlossDb + h2FastFadingMagDb);

  double pRxRsrp1 = pPSSCH - (pathlossDb + h1FastFadingMagDb);
  double pRxRsrp2 = pPSSCH - (pathlossDb + h2FastFadingMagDb);

  // return params (in dBm/dB)
  return {
  .h1Rssi = pRx1, // dBm
  .h2Rssi = pRx2, // dBm
  .h1Rsrp = pRxRsrp1, // dBm
  .h2Rsrp = pRxRsrp2, // dBm
  .rssi = std::max(pRx1, pRx2),  // dBm
  .rsrp = std::max(pRxRsrp1, pRxRsrp2),  // dBm
  .pathloss = pathlossDb,  // dB
  //.shadowing = shadowingDb, [no shadowing for now]
  .h1FastFadingMagDb = h1FastFadingMagDb,
  .h2FastFadingMagDb = h2FastFadingMagDb,
  .h1FastFadingComplex = h1FastFadingComplex,
  .h2FastFadingComplex = h2FastFadingComplex
 };




}

/* return the TX ids for a given subchannel n, n+1 and the current time */
std::vector<int>
LTEV2X_System::GetTXListByChidN (double timeMS, int chidN)
{
  std::vector<int> vidList1, vidList2, outTXList;

  SFUsageMap sfum;
  SFUsageMap::const_iterator it;

  /* for first subchannel */
  sfum = m_SFReserveAllChannelList.at (chidN);
  it = sfum.find(timeMS);

  if (it != sfum.end())
    vidList1 = it->second; // extract the elements;

  /* for second subchannel */
  sfum = m_SFReserveAllChannelList.at (chidN+1);
  it = sfum.find(timeMS);

  if (it != sfum.end())
    vidList2 = it->second; // extract the elements;

  if (vidList1.empty () || vidList2.empty ())
    return {};  // return empty list

  // vectors are not empty
  for (std::vector<int>::iterator i = vidList1.begin(); i != vidList1.end(); ++i)
    {
      if (std::find(vidList2.begin(), vidList2.end(), *i) != vidList2.end())
        {
          outTXList.push_back(*i);
        }
    }

  return outTXList;  // return the common TX id in both subchannels

}

/* calculate IBE -> see Takayuki's MATLAB script */
std::vector<double>
LTEV2X_System::CalcIBE (int nRB, int lcrb, int rbStart, double pRB)
{

  // NS_LOG_INFO ("rbStart:" << rbStart << " lcrb: " << lcrb);
  // rbStart = 11;  // dummy

  /*
  EVM      = 0.175; % EVM for QPSK
  W        = 3;     % W [dB] (ref: 3GPP TR 36.885)
  X        = 6;     % X [dB] (ref: 3GPP TR 36.885)
  Y        = 3;     % Y [dB] (ref: 3GPP TR 36.885)
  Z        = 3;     % Z [dB] (ref: 3GPP TR 36.885)
 */
  double evm = 0.175;
  double w = 3.0;
  double x = 6.0;
  double y = 3.0;
  double z = 3.0;
  double infVal = -std::numeric_limits<double>::infinity ();
  // double infVal = 0.0;

  // occupied band
  std::vector<double> occupied(nRB, infVal);
  for (int i = rbStart; i < rbStart + lcrb; i++)
    occupied.at (i-1) = 0; // index is 1 less

  // general band
  std::vector<double> general(nRB, infVal);
  std::vector<double> deltaRB(nRB, 0);
  int idx = 0;
  for (int val = rbStart-1; val > 0; val--)
    {
      deltaRB.at (idx) = val;
      idx++;
    }


  for (int i=0; i < lcrb; i++)
    {
      deltaRB.at (idx) = 0;
      idx++;
    }



  for (int i=1; i < nRB-(rbStart+lcrb); i++)
    {
      deltaRB.at (idx) = i;
      idx++;
    }


  double aa = -25-10*std::log10 (nRB/lcrb)-x;

  double cc = -57 - pRB - x;

  for (size_t i = 0; i < general.size(); i++)
    {
      double bb = 20*std::log10 (evm) - 3 - 5*(abs (deltaRB.at (i))-1)/(double) lcrb - w;
      general.at (i) = std::max({aa, bb, cc});
      // NS_LOG_INFO ("i: " << i<< "-> general[i]:" << general.at (i));
    }

  // IQ Image
  std::vector<double> iqi(nRB, infVal);
  double valIQI = -25.0 - y;

  for (int i=nRB-rbStart+1; i >= nRB-rbStart+2-lcrb; i--)
    {
      // NS_LOG_INFO ("rbStart:" << rbStart << " lcrb: " << lcrb << "iqi_mask" << i);
      iqi.at (i-1) = valIQI; // index is 1 less
      //  NS_LOG_INFO ("indx: " << i-1<< "-> iqi[indx]:" << iqi.at (i-1));

    }

  // Carrier Leakage
  std::vector<double> cl(nRB, infVal);
  for (int i=(int)nRB/2; i <= (int) nRB/2 + 1;  i++)
    cl.at (i-1) = -25.0 - z;


  // Combined IBE as the power sum of General, IQ image, and Carrier leakage
  std::vector<double> ibeSum(nRB, 0);
  for (size_t i = 0; i < ibeSum.size(); i++)
    {
      double t1 = std::pow (10, occupied.at (i)/10.0) + std::pow (10, general.at (i)/10.0) + std::pow (10, iqi.at (i)/10.0) + std::pow(10, cl.at (i)/10.0);
      // NS_LOG_INFO ("t1 is: " << t1 << "general: " << general.at (i));
      ibeSum.at (i) = 10*std::log10 (t1); // Linear sum
      // NS_LOG_INFO ("rbStart:" << rbStart << " lcrb: " << lcrb << " t1 is: " << t1 << " ibesub: " << ibeSum.at (i));

    }



  // NS_LOG_INFO ("===BEGIN===");
  // for (size_t index = 0; index < deltaRB.size(); ++index)
  //   NS_LOG_INFO ("rbStart:" << rbStart << " lcrb: " << lcrb << " indx:" << index << " ==> deltaRB[i]:" << deltaRB[index]);
  // NS_LOG_INFO ("===END===");

  // NS_LOG_INFO ("===BEGIN===");
  // for (size_t index = 0; index < ibeSum.size(); ++index)
  //   NS_LOG_INFO ("rbStart:" << rbStart << " lcrb: " << lcrb << " indx:" << index << " ==> ibeSum[i]:" << ibeSum[index]);
  // NS_LOG_INFO ("===END===");

  return ibeSum;


}


/* do the IBE calculations */
auto
LTEV2X_System::GetIBEByOccupiedChIDn (int occupiedChidN, int primaryChid, int txid, int rxid)
{

  /* used to return the data */
  struct result
  {
    double intfPowDBM;  // interference power (for SINR)
    double inftPowrLossDBMrssi; // interference (power-loss) for RSSI
  };

  if (occupiedChidN == primaryChid || occupiedChidN+1 == primaryChid)
    return result {-INFINITY, -INFINITY}; // return zero [since this is already in primary interference]



  double pTx = m_vehicleList.at (txid).m_currentTxPower; // Total Transmission Power for PSCCH and PSSCH [dBm]

  /* first two for PSCCH, rest 18 for PSSCH */
  const int lcrbControl = 2;
  const int lcrbData = 18;
  const int nRBperSubCh = 10;
  const int nRB = nRBperSubCh * m_nSubChannels; // total RB: 100 for 20 MHz, 50 for 10 MHz


  /* NOTE: we use this with 1 indexing ->
  * say for subchannel 1: 1-10
  * subchannel 2: 11-20 and so on
  */
  int rbStart = occupiedChidN * nRBperSubCh + 1; // Start RB index of Occupied Transmission Bandwidth [PRB]

  int rbStartControl = rbStart; // PSCCH (first two)
  int rbStartData = rbStart + 2; // PSSCH (data, rest 18 RBs)

  // Tx Power of PSCCH is 3 dB higher than PSSCH
  double pControl =  pTx + 10 * std::log10 (2.0/3.0);
  double pData = pTx + 10 * std::log10 (1.0/3.0);

  // Transmission power per PRB [dBm]
  double pRBControl = pControl - 10 * std::log10 (lcrbControl*1.0);
  double pRBData = pData - 10 * std::log10 (lcrbData*1.0);

  // NS_LOG_INFO ("txid: " << txid << " ptx:" << pTx << " pRBControl:" << pRBControl << " pRBData:" << pRBData << " rxid: " << rxid);


  std::vector<double> ibeSumControl = CalcIBE (nRB, lcrbControl, rbStartControl, pControl);
  std::vector<double> ibeSumData = CalcIBE (nRB, lcrbData, rbStartData, pData);


  std::vector<double> ibeSumDBM(nRB, 0);

  for (int i = 0; i < nRB; i++)
    {
      // Overall in-band emission [dBm/PRB] (ref: 3GPP TS 36.101 V14.11.0 Table 6.5.2.3.1-1)
      double ibeSumControlDBM = ibeSumControl.at (i) + pRBControl;
      double ibeSumDataDBM = ibeSumData.at (i) + pRBData;
      double t1 = pRBData-30;


      double t2 = 10 * std::log10( std::pow(10, ibeSumControlDBM/10.0) + std::pow(10, ibeSumDataDBM/10.0) );

      // NS_LOG_INFO ("t1 is: " << t1 << "ibeSumControlDBM:" << ibeSumControlDBM << " t2 is: " << t2);
      ibeSumDBM.at (i) = std::max (t1, t2);
    }


  double intPowInSubCh = 0.0;  // dBm
  int begIndx =  primaryChid*nRBperSubCh;
  int endIndx = begIndx + nRBperSubCh -1;

  double tWatt = 0.0;  // a temporary variable
  for (int i = begIndx; i <= endIndx; i++)
    {
      tWatt += LTEV2X_Helper::DbmToWatt (ibeSumDBM.at (i));
      // NS_LOG_INFO (" occupiedChN: " << occupiedChidN <<  " primaryCh: " << primaryChid <<
      //             " i: " << i << " current sidelobe intf pw (watt): " << tWatt <<
      //             " dBm: " << LTEV2X_Helper::WattToDbm (tWatt));
    }


  if (tWatt == 0.0)
    return result {-INFINITY, -INFINITY}; // return zero

  intPowInSubCh = LTEV2X_Helper::WattToDbm (tWatt); // get dBm value

  // NS_LOG_INFO ("txid: " << txid << " primary chid: " << primaryChid << " total sidelobe intf pw:" << intPowInSubCh);

  /* calculate interference (power - pl) */
  double pathlossDb = GetPathLossByTXnRXID (txid, rxid);
  double dist = GetDistanceByTxRxVID (txid, rxid);
  double gamma =  ChannelInfo::m_nakagamiGamma;
  double m =   ChannelInfo::GetFadingCoefByDist (dist);

  double h1FastFadingMagLinear = LTEV2X_Helper::GetNakagamiRandVar (m, gamma); // linear
  double h2FastFadingMagLinear = LTEV2X_Helper::GetNakagamiRandVar (m, gamma); // linear
  double h1FastFadingMagDb = 10 * std::log10 (h1FastFadingMagLinear); // dB
  double h2FastFadingMagDb = 10 * std::log10 (h2FastFadingMagLinear); // dB

  // dBm

  double pRx1 = intPowInSubCh - (pathlossDb + h1FastFadingMagDb);
  double pRx2 = intPowInSubCh - (pathlossDb + h2FastFadingMagDb);

  double pRx = std::max(pRx1, pRx2);  // return from two antennas (dBm)


  return result {intPowInSubCh, pRx}; // return both power and power-gain

}





/* returns in band emmission for SINR calculation (watt) */
double
LTEV2X_System::CalculateIBEIntfForSINRWatt (double timeMS, int primaryChid, int rxid,  std::complex<double> h1TxComplex, std::complex<double> h2TxComplex)
{
  double ibeIntfWatt = 0.0; //  in watt

  /* get all the TX list use different subchannels and sum all the sidelobe inteference from them */
  for (int chid = 0; chid < m_nSubChannels-1; chid++ )
    {
      std::vector<int> intfTXList = GetTXListByChidN (timeMS, chid);
      if (!intfTXList.empty ())
        {
          for (auto ixid : intfTXList)
            {
              if (ixid != rxid)
                {
                  // NS_LOG_INFO ("txid: " << txid << " primary chid: " << primaryChid << " rxid:" << rxid );
                  auto result = GetIBEByOccupiedChIDn (chid, primaryChid, ixid, rxid);

                  if (result.intfPowDBM != -INFINITY)
                    {

                      double infTxPowerWatt = LTEV2X_Helper::DbmToWatt (result.intfPowDBM);
                      double pathLoss = GetPathLossByTXnRXID (ixid, rxid);
                      double intfPlLinear = std::pow(10, -pathLoss/10.0);
                      double dist = GetDistanceByTxRxVID (ixid, rxid);
                      std::complex<double> h1InfComplex = GetFastFadingComplex (dist);
                      std::complex<double> h2InfComplex = GetFastFadingComplex (dist);
                      ibeIntfWatt += GetIntfReceivedPowerWatt (h1TxComplex, h2TxComplex,
                                                  infTxPowerWatt, intfPlLinear, h1InfComplex, h2InfComplex);

                      // NS_LOG_INFO ("Time: " << timeMS << " primaryChid: " << primaryChid <<
                      //             " ixid: " << ixid << " rxid:" << rxid <<
                      //             " ix-rx-dist:" << GetDistanceByTxRxVID (ixid, rxid) <<
                      //             " p_ix (dBm): " << LTEV2X_Helper::WattToDbm (infTxPowerWatt) );

                    }




                }
            }
        }
    }
  return ibeIntfWatt; // return total interference (power - gain in dBm)

}


/* returns in band emmission (Watt) */
double
LTEV2X_System::CalculateIBEIntfWatt (double timeMS, int primaryChid, int rxid)
{
  double ibeIntfWatt = 0.0;

  /* get all the TX list use different subchannels and sum all the sidelobe inteference from them */
  for (int chid = 0; chid < m_nSubChannels-1; chid++ )
    {


      std::vector<int> intfTXList = GetTXListByChidN (timeMS, chid);
      // if (timeMS > 1000)
      //   NS_LOG_INFO ("time:" << timeMS << " chid: (" << chid << "," << chid+1 << ") primary: " << primaryChid << " rxid: " << rxid << " in ibe! -> nIntfTX:" << intfTXList.size ());

      if (!intfTXList.empty ())
        {
          for (auto txid : intfTXList)
            {
              if (txid != rxid)
                {
                  // NS_LOG_INFO ("txid: " << txid << " primary chid: " << primaryChid << " rxid:" << rxid );
                  auto result = GetIBEByOccupiedChIDn (chid, primaryChid, txid, rxid);
                  if (result.inftPowrLossDBMrssi != -INFINITY)
                    ibeIntfWatt += LTEV2X_Helper::DbmToWatt (result.inftPowrLossDBMrssi);

                }

            }

        }


    }
  return ibeIntfWatt; // return total interference (power - gain in Watt)
}


/* returns channel busy ratio for a given SF [timeMS] */
double
LTEV2X_System::GetCBR (double timeMS, int vid)
{
  if (timeMS < m_sensingDuration)
    return 0.0;  // we measure after 1000 MS

  int nSF = 100;
  double totalResource = (double) m_nSubChannels * nSF;
  int count = 0;

  for (int chid = 0; chid < m_nSubChannels; chid++)
    {
      for (int tms = (int) timeMS - nSF; tms < (int) timeMS; tms++)
        {
          // NS_LOG_INFO ("chid: " << chid << " vid: " << vid << "tms: " << tms << " rssi: " << m_vehicleList.at (vid).m_localSFCSI.at (chid)[tms].m_rssi);

          if (m_vehicleList.at (vid).m_localSFCSI.at (chid)[tms].m_rssi > m_thCBR)
            {
              // NS_LOG_INFO ("chid: " << chid << " vid: " << vid << "tms: " << tms << " rssi: " << m_vehicleList.at (vid).m_localSFCSI.at (chid)[tms].m_rssi);
              count++;
            }
        }
    }

  return (double) count/totalResource; // return the CBR

}

/* returns the Max_ITT -> SAE J3161 Eq (7). Input: vehicle density */
double
LTEV2X_System::GetMaxITT (double vd)
{
  // double vd = m_vehicleList.at (vid).m_smoothVehicleDensity;

  // int r = LTEV2X_Helper::GetRandomNumberInRange(-10, 10);  // +- 10 bias as in the slides
  int r = 0; // no bias as Takayuki suggested

  if (vd <= 25)
    return (double) (100.0 + r);
  else if (vd > 25 && vd <= 150)
    {
      double d = (double) (100.0 * (vd/25.0) + r);
      return round (d);
    }
  else
    return double (600.0 + r);
}


/* update CBR for VID */
void
LTEV2X_System::UpdateCBRForVID (double timeMS, int vid)
{
  // if (!m_isFixedT2Window)
  //   {
  //     /* update CBR */
  //     m_vehicleList.at (vid).m_currentCBR = GetCBR (timeMS, vid);
  //   }

  /* update CBR when required */

  if (m_isFixedT2Window == false || m_isPowerControlEnabled == true)
    {
      /* update CBR */
      m_vehicleList.at (vid).m_currentCBR = GetCBR (timeMS, vid);
    }
  

}

void
LTEV2X_System::UpdateSmoothVehicleDensity (double timeMS, int vid)
{
  
  /*  based on tracking error 
      we need to do this anyways regardless of rate control
  */

  double nextScheduledMsgTime = m_vehicleList.at (vid).m_PckArivSF; // supposed to be here
  double lastTxTime = m_vehicleList.at (vid).m_PrevTXSF; // last msg generation time


  double ek = GetTrackingErrorForVID (timeMS, vid);

  if (ek != 0)
    {
      double pk = GetTransmissionProbability (ek);
      double rv = LTEV2X_Helper::GetRandomRealNumberInRange (0, 1);

      if ( (rv <= pk) && ((nextScheduledMsgTime - timeMS) >= m_vRescheduleTh) )
        m_vehicleList.at (vid).m_txDecisionDynamics = true;
      else
        m_vehicleList.at (vid).m_txDecisionDynamics = false;

      // NS_LOG_INFO (" vid: " << vid <<  " current time: " << timeMS << " ek: " << ek << " txdynamics:" << m_vehicleList.at (vid).m_txDecisionDynamics);

      if (m_vehicleList.at (vid).m_txDecisionDynamics && nextScheduledMsgTime != lastTxTime)
        {
          m_vehicleList.at (vid).m_PckArivSF = timeMS; // schedule the packet now 
          // no need to check IIT value then
          return;
        }
    }



  /* TX decision based on ITT (only in rate control enabled) */

  if (m_isCongestControlEnabled)
    {
      // double nVehles = (double) GetNeighborVehicleForVID (vid, m_neighborVehicleSearchTh);
  
      /* get neighbour count based on suceessful packet reception */
      double nVehles = (double) GetNeighborCountBySuccReceptionForVID (timeMS, vid, m_neighborVehicleSearchTh);

      // smooth as in Eq. (6) for the SAE J3161
      m_vehicleList.at (vid).m_smoothVehicleDensity = m_vDensityWeightFactor * nVehles + (1 - m_vDensityWeightFactor) * m_vehicleList.at (vid).m_smoothVehicleDensity;


      /* perform this operation -> see specs */
      double maxITT = GetMaxITT (m_vehicleList.at (vid).m_smoothVehicleDensity);
      // double nextScheduledMsgTime = m_vehicleList.at (vid).m_PckArivSF; // supposed to be here
      // double lastTxTime = m_vehicleList.at (vid).m_PrevTXSF; // last msg generation time


      if (nextScheduledMsgTime - (lastTxTime + maxITT) >= m_vRescheduleTh)
        m_vehicleList.at (vid).m_txDecisionMaxITT = true;
      else
        m_vehicleList.at (vid).m_txDecisionMaxITT = false;

      /* if the flag is true update scheduled transmission */
      // if (m_vehicleList.at (vid).m_txDecisionMaxITT)
      if (m_vehicleList.at (vid).m_txDecisionMaxITT && nextScheduledMsgTime != lastTxTime)
        m_vehicleList.at (vid).m_PckArivSF = std::max(timeMS, lastTxTime + maxITT);


      // NS_LOG_INFO (" vid: " << vid <<  " current time: " << timeMS << " last pck: " << lastTxTime << " next pck:" << m_vehicleList.at (vid).m_PckArivSF);
      // NS_LOG_INFO (" vid: " << vid << " current time: " << timeMS << " last pck: " << lastTxTime << " next scheduled pck:" <<  nextScheduledMsgTime);

      
    }


  

}

/* returns SINR, distance and corresponding TX id */
auto
LTEV2X_System::GetSINRFromInterfererList (double timeMS, int vid, std::vector<InterfererInfo> intfInfoVec)
{
  struct result
  {
    // sinr and distance between TX-RX
    double sinr; double dist; int txid;
  };

  TXStateWithPckIDInfo tsinfo = IsVehicleInTXMode (timeMS, vid);
  bool isTXMode = tsinfo.isTX;
  // bool isTXMode = IsVehicleInTXMode (timeMS, vid);

  if (isTXMode)
    return result {VERYLOWSINRDB, -1, -1}; /* vehicle is in TX mode. This transmission is unsucessful anyways */

  /* thermal noise */
  double n0 = ChannelInfo::GetThermalNoise ();  // in dBm
  n0 =  LTEV2X_Helper::DbmToWatt (n0); // in watt

  double maxSinrDb = VERYLOWSINRDB; // initialize to a very low SINR
  double maxDist = -1;  // distance betwee TX-RX
  int maxTXid = -1;  // id of the successful TX
  for ( const auto& tv : intfInfoVec )
    {
      int txid = tv.txid;
      double pRx = tv.pRxSINRWatt;  // Pr_MRC in the Equation [see specs]

      std::complex<double> h1TxComplex = tv.h1FastFadingComplex;
      std::complex<double> h2TxComplex = tv.h2FastFadingComplex;

      double intfRx = 0.0;

      for ( const auto& iv : intfInfoVec )
        {
          int ixid = iv.txid;  // id of the interfering vehicle

          if (ixid != txid)
            {
              /*  all are linear domain */
              double infPl =  iv.plLinear;
              double infTxPower =  iv.txPowerWatt;
              std::complex<double> h1InfComplex = iv.h1FastFadingComplex;
              std::complex<double> h2InfComplex = iv.h2FastFadingComplex;
              intfRx += GetIntfReceivedPowerWatt (h1TxComplex, h2TxComplex,
                                                  infTxPower, infPl, h1InfComplex, h2InfComplex);
            }

        }

      double pSignalDb = 10 * std::log10 (pRx);
      double pNoiseDb = 10 * std::log10 (intfRx + n0);
      double sinrDb = pSignalDb - pNoiseDb;
      sinrDb = GetAvgSINRPerRxAntennaDb (sinrDb); // get the average of two antenna (dB)

      if (sinrDb > maxSinrDb)
        {
          maxSinrDb = sinrDb;  // update
          maxDist = GetDistanceByTxRxVID (txid, vid);
          maxTXid = txid;

        }




    }


  /* return SINR and corresponding TX-RX distance */
  return result {maxSinrDb, maxDist, maxTXid};


}

/* returns SINR, distance and corresponding TX id */
double
LTEV2X_System::GetSINRnDistForTXnRXnInterfererList (double timeMS, int chid, int txid, int rxid, std::vector<InterfererInfo> allTXInfoVec, std::complex<double> h1TxComplex, std::complex<double> h2TxComplex)
{
  // struct result
  // {
  //   // sinr and distance between TX-RX
  //   double sinr; double dist;
  // };

  bool found = false;
  double pRx = -1.0;
  // std::complex<double> h1TxComplex;
  // std::complex<double> h2TxComplex;

  // for ( const auto& tv : allTXInfoVec )
  //   {
  //     if (txid == tv.txid)
  //       {
  //         found = true;
  //         pRx = tv.pRxSINRWatt;  // Pr_MRC in the Equation [see specs]

  //         h1TxComplex = tv.h1FastFadingComplex;
  //         h2TxComplex = tv.h2FastFadingComplex;

  //         break;

  //       }

  //   }

  if (!found)
    {
      // NS_LOG_INFO ("time: " << timeMS << " txid: " << txid << " not found!");
      // return result {VERYLOWSINRDB, -1};  // return null
      return VERYLOWSINRDB;

    }



  // now calculate interference

  /* thermal noise */
  double n0 = ChannelInfo::GetThermalNoise ();  // in dBm
  n0 =  LTEV2X_Helper::DbmToWatt (n0); // in watt

  double intfRx = 0.0;

  for ( const auto& iv : allTXInfoVec )
    {
      int ixid = iv.txid;  // id of the interfering vehicle

      /* add those are interferer */
      if (ixid != txid)
        {
          /*  all are linear domain */
          double infPl =  iv.plLinear;
          double infTxPower =  iv.txPowerWatt;
          std::complex<double> h1InfComplex = iv.h1FastFadingComplex;
          std::complex<double> h2InfComplex = iv.h2FastFadingComplex;
          intfRx += GetIntfReceivedPowerWatt (h1TxComplex, h2TxComplex,
                                              infTxPower, infPl, h1InfComplex, h2InfComplex);
        }

    }

  /* add the interference due to IBE */
  double ibeIntfWatt = CalculateIBEIntfForSINRWatt (timeMS, chid, rxid, h1TxComplex, h2TxComplex); // watt
  intfRx += ibeIntfWatt; // add ibe interfrerence (all with MCR compatible)


  /* now calculate SINR */
  if (found)
    {
      double pSignalDb = 10 * std::log10 (pRx);
      double pNoiseDb = 10 * std::log10 (intfRx + n0);
      double sinrDb = pSignalDb - pNoiseDb;
      sinrDb = GetAvgSINRPerRxAntennaDb (sinrDb); // get the average of two antenna (dB)
      // double dist = GetDistanceByTxRxVID (txid, rxid);
      /* return SINR and corresponding TX-RX distance */
      // return result {sinrDb, dist};
      return sinrDb;

    }
  else
    // return result {VERYLOWSINRDB, -1};  // return null
    return VERYLOWSINRDB;  // return null



}

void
LTEV2X_System::DoUpdateCSI (double timeMS, int vid, int chid)
{
  double rssi = 0.0;
  double rsrp = 0.0;
  // double sinr;
  // double dist;

  double n0 = ChannelInfo::GetThermalNoise ();  // in dBm
  n0 =  LTEV2X_Helper::DbmToWatt (n0); // in watt
  bool isTXMode = false;

  std::vector<InterfererInfo> intfInfoVec;  // contain information about all interferer

  SFUsageMap sfum = m_SFReserveAllChannelList.at (chid);
  // std::vector<int> vidList = m_SFReserveAllChannelList.at (chid);
  // NS_LOG_INFO ("chid: " << chid << " sfum address: " << &sfum);

  SFUsageMap::const_iterator it = sfum.find(timeMS);

  if (it != sfum.end())
    {
      std::vector<int> vidList = it->second; // extract the elements;
      if (!vidList.empty ())
        {
          // NS_LOG_INFO ("chid: " << chid << " vid: " << vid << " time: " << timeMS << " vidlist not empty!");
          // NS_LOG_INFO ("vidlist size: " << vidList.size ());
          for (std::size_t id = 0; id < vidList.size (); id++)
            {
              // NS_LOG_INFO ("chid: " << chid << " local_vid: " << vid << " time: " << timeMS << " vidlistid: " << vidList[id]);
              int txid = vidList[id];
              if (txid == vid)
                {
                  // ignore SF that are used for TX
                  rssi = LARGEVALUE;
                  rsrp = LARGEVALUE;
                  // sinr = VERYLOWSINRDB;
                  // dist = -1;
                  isTXMode = true;
                  break;
                }
              else
                {

                  // add the interference from those vehicles use the subchannel
                  auto csi = GetChannelQuality (chid, txid, vid);
                  rssi += LTEV2X_Helper::DbmToWatt (csi.rssi);
                  rsrp += LTEV2X_Helper::DbmToWatt (csi.rsrp);

                  double txPower = m_vehicleList.at (txid).m_currentTxPower;
                  double txPowerWatt = LTEV2X_Helper::DbmToWatt (txPower);
                  double plLinear = std::pow(10, -csi.pathloss/10.0);
                  double pRxSINRWatt = GetCombinedAntennaRxPowerWatt (txPowerWatt, abs (csi.h1FastFadingComplex), abs (csi.h2FastFadingComplex), plLinear);


                  InterfererInfo iInfo;
                  iInfo.txid = txid;
                  iInfo.plLinear = plLinear;
                  iInfo.pRxSINRWatt = pRxSINRWatt;
                  iInfo.h1FastFadingComplex = csi.h1FastFadingComplex;
                  iInfo.h2FastFadingComplex = csi.h2FastFadingComplex;

                  intfInfoVec.push_back (iInfo);

                }
            }
        }
    }
  // else
  //   {
  //     // NS_LOG_INFO ("chid: " << chid << " vid: " << vid << "time: " << timeMS << " key-val not found!");

  //   }

  if (!isTXMode)
    {
      rssi += n0; // add thermal noise

      /* add IBE interference */
      double ibeIntf = CalculateIBEIntfWatt (timeMS, chid, vid); // watt

      if (ibeIntf > 0)
        rssi +=  ibeIntf;

      rssi =  LTEV2X_Helper::WattToDbm (rssi); // change to dBm


      // if (timeMS > 1100)
        // NS_LOG_INFO ("chid: " << chid << " vid: " << vid << " time: " << timeMS << " rssi: (dBm) " << rssi << " inftIBE (w): " << ibeIntf << " inftIBE: (dBm)" <<  LTEV2X_Helper::WattToDbm (ibeIntf));


      /* we are not using rsrp actually (so IBE interference is not added) */
      rsrp += n0; // add thermal noise
      rsrp = LTEV2X_Helper::WattToDbm (rsrp); // change to dBm
    }

  /* We are not calculating SINR from receiver vehicle anymore (e.g. measured for each packet TX) */
  /* get the SINR and TX-RX distance */
  // auto result  = GetSINRFromInterfererList (timeMS, vid, intfInfoVec);

  // NS_LOG_INFO ("SINR is: " << sinr);

  /* now save those info */
  ChStateInfo chsi;
  // chsi.m_rsrp = rsrp; chsi.m_rssi = rssi; chsi.m_sinr = result.sinr; chsi.m_dist = result.dist; chsi.m_txid = result.txid;
  chsi.m_rsrp = rsrp; chsi.m_rssi = rssi; chsi.m_sinr = -1; chsi.m_dist = -1; chsi.m_txid = -1;



  // NS_LOG_INFO ("chid: " << chid << " vid: " << vid << " time: " << timeMS << " rssi: " << rssi << " sinr: " << sinr);

  // if (timeMS > 1100)
  //   NS_LOG_INFO ("chid: " << chid << " vid: " << vid << " time: " << timeMS << " rssi: " << rssi << " inftIBE:" << ibeIntf);

  m_vehicleList.at (vid).m_localSFCSI.at (chid)[timeMS] = chsi;  // save this info

}

void
LTEV2X_System::UpdateCSIbyVID (double timeMS, int vid)
{

  for (int chid=0; chid<m_nSubChannels; chid++)
    {
      DoUpdateCSI (timeMS, vid, chid);
    }

}

/* update next packet arrival time */

void
LTEV2X_System::UpdateNextPacketArrivalTimeForVid (int vid)
{
  /* updated with congestion (rate) control */
  if (m_isCongestControlEnabled)
    {
      m_vehicleList.at (vid).m_PrevTXSF =  m_vehicleList.at (vid).m_PckArivSF;  // save previous one

      double maxITT = GetMaxITT (m_vehicleList.at (vid).m_smoothVehicleDensity);

      m_vehicleList.at (vid).m_PckArivSF += maxITT;

    }
  else
    {
      m_vehicleList.at (vid).m_PrevTXSF =  m_vehicleList.at (vid).m_PckArivSF;
      m_vehicleList.at (vid).m_PckArivSF += m_defaultMsgArrivalMS;

    }

}

/* this will return variable window size based on channel busy ratio */
int
LTEV2X_System::GetWindowSizeT2 (double timeMS, int vid)
{
  if (m_isFixedT2Window)
    return m_selectionWindowFixedT2;
  else
    return  m_vehicleList.at (vid).m_selectionWindowT2;  // based on CBR

}

/* update CBR based window size T2 */
void
LTEV2X_System::UpdateCBRBasedWindowT2 (int vid)
{

  double t2;
  double cbr = m_vehicleList.at (vid).m_currentCBR;  // get the last obtained cbr

  if (cbr <= 0.25)
    t2 = 20;
  else if (cbr > 0.25 && cbr <= 0.75)
    t2 = std::min (m_PDB-10, 50.0 );
  else
    t2 = m_PDB - 10;

  m_vehicleList.at (vid).m_selectionWindowT2 = t2;  // update

  // NS_LOG_INFO ("time: " << timeMS << " vid: " << vid << " cbr: " << cbr << " windwoT2: " <<  m_vehicleList.at (vid).m_selectionWindowT2);



}

/* returns the interferer list for a given (TX) vid */
std::vector<int>
LTEV2X_System::GetAllTXListbyChId (double timeMS, int chid)
{
  SFUsageMap sfum = m_SFReserveAllChannelList.at (chid);
  SFUsageMap::const_iterator it = sfum.find(timeMS);
  if (it != sfum.end())
    {
      std::vector<int> txList = it->second;
      return txList; // extract the elements and return
    }
  else
    return {};  // return null

}

/* store which SF and which TX packet is successful */



void
LTEV2X_System::StoreSuccReceptionForRX (int txid, int rxid, double txtime)
{
  m_vehicleList.at (rxid).m_succPckRecepMMap.insert (std::pair<double, int> (txtime, txid) );
}

/* save PER info */

void
LTEV2X_System::SavePERForLogging (double arrivalTime, double txTime, int txid, int rxid, double dist, int per)
{

  // get position and speed
  auto txPS = GetPositionNSpeedByVID (txid);
  auto rxPS = GetPositionNSpeedByVID (rxid);
  // double dist = GetDistanceByTxRxVID (txid, rxid);



  // NS_LOG_INFO (" txid: " << txid << " rxid: " << rxid << " per: " <<  per << " dist: " << dist);


  /* save only if the TX-RX vehicles within area-of-interest */
  if (  (arrivalTime >= m_logStartTime) &&
        (txPS.pos.x >= m_aoiX1 && txPS.pos.x <= m_aoiX2) &&
        (rxPS.pos.x >= (m_aoiX1+m_rxPad) && rxPS.pos.x <= (m_aoiX2-m_rxPad) ) &&
        (dist <= m_distCap) )
    {
      /* creare object to save */
      LogInfoPER linfo;
      linfo.txid = txid;
      linfo.rxid = rxid;
      linfo.arrivalSF = arrivalTime;
      linfo.txSF = txTime;
      linfo.per = per;
      linfo.dist = dist;
      linfo.txPos = txPS.pos;
      linfo.txSpeed = txPS.speed;
      linfo.rxPos = rxPS.pos;
      linfo.rxSpeed = rxPS.speed;

      /* save this info to internal data structure */
      m_vehicleList.at (txid).m_logPERInfo.push_back (linfo);
    }


}

/* save SINR for a given channel */
void
LTEV2X_System::DoSaveTXRXSINRInfoForTXVIDByChId (double timeMS, int txid, int rxid, int chid)
{
  std::vector<int> txList = GetAllTXListbyChId (timeMS, chid);

  if (!txList.empty ())
    {


      std::vector<InterfererInfo> allTXInfoVec;  // contain information about all tx
      // those are all tx list
      for (auto id: txList)
        {
          auto csi = GetChannelQuality (chid, id, rxid);
          double txPower = m_vehicleList.at (id).m_currentTxPower;
          double txPowerWatt = LTEV2X_Helper::DbmToWatt (txPower);
          double plLinear = std::pow(10, -csi.pathloss/10.0);
          double pRxSINRWatt = GetCombinedAntennaRxPowerWatt (txPowerWatt, abs (csi.h1FastFadingComplex), abs (csi.h2FastFadingComplex), plLinear);


          InterfererInfo iInfo;
          iInfo.txid = id;
          iInfo.plLinear = plLinear;
          iInfo.pRxSINRWatt = pRxSINRWatt;
          iInfo.h1FastFadingComplex = csi.h1FastFadingComplex;
          iInfo.h2FastFadingComplex = csi.h2FastFadingComplex;

          allTXInfoVec.push_back (iInfo);


        }


      // get position and speed
      auto txPS = GetPositionNSpeedByVID (txid);
      auto rxPS = GetPositionNSpeedByVID (rxid);
      double dist = GetDistanceByTxRxVID (txid, rxid);



      // NS_LOG_INFO ("time: " << timeMS << " txid: " << txid << " rxid: " << rxid << " tx_pos.x " <<  txPS.pos.x << " dist: " << dist);



      /* save only if the TX-RX vehicles within area-of-interest */
      if (  (txPS.pos.x >= m_aoiX1 && txPS.pos.x <= m_aoiX2) &&
            (rxPS.pos.x >= (m_aoiX1+m_rxPad) && rxPS.pos.x <= (m_aoiX2-m_rxPad) )  &&
            (dist <= m_distCap) )
        {
          /* get the SINR and TX-RX distance */
          auto ffcoef = GetCommonFFCoef (chid, txid, rxid);
          double sinr  = GetSINRnDistForTXnRXnInterfererList (timeMS, chid, txid, rxid, allTXInfoVec, ffcoef.h1FastFadingComplex, ffcoef.h2FastFadingComplex);


          // NS_LOG_INFO ("time: " << timeMS << " txid: " << txid << " rxid: " << rxid << " sinr " <<  sinr << " dist: " << dist);


          /* creare object to save */
          LogInfoSINR linfo;
          linfo.chid = chid;
          linfo.txid = txid;
          linfo.rxid = rxid;
          linfo.arrivalSF = m_vehicleList.at (txid).m_PrevTXSF;
          linfo.txSF = timeMS;
          linfo.sinr = sinr;
          linfo.dist = dist;
          linfo.txPos = txPS.pos;
          linfo.txSpeed = txPS.speed;
          linfo.rxPos = rxPS.pos;
          linfo.rxSpeed = rxPS.speed;

          /* save this info to internal data structure */
          m_vehicleList.at (txid).m_logSINRInfo.push_back (linfo);

        }

    }


}

/* return SINR for a given channel */
double
LTEV2X_System::DoGetTXRXSINRInfoForTXVIDByChId (double timeMS, int txid, int rxid, int chid,  std::complex<double> h1TxComplex, std::complex<double> h2TxComplex)
{
  std::vector<int> txList = GetAllTXListbyChId (timeMS, chid);
  double sinr = VERYLOWSINRDB;

  if (!txList.empty ())
    {


      std::vector<InterfererInfo> allTXInfoVec;  // contain information about all tx
      // those are all tx list
      for (auto id: txList)
        {
          auto csi = GetChannelQuality (chid, id, rxid);
          double txPower = m_vehicleList.at (id).m_currentTxPower;
          double txPowerWatt = LTEV2X_Helper::DbmToWatt (txPower);
          double plLinear = std::pow(10, -csi.pathloss/10.0);
          double pRxSINRWatt = GetCombinedAntennaRxPowerWatt (txPowerWatt, abs (csi.h1FastFadingComplex), abs (csi.h2FastFadingComplex), plLinear);


          InterfererInfo iInfo;
          iInfo.txid = id;
          iInfo.plLinear = plLinear;
          iInfo.pRxSINRWatt = pRxSINRWatt;
          iInfo.h1FastFadingComplex = csi.h1FastFadingComplex;
          iInfo.h2FastFadingComplex = csi.h2FastFadingComplex;

          allTXInfoVec.push_back (iInfo);


        }

      /* get the SINR */
      sinr  = GetSINRnDistForTXnRXnInterfererList (timeMS, chid, txid, rxid, allTXInfoVec,  h1TxComplex, h2TxComplex);


    }

    return sinr;


}

/* save CBR and packet arrival rate for each vehicle
*  as Hongseng mentioned
*/
void
LTEV2X_System::SaveCBRandPcktArrivalRateforVID (double timeMS, int vid)
{
  // get position and speed
  auto vPS = GetPositionNSpeedByVID (vid);

  /* save only if the TX-RX vehicles within area-of-interest */
  if (vPS.pos.x >= m_aoiX1 && vPS.pos.x <= m_aoiX2)
    {
      /* creare object to save */
      LogInfoCBRnArrivalRate linfo;
      linfo.logSF = timeMS;
      linfo.cbr =  m_vehicleList.at (vid).m_currentCBR;
      linfo.vid = vid;
      linfo.arrivalRate = m_vehicleList.at (vid).m_PckArivSF - m_vehicleList.at (vid).m_PrevTXSF;
      linfo.txPower = m_vehicleList.at (vid).m_currentTxPower;

      /* save this info to internal data structure */
      m_vehicleList.at (vid).m_logCBRnArrivalRateInfo.push_back (linfo);



    }

}


/* save SINR for each TX packet */
void
LTEV2X_System::SaveTXRXSINRInfoForTXVID (double timeMS, int txid, double packetid)
{
  // std::vector<int> chidVec;
  int chid = GetChIDBySFforTXVID (timeMS, txid);

  /* a sanity check */
  if (chid < 0)
    return;

  // NS_LOG_INFO ("time: " << timeMS << " txid: " << txid << " packet TX!" << " ch [1]: " << chid);

  /* get for both channel and all RX (since we need two subchannel for each packet transmission) */

  for (int rxid = 0; rxid < m_nVehicle; rxid++)
    {
       if (txid != rxid)
        {
          // DoSaveTXRXSINRInfoForTXVIDByChId (timeMS, txid, rxid, chid);
          // DoSaveTXRXSINRInfoForTXVIDByChId (timeMS, txid, rxid, chid+1);

          /* get common fading coef */

          auto ffcoef = GetCommonFFCoef (chid, txid, rxid);

          // NS_LOG_INFO ("time: " << timeMS << " txid: " << txid << " rxid:" << rxid << " ffcof" <<  ffcoef.h1FastFadingComplex);


          double sinr1 = DoGetTXRXSINRInfoForTXVIDByChId (timeMS, txid, rxid, chid, ffcoef.h1FastFadingComplex, ffcoef.h2FastFadingComplex);
          double sinr2 = DoGetTXRXSINRInfoForTXVIDByChId (timeMS, txid, rxid, chid+1, ffcoef.h1FastFadingComplex, ffcoef.h2FastFadingComplex);
          double sinr = 2/((1/sinr1) + (1/sinr2)); // this equation comes from Javi [from two subchannels]

          // NS_LOG_INFO ("time:" << timeMS << " txid:" << txid << " rxid:" << rxid <<
          //             " dist: " << GetDistanceByTxRxVID (txid, rxid) <<
          //             " sinr1:" <<  sinr1 << " sinr2:" << sinr2 << " sinr:" << sinr);


          // key: (rxid,packetid), val: (sinr, txtime)

          rxPacketKeyPair kp = std::make_pair (rxid, packetid); // key
          SINRTXTimeInfo stti = {.sinr=sinr, .txtime=timeMS}; // val

          m_vehicleList.at (txid).m_rxPckSINRTxTimeMultiMap.insert (std::pair<rxPacketKeyPair, SINRTXTimeInfo> (kp, stti) );

          uint keyCount = ((m_isHARQEnabled) ? 2 : 1); // two transmissions for each packet if HARQ enabled

          // NS_LOG_INFO ("time: " << timeMS << " txid: " << txid << " rxid:" << rxid << " key count" <<  m_vehicleList.at (txid).m_rxPckSINRTxTimeMultiMap.count (kp));

          if (m_isHARQEnabled)
            {
              if (m_vehicleList.at (txid).m_rxPckSINRTxTimeMultiMap.count (kp) == keyCount)
                {
                  // handle two SINR
                  // save 2 sinr and 2 tx time
                  double sinrarr[2] = {VERYLOWSINRDB, VERYLOWSINRDB};
                  double tttimearr[2] = {-1, -1}; 
                  std::pair<rxPckSINRTxTimeMMIterator, rxPckSINRTxTimeMMIterator> it = m_vehicleList.at (txid).m_rxPckSINRTxTimeMultiMap.equal_range (kp);

                  /* a sanity check */
                  if (std::distance(it.first, it.second) != keyCount)
                    {
                      NS_LOG_ERROR ("Key error when HARQ enabled. Exiting...");
                      exit (1);
                    }
                  // Iterate over the two values
                  int idx = 0;
                  for (rxPckSINRTxTimeMMIterator mmi = it.first; mmi != it.second;  mmi++)
                    {
                      sinrarr[idx] = mmi->second.sinr;
                      tttimearr[idx] = mmi->second.txtime;
                      idx++;
                    }

                  double dist = GetDistanceByTxRxVID (txid, rxid);
                  // get the per and corresponding tx time
                  auto result = GetSINR2PERwithHARQ (sinrarr[0], sinrarr[1], tttimearr[0], tttimearr[1], dist);

                  // NS_LOG_INFO ("time: " << timeMS << " txid: " << txid << " rxid:" << rxid << " per:" <<  result.per << "txtime:" << result.txtime);

                  SavePERForLogging (packetid, result.txtime, txid, rxid, dist, result.per);

                  // also save the succssfull reception info
                  if (result.per==0)
                    StoreSuccReceptionForRX (txid, rxid, result.txtime);

                }

            }
          else
            {
              // get PER with single SINR
              if (m_vehicleList.at (txid).m_rxPckSINRTxTimeMultiMap.count (kp) == keyCount)
               {
                 double dist = GetDistanceByTxRxVID (txid, rxid);
                 int per = GetSINR2PERnoHARQ (sinr, dist);

                //  NS_LOG_INFO ("txtime: " << timeMS << " txid: " << txid << " rxid:" << rxid << " per:" << per << " dist:" <<  GetDistanceByTxRxVID (txid, rxid));

                 SavePERForLogging (packetid, timeMS, txid, rxid, dist, per);
                 if (per == 0)
                    StoreSuccReceptionForRX (txid, rxid, timeMS);
               }

            }




        }

    }


}

/* returns the selected channel and SF (absolute SF number) */
auto
LTEV2X_System::GetCandidateChannelSF (double timeMS, int vid)
{
  /* used to return the data */
  struct result
  {
    // subchannel and SF (alwasys sends the first subchannel ID)
    int reserveChannelIDn;  // primary
    double reserveSFTimeMS;
    int reserveChannelIDnHARQ; // HARQ
    double reserveSFTimeMSHARQ;
  };

  /* this is the selection size (to take average of 1000 ms sension duration) */
  const int sensingPeriodInterval = 100;

  // number of SF used for averaging (1000/100 = 10)
  const int nSensingSamples = ((int) m_sensingDuration)/sensingPeriodInterval;

  // create an array to store the values (average of 10 values in 100 ms interval)
  auto rssiArray = new double[m_nSubChannels][sensingPeriodInterval];

  double beginTime =  timeMS - m_sensingDuration;

  for (int chid = 0; chid < m_nSubChannels; chid++)
    {
      for (int j = 0; j < sensingPeriodInterval; j++)
        {
          // NS_LOG_INFO ("Channel: " << chid << " Window: " << j);
          double rssiSum = 0.0;  // this will be used to take the average

          bool isIgnoreSF = false;

          for (int i = j; i < (int) m_sensingDuration; i+= sensingPeriodInterval)
            {
              double absoluteTime = beginTime + i;
              // NS_LOG_INFO ("SF List: " << i << " Absolute time: " << absoluteTime);

              /* ignore the SF from selection if the vehicle is in TX mode */
              TXStateWithPckIDInfo tsinfo = IsVehicleInTXMode (absoluteTime, vid);
              bool isTXMode = tsinfo.isTX;

              // NS_LOG_INFO ("time: " << timeMS << " vid: " << vid << " isTX:" << tsinfo.isTX << " packetid:" << tsinfo.packetID);

              // if (IsVehicleInTXMode (absoluteTime, vid))
              if (isTXMode)
                isIgnoreSF = true;

              rssiSum += m_vehicleList.at (vid).m_localSFCSI.at (chid)[timeMS].m_rssi;


            }

          if (isIgnoreSF)
            rssiArray[chid][j] = LARGEVALUE;  // ignore that SF
          else
            rssiArray[chid][j] = rssiSum/(double) nSensingSamples;  // take the average and save it

          // NS_LOG_INFO ("RSSI AVG:" << rssiSum/(double) nSensingSamples);

        }
    }


  int windowSizeT2 = GetWindowSizeT2(timeMS, vid);


  /* select top 20% resource from the selection window */


  /* the following code is similar from 2018 V2X simulator */
  typedef std::tuple<double, int, int> P;  // <rssi, chidN (first channel between 2), sfindx>
  // overload the operator --> since we need least RSSI
  struct Order
    {
      bool operator()(P const& a, P const& b) const
      {
        return std::get<0> (a) > std::get<0> (b);
      }
    };
  std::priority_queue< P, std::vector<P>, Order > q;

  /* push the values into the priority queue */

   for (int sfidx = m_selectionWindowT1; sfidx < windowSizeT2; sfidx++)
    {
      /* do pairwise search */
      for (int chid = 0; chid < m_nSubChannels-1; chid++)
        {
          double rssiVal = (rssiArray[chid][sfidx] + rssiArray[chid+1][sfidx]) / 2.0; // take pairwise average

          // get a small variation in range 0~0.001 as Takayuki mentioned
          double randval = (LTEV2X_Helper::GetRandomRealNumberInRange (0, 1))/1000;
          rssiVal = rssiVal + randval;

          if (rssiArray[chid][sfidx] != LARGEVALUE || rssiArray[chid+1][sfidx] != LARGEVALUE)
            q.push (std::make_tuple (rssiVal, chid, sfidx));  // push into the priority queue

        }
    }



  /* now get top 20% resources */

  // 20% of selection window
  int nResources = ((windowSizeT2 - m_selectionWindowT1) * m_nSubChannels) * (m_topResourcePercentage/100);


  typedef std::pair<int, int> resPair;  // channel, selection widdow idx pair
  std::vector<resPair> resVector;  // create vector to save the SF/Channel info (all are candidate resources)
  // resVector.resize (nResources);

  for (int i = 0; i < nResources; i++) {
    int chidN = std::get<1> (q.top());  // extract the first channel id
    int sfindx = std::get<2> (q.top());  // extract subframe id

    // save it
    resVector.push_back (std::make_pair (chidN, sfindx));
    q.pop();
  }

  // get a random index to obtain from the set of candidate resources
  int randIndx = LTEV2X_Helper::GetRandomNumberInRange (0, nResources-1);

  resPair rp = resVector.at (randIndx);

  // NS_LOG_INFO ("RandIndx: " << randIndx << " Selected SF: " << rp.second << " Ch_first: " << rp.first);

  int selectedChFirst = rp.first;
  double selectedSF = rp.second + timeMS;  // make it absolute time

  /* select HARQ candidates if enabled */

  // HARQ subchannel and SF (initialized to some negative value)
  int selectedChFirstHARQ = -1;
  double selectedSFHARQ = -1;

  if (m_isHARQEnabled)
    {
      int primarySFIdx = rp.second;
      std::vector<resPair> resVectorHARQ;

      // upper and lower bound of (relative) HARQ SF

      int ub = std::min(windowSizeT2-1, primarySFIdx + m_HARQDeltaSF);
      int lb = primarySFIdx - m_HARQDeltaSF < m_selectionWindowT1 ? m_selectionWindowT1 : primarySFIdx - m_HARQDeltaSF;

      // NS_LOG_INFO ("vid: " << vid << " in time: " << timeMS << " selected SF:" << primarySFIdx << " ub: " << ub << " lb: " << lb);


      for (auto it:resVector)
        {
          /* select resources within +- 15 ms window */
          if ( primarySFIdx != it.second && (it.second >= lb && it.second <= ub) )
            resVectorHARQ.push_back (std::make_pair (it.first, it.second));  // save it
        }

      // now pick a random one
      if (!resVectorHARQ.empty())
        {
          int randIndxHARQ = LTEV2X_Helper::GetRandomNumberInRange (0, (int) resVectorHARQ.size ()-1);
          resPair rpHARQ = resVectorHARQ.at (randIndxHARQ);
          selectedChFirstHARQ = rpHARQ.first;
          selectedSFHARQ = rpHARQ.second + timeMS;  // make it absolute time
        }
    }


  delete[] rssiArray;  /* free memory */

  /* return the selection */
  return result {selectedChFirst, selectedSF, selectedChFirstHARQ, selectedSFHARQ};

}


void
LTEV2X_System::DoSensingAndReservation (double timeMS, int vid)
{
  // NS_LOG_INFO (" vid: " << vid << " time: " << timeMS << " -> I will do sensing and resource selection!");

  auto result = GetCandidateChannelSF (timeMS, vid);
  double packetid = m_vehicleList.at (vid).m_PckArivSF;

  // sets the vehicle in TX for for that SF
  SetVehicleInTXMode (result.reserveSFTimeMS, vid, packetid);

  // update channel usage map
  SetVehicleChannelUsageMap (result.reserveSFTimeMS, vid, result.reserveChannelIDn);

  /* add vehicle to the consecutive channels */
  AddVehicleToChannel (result.reserveSFTimeMS, vid, result.reserveChannelIDn);
  AddVehicleToChannel (result.reserveSFTimeMS, vid, result.reserveChannelIDn+1);

  /* reserve resource if HARQ is also enabled */
  if (m_isHARQEnabled)
    {
      /* reserve resource only if we find something for HARQ */

      if (result.reserveSFTimeMSHARQ >=0)
        {
          // sets the vehicle in TX for for that SF
          SetVehicleInTXMode (result.reserveSFTimeMSHARQ, vid, packetid);

          // update channel usage map
          SetVehicleChannelUsageMap (result.reserveSFTimeMSHARQ, vid, result.reserveChannelIDnHARQ);

          /* add vehicle to the consecutive channels */
          AddVehicleToChannel (result.reserveSFTimeMSHARQ, vid, result.reserveChannelIDnHARQ);
          AddVehicleToChannel (result.reserveSFTimeMSHARQ, vid, result.reserveChannelIDnHARQ+1);

        }




    }

  // update in vehicle data structure
  m_vehicleList.at (vid).m_resReservationInfo.primaryChannelIDn = result.reserveChannelIDn;
  m_vehicleList.at (vid).m_resReservationInfo.primarySFTimeMS = result.reserveSFTimeMS;
  m_vehicleList.at (vid).m_resReservationInfo.primarySFTimeRelative = result.reserveSFTimeMS - timeMS;
  m_vehicleList.at (vid).m_resReservationInfo.harqChannelIDn = result.reserveChannelIDnHARQ;
  m_vehicleList.at (vid).m_resReservationInfo.harqSFTimeMS = result.reserveSFTimeMSHARQ;
  if (result.reserveSFTimeMSHARQ >=0)
    m_vehicleList.at (vid).m_resReservationInfo.harqSFTimeRelative = result.reserveSFTimeMSHARQ - timeMS;
  else
    m_vehicleList.at (vid).m_resReservationInfo.harqSFTimeRelative = result.reserveSFTimeMSHARQ;  // set to -1


  // NS_LOG_INFO ("vid: " << vid << " Selected Subchannels (primary): " << "(" << result.reserveChannelIDn << "," << result.reserveChannelIDn+1 << ") -> Sel SF: " << result.reserveSFTimeMS);
  // NS_LOG_INFO ("vid: " << vid << " Selected Subchannels (HARQ): " << "(" << result.reserveChannelIDnHARQ << "," << result.reserveChannelIDnHARQ+1 << ") -> Sel SF: " << result.reserveSFTimeMSHARQ);


  // /* also save this info as Hongseng mentioned */
  // SaveResourceSelectionInfo (timeMS, vid);

}

/* no sensing -> only reservation */
/* timeMS is the current packet arrival time */
void
LTEV2X_System::DoReservationOnly (double timeMS, int vid)
{

  // NS_LOG_INFO (" vid: " << vid << " time: " << timeMS << " -> Do only reservation!");


  /* for readability assign to some variables */
  double nextReserveSFPrimaryMS = m_vehicleList.at (vid).m_resReservationInfo.primarySFTimeRelative + timeMS;
  double nextReserveSFHARQMS = m_vehicleList.at (vid).m_resReservationInfo.harqSFTimeRelative + timeMS;
  int primaryChannelIDn =  m_vehicleList.at (vid).m_resReservationInfo.primaryChannelIDn;
  int harqChannelIDn =  m_vehicleList.at (vid).m_resReservationInfo.harqChannelIDn;

  double packetid = m_vehicleList.at (vid).m_PckArivSF;



  /* set with this resources */
  // sets the vehicle in TX for for that SF
  SetVehicleInTXMode (nextReserveSFPrimaryMS, vid, packetid);

  // update channel usage map
  SetVehicleChannelUsageMap (nextReserveSFPrimaryMS, vid, primaryChannelIDn);


  /* add vehicle to the consecutive channels */
  AddVehicleToChannel (nextReserveSFPrimaryMS, vid, primaryChannelIDn);
  AddVehicleToChannel (nextReserveSFPrimaryMS, vid, primaryChannelIDn+1);

  /* reserve resource if HARQ is also enabled */
  if (m_isHARQEnabled)
    {
      // sets the vehicle in TX for for that SF
      SetVehicleInTXMode (nextReserveSFHARQMS, vid, packetid);

      // update channel usage map
      SetVehicleChannelUsageMap (nextReserveSFHARQMS, vid, harqChannelIDn);

      /* add vehicle to the consecutive channels */
      AddVehicleToChannel (nextReserveSFHARQMS, vid, harqChannelIDn);
      AddVehicleToChannel (nextReserveSFHARQMS, vid, harqChannelIDn+1);

    }

  //  NS_LOG_INFO (" vid: " << vid << " packet arrival: " << timeMS <<
  //       " reselect counter: " << m_vehicleList.at (vid).m_resourceReselectionCounter <<
  //       " SF --> prev (primary): " << m_vehicleList.at (vid).m_resReservationInfo.primarySFTimeMS <<
  //       " next (primary): " << nextReserveSFPrimaryMS <<
  //       " prev (HARQ): " << m_vehicleList.at (vid).m_resReservationInfo.harqSFTimeMS <<
  //       " next (HARQ): " << nextReserveSFHARQMS);

  /* update internal data structure */
  m_vehicleList.at (vid).m_resReservationInfo.primarySFTimeMS = nextReserveSFPrimaryMS;
  m_vehicleList.at (vid).m_resReservationInfo.harqSFTimeMS =  nextReserveSFHARQMS;




}


/* perform sensing and resource allocation */
void
LTEV2X_System::ReserveResourceForVID (double timeMS, int vid)
{

  /* do sensing and reservation */


  if (m_vehicleList.at (vid).m_resourceReselectionCounter < 0)  /* this is for first time */
    {
      // select a new reservation counter
      m_vehicleList.at (vid).m_resourceReselectionCounter = LTEV2X_Helper::GetRandomNumberInRange (m_resourceReselectionCounterMin, m_resourceReselectionCounterMax);

      // /* for HARQ two packets is considered as one */
      // if (m_isHARQEnabled)
      //   m_vehicleList.at (vid).m_resourceReselectionCounter = 2 * m_vehicleList.at (vid).m_resourceReselectionCounter;

      m_vehicleList.at (vid).m_C_resel = 10 * m_vehicleList.at (vid).m_resourceReselectionCounter;
      
      // /* for HARQ two packets is considered as one */
      // if (m_isHARQEnabled)
      //   m_vehicleList.at (vid).m_C_resel = 2 * m_vehicleList.at (vid).m_C_resel;

      
      // NS_LOG_INFO (" vid: " << vid << " current (packet arrival) time: " << timeMS <<
      //   " reselect counter: " << m_vehicleList.at (vid).m_resourceReselectionCounter);

      DoSensingAndReservation (timeMS, vid);  // perform sensing and reservation

    }
  else if (m_vehicleList.at (vid).m_resourceReselectionCounter > 0)
    {
      // continue reserving previous resource
      DoReservationOnly (timeMS, vid);
      m_vehicleList.at (vid).m_resourceReselectionCounter--;  // decrement counter

    }
  else  /* counter is 0 */
    {
      /* check if the vehicle wants to keep the current resource */
      bool isKeepCurrent = false;
      int randomVal = LTEV2X_Helper::GetRandomNumberInRange (1, 100);

      if (randomVal <= (int)  (m_pKeep * 100))
        isKeepCurrent = true;

      if (isKeepCurrent)
        {
          // NS_LOG_INFO (" vid: " << vid << " time: " << timeMS << " Relect Counter 0 -> Keeping current Resource..");

          /* for HARQ two packets is considered as one */
          if (m_isHARQEnabled)
            m_vehicleList.at (vid).m_C_resel = m_vehicleList.at (vid).m_C_resel - 2;
          else
            m_vehicleList.at (vid).m_C_resel--;


          /* can't use the same resource -> do reservation */
          if (m_vehicleList.at (vid).m_C_resel <= 0)
            {
              // select a new reservation counter
              m_vehicleList.at (vid).m_resourceReselectionCounter = LTEV2X_Helper::GetRandomNumberInRange (m_resourceReselectionCounterMin, m_resourceReselectionCounterMax);
              
              // /* for HARQ two packets is considered as one */
              // if (m_isHARQEnabled)
              //   m_vehicleList.at (vid).m_resourceReselectionCounter = 2 * m_vehicleList.at (vid).m_resourceReselectionCounter;

              // set new C_resel
              m_vehicleList.at (vid).m_C_resel = 10 * m_vehicleList.at (vid).m_resourceReselectionCounter;
              /* for HARQ two packets is considered as one */
              // if (m_isHARQEnabled)
              //   m_vehicleList.at (vid).m_C_resel = 2 * m_vehicleList.at (vid).m_C_resel;

              
              DoSensingAndReservation (timeMS, vid);  // perform sensing and reservation

            }
          else
            {
              // select a new reservation counter
              m_vehicleList.at (vid).m_resourceReselectionCounter = LTEV2X_Helper::GetRandomNumberInRange (m_resourceReselectionCounterMin, m_resourceReselectionCounterMax);
              
              // /* for HARQ two packets is considered as one */
              // if (m_isHARQEnabled)
              //   m_vehicleList.at (vid).m_resourceReselectionCounter = 2 * m_vehicleList.at (vid).m_resourceReselectionCounter;

              
              DoReservationOnly (timeMS, vid);  // only reserve the resource

            }



          
        }
      else
        {
          // NS_LOG_INFO (" vid: " << vid << " time: " << timeMS << " Relect Counter 0 -> Want to do sensing..");

          // select a new reservation counter
          m_vehicleList.at (vid).m_resourceReselectionCounter = LTEV2X_Helper::GetRandomNumberInRange (m_resourceReselectionCounterMin, m_resourceReselectionCounterMax);
          
          // /* for HARQ two packets is considered as one */
          // if (m_isHARQEnabled)
          //   m_vehicleList.at (vid).m_resourceReselectionCounter = 2 * m_vehicleList.at (vid).m_resourceReselectionCounter;

          
          DoSensingAndReservation (timeMS, vid);  // perform sensing and reservation
        }
    }




  /* update next packet arrival time */
  UpdateNextPacketArrivalTimeForVid (vid);

  /* update window size T2 */
  if (!m_isFixedT2Window)
    UpdateCBRBasedWindowT2 (vid);






}

/* ... */



}
