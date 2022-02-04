/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef LTE_V2X_H
#define LTE_V2X_H

#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/core-module.h>
#include <ns3/mobility-model.h>
#include "ns3/constant-velocity-mobility-model.h"


#include <stdlib.h> 
#include <unistd.h>
#include <vector>
#include <unordered_map> 
#include <complex>
#include <map>
#include <utility>




namespace ns3 {


/* NOTE: chnage this defines if anything change in mapping */

#define _MIN_SINR_HARQ 1
#define _MAX_SINR_HARQ 2.5
#define _INC_HARQ 0.1  // increment in SINR table
#define __MAXKEY_HARQ_INDX 255 // [from 1 to 2.5 with 0.1 increment]

#define _N_SINR_HARQ_LIST_ENTRY 16  // size of that __SINR_LIST table

const double __SINR_LISTS[] = { 1,
                            1.1,
                            1.2,
                            1.3,
                            1.4,
                            1.5,
                            1.6,
                            1.7,
                            1.8,
                            1.9,
                            2,
                            2.1,
                            2.2,
                            2.3,
                            2.4,
                            2.5 };
/* END of SINR-PER Mapping */


  
#define degreesToRadians(angleDegrees) ((angleDegrees) * M_PI / 180.0)
#define radiansToDegrees(angleRadians) ((angleRadians) * 180.0 / M_PI)


#define LARGEVALUE 5000000  // a large value for rssi/rsrp
const double VERYLOWSINRDB = -1000.00;  // a very low SINR (dB)

// mhasan: implementation of LTE-V2X

/* this class contained all channel information */
class ChannelInfo
{
  public:
    static constexpr double m_noiseSd = -174;  // dbm/HZ
    static constexpr int m_subChannelBwMHz = 2; // bandwidth of each subchannel
    static constexpr double m_carrierFrequency = 5915000000; // in Hz (The carrier frequency is 5.915 GHz)
    static constexpr double m_vehicleAntennaHeight = 1.5; // antenna height (meter)
    
    /* Total 10 PRB for 2 MHz -> 2 PRBs (fixed) for PSCCH and 8 PRBs for PSSCH */
    static constexpr double m_psschBandwidth = 180*1000 * 8; // in Hz (8 Resoruce block, each 180 KHz)
    static constexpr double m_noiseFigure = 9;  // dB
    static constexpr double m_nakagamiGamma = 1;  // Nakagami RV param
    static double GetThermalNoise (); // returns thermal noise
    static double GetPSSCHThermalNoise (); // returns pssch thermal noise 
    static double GetPathLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b, double dist, double carrierFrequency); // pathloss in dB
    static double GetFadingCoefByDist (double dist);  // returns fading co-efficients
}; // end of ChannelInfo class


// the struct contains low-level channel information
/* use to return from function */
typedef struct CSIComponents {

  // for each antenna
  double h1Rssi;
  double h2Rssi;
  double h1Rsrp;
  double h2Rsrp;

  // combined
  double rssi;
  double rsrp;

  // pathloss, shadowing, and fading
  double pathloss;
  // double shadowing; /* shadowing is disabled for now */
  double h1FastFadingMagDb;
  double h2FastFadingMagDb;

  std::complex<double> h1FastFadingComplex;
  std::complex<double> h2FastFadingComplex;

} 
CSIComponents;

/* save channel information (rssi, rsrp) -> local for each vehicle */
typedef struct ChStateInfo
{
  double m_rssi;  // in dBm
  double m_rsrp; // in dBm
  double m_sinr;  // in dB
  double m_dist; // in meter
  int m_txid; // id of the TX (for which TX is attched for the RX)
}
ChStateInfo; 

/* information about interfereing vehicle using a given channel in a given subframe */
typedef struct InterfererInfo
{
  int txid;
  double pRxSINRWatt;
  double txPowerWatt;
  double plLinear;
  std::complex<double> h1FastFadingComplex;
  std::complex<double> h2FastFadingComplex;
}
InterfererInfo;

/* information about each packet TX */
typedef struct LogInfoSINR
{
  double arrivalSF; // packet arrival SF
  double txSF;  // SF used for packet transmission
  int chid; // channel id
  int txid; // txid
  int rxid; // rxid
  double dist; // distance between tx-rx
  double sinr; // sinr
  Vector3D txPos; // position of tx
  Vector3D rxPos; // position of rx
  Vector3D txSpeed; // speed of tx
  Vector3D rxSpeed; // speed of rx
}
LogInfoSINR;

/* information about each packet TX */
typedef struct LogInfoPER
{
  double arrivalSF; // packet arrival SF
  double txSF;  // SF used for packet transmission
  int txid; // txid
  int rxid; // rxid
  double dist; // distance between tx-rx
  int per; // per (success/fail)
  Vector3D txPos; // position of tx
  Vector3D rxPos; // position of rx
  Vector3D txSpeed; // speed of tx
  Vector3D rxSpeed; // speed of rx
}
LogInfoPER;


/* information about CBR and arrival rate */
typedef struct LogInfoCBRnArrivalRate
{
  double logSF; // time when value is logged
  int vid; // vehicle id
  double cbr; // cbr
  double arrivalRate; // packet arrival rate
  double txPower; // save current TX power
}
LogInfoCBRnArrivalRate;


/* store resource reservation information */
typedef struct ResourceReservationInfo 
  {
    // subchannel and SF 
    int primaryChannelIDn;  // primary
    double primarySFTimeMS;
    double primarySFTimeRelative;  // relative index in the window
    int harqChannelIDn; // HARQ
    double harqSFTimeMS;
    double harqSFTimeRelative;  // related index 
  }
  ResourceReservationInfo;

/* store which packet is transmitted in for a given SF (packet id is the arrival time) */
typedef struct TXStateWithPckIDInfo 
  {
    double packetID;
    bool isTX; 
  }
  TXStateWithPckIDInfo;

/* use to store SINR and TX time */
typedef struct SINRTXTimeInfo 
  {
    double sinr;
    double txtime; 
  }
  SINRTXTimeInfo;

/* use to store RXID and packetid (key) */
typedef std::pair<int, double> rxPacketKeyPair;
// typedef struct rxPacketKeyPair 
//   {
//     int rxid;
//     double packetid;
//   }
//   rxPacketKeyPair;

// vehicle id list for each subframe (key: SF, val: vector of VID)
typedef std::unordered_map<double, std::vector<int>> SFUsageMap;

// store CSI for each subframe (SF is the key)
typedef std::unordered_map<double, ChStateInfo> SFCSIMap; 

// store which SF vehicle is in TX state (SF is the key)
typedef std::unordered_map<double, TXStateWithPckIDInfo> VehicleTXStateMap; 

// store which channel (first one) vehicle uses for transmission(SF is the key)
typedef std::unordered_map<double, int> VehicleChUsageMap; 

/* store successfull packet reception info (key is the SF when packet received, val TX id) */
typedef std::multimap<double, int> SuccReceptionMultiMap; 

/* iterator for this map */
typedef std::multimap<rxPacketKeyPair, SINRTXTimeInfo>::iterator rxPckSINRTxTimeMMIterator;

/* iterator for this map */
typedef std::multimap<double, int>::iterator succReceptionMMIterator;


/* this struct contains all vehicle info */
/* this is vehicle specific */
typedef struct LTEV2X_Vehicle
{

  int m_vehicleId; // id of the vehicle
  double m_curPckArivRateMS; // current packet arrival rate (may vary based on density [for congestion control])
  double m_PckArivSF; // moving counter to decide when packet arrives (upated after each arrival)
  double m_PrevTXSF; // moving counter to save previous packet arrival time
  double m_currentTxPower; // current transmit power (dBm) -> may vary if power control enabled
  std::vector<SFCSIMap> m_localSFCSI;  // save CSI for each subframe for all channel (SF is the key)
  VehicleTXStateMap m_isTXinSF; // use to know whether vehicle is in TX state for a particular SF 
  int m_resourceReselectionCounter = -1;  // counter for resource reselection
  int m_C_resel;  // max number of times vehicle can keep the same resource 
  ResourceReservationInfo m_resReservationInfo;  // moving data to save reserverd resource information
  double m_smoothVehicleDensity;  // smooth vehicle density -> Eq. (6) in SAE J3161 
  double m_currentCBR;  // moving variable to update cbr every time rate control is checked
  double m_vehDenCheckSF; // moving counter to check for vehicle density
  bool m_txDecisionMaxITT = false; // flag for rate control -> see specs
  bool m_txDecisionDynamics = false; // flag for tracking error -> see specs
  int m_selectionWindowT2; // window size T2 (for CBR based)
  VehicleChUsageMap m_chUsageMap; // use to save which channel (first id) vehicle is used for packet TX
  std::vector<LogInfoSINR> m_logSINRInfo;  // save SINR for all other vehicle for a given TX
  std::vector<LogInfoPER> m_logPERInfo;  // save PER for all other vehicle for a given TX
  std::vector<LogInfoCBRnArrivalRate> m_logCBRnArrivalRateInfo;  // save CBR and packet arrival rate a given vehicle
  std::vector<double> m_logResourceSelectionSFInfo; // save which SF vehicle do resource (re)selection
  /* this is to contain packet information and sinr for each transmission 
  * we need this to distingush HARQ packets/transmissions 
  * Save at the TX side */
  std::multimap<rxPacketKeyPair, SINRTXTimeInfo> m_rxPckSINRTxTimeMultiMap;   // key: (rxid,packetid), val: (sinr, txtime)
  SuccReceptionMultiMap m_succPckRecepMMap; // receiver side -> save at which SF (and which TX) packet is successfull

  Vector3D m_lastTxPosition;  // save the position of last TX (for tracking error)

}
VehicleInfo; // end of LTEV2X_Vehicle structure

/* Structure to contain simulation parameters */
typedef struct SimParam 
{
  double m_simDuration; // simulation duration
  std::string m_expName;  // name of the experiment;
  std::string m_outLogFileName; // log file name to save the traces 
  bool m_isHARQEnabled;  // HARQ
  bool m_isPowerControlEnabled;  // power control
  int m_sysBwMHz = 20;  // 20 MHz default system BW
  int m_selectionWindowT1 = 4; // UE dependent resource selection window size (T1)
  double m_pKeep = 0.8;  // probablity that same resource will be kept
  bool m_isCongestControlEnabled = true; // congestion control: enabled default
  double m_defaultMsgArrivalMS = 100; // message arrival rate (default 10 Hz -- when no congestion)
  int m_MCS = 5;  // modulation and coding information
  int m_nSubChPSSCHTx = 2;  // number of selected subchannel for tansmission (should be contigious)
  bool m_isFixedT2Window; // is fixed window size is used (default: false)
  int m_selectionWindowFixedT2;  // used when fixed T2 window (ms)
  bool m_isSingleTX = false;  // if we want to have a single TX (for analysis) -> disabled default
  /* area of interest (cutoff x-coordinate from HWY simulation scenario) */
  double m_aoiX1;  
  double m_aoiX2; 
  double m_rxPad; 
  double m_logStartTime;   // when to start logging

} 
SimParam; // end of SimParam structure


/* main class with all simulation information */
class LTEV2X_System
{
  public:
    void SimulateSubframe ();
    void SimulateSubframeSingleTX (); // simulate with only single TX
    void SetVehicleInfo (int nVehicle, NodeContainer &vehicleContainer);
    void InitSimParam (SimParam &simP);

    
    /* dummy function to see if we can write data traces after simulation */
    void LogSimulationResults (double simDuration);
   
  protected:
    double m_logStartTime; // when to start logging time in the file
    // double m_highwayLength = 5000.0; // in meter. we cosider a 5KM highway
    double m_carrierFrequency = 5915000000; // in Hz (The carrier frequency for LTE-V2X is 5.915 GHz)
    double m_simDuration; // simulation duration
    std::string m_expName;  // name of the experiment;
    std::string m_outLogFileName; // log file name to save the traces 
    int m_nVehicle; // number of vehicles
    NodeContainer *m_vehicleContainer = NULL;  // vehicle container
    std::vector<VehicleInfo> m_vehicleList;  // list of all vehicles
    bool m_isHARQEnabled;  // whether HARQ enabled
    bool m_isPowerControlEnabled; // whether power control enabled
    bool m_isCongestControlEnabled; // whether congestion control enabled
    bool m_isFixedT2Window; // is fixed window size is used
    int m_sysBwMHz; // system bandwidth (MHz)
    double m_topResourcePercentage = 20; // take top 20% resources from selection window
    int m_nSubChannels; // number of subchannels (depends on system BW)
    int m_selectionWindowT1; // window size (T1) for resource selection
    int m_selectionWindowFixedT2; // window size (T2) for resource selection (if give as an input)
    double m_sensingDuration = 1000; // sensing duration 1000 ms
    double m_txPowerDefault = 20; // default transmit power of the vehicle (dBm)
    // double m_pPSSCH; // PSSCH power (dBm)
    double m_pKeep;  // probablity that same resource will be kept
    double m_defaultMsgArrivalMS; // message arrival rate (when no congestion)
    int m_MCS; // modulation and coding info
    int m_nSubChPSSCHTx; // number of subchannel selected for tansmission
    double m_dataControlPowerFactor = 3.0;  // Tx power of PSCCH signal is 3 dB higher than one of PSSCH signal
    double m_vehicleAntennaHeight = 1.5; // Antenna height (in meter)
    int m_HARQDeltaSF = 15; // take +- 15 ms SF from the selected resource (if HARQ enabled)
    std::vector<SFUsageMap> m_SFReserveAllChannelList;  // vehicle id list for each subframe and all channels (key: SF, val: vector of VID)
    double m_vTxRateCntrlInt = 100; // how often we check neighbor densite (for rate control) -> ms

    double m_neighborVehicleSearchTh = 100; // search neighbors within distance to calculate density (meter)
    /* resource reservation counter range [5, 15] */
    int m_resourceReselectionCounterMin = 5;
    int m_resourceReselectionCounterMax = 15;

    double m_vDensityWeightFactor = 0.05; // lambda in Eq. 6 -> SAE J3161
    double m_vSUPRAGain = 0.5; // for power control in Eq. 9 -> SAE J2945
    double m_vRescheduleTh = 25; // ms --> see SAE J3161 spec (6.3.7.5)

    double m_thCBR = -94; // CBR threshold per subchannel (dBm)

    double m_PDB = 100; // packet delay budget (100 ms)

    double m_aoiX1;  // 1st cutoff x-coordinate from HWY simulation scenario
    double m_aoiX2;  // 2nd cutoff x-coordinate from HWY simulation scenario
    double m_distCap = 550; // distance cutoff
    double m_rxPad; // distance cutoff for RX from AoI

    double m_vPERSubInterval = 1000;  // window size to check successful reception [SAE J3161 Fig. 23]
    
    /* SINR to PER mapping table */
   
    // std::vector<std::pair<double, double>> m_sinr2perTableNOHARQ__25; // (sinr, per)
    // std::vector<std::pair<double, double>> m_sinr2perTableNOHARQ__75; // (sinr, per)
    // std::vector<std::pair<double, double>> m_sinr2perTableNOHARQ__200; // (sinr, per)

    // std::vector<std::pair<double, double>> m_sinr2perTableWITHHARQ__25; // (sinr, per)
    // std::vector<std::pair<double, double>> m_sinr2perTableWITHHARQ__75; // (sinr, per)
    // std::vector<std::pair<double, double>> m_sinr2perTableWITHHARQ__200; // (sinr, per)

    std::vector<std::pair<double, double>> m_sinr2perTableNOHARQ__AWGN; // (sinr, per)
    std::vector<std::pair<double, double>> m_sinr2perTableWITHHARQ__AWGN; // (sinr, per)


    /* tracking error related variables */
    double m_vTrackingErrMin = 0.2; // m
    double m_vTrackingErrMax = 0.5; // m
    double m_vErrSensitivity = 75;

    /* update CSI for for a given vehicle */
    void UpdateCSIbyVID (double timeMS, int vid);
    void DoUpdateCSI (double timeMS, int vid, int chid);
    
    double GetPathLossByTXnRXID (int txid, int rxid); // returns the pathloss (dB) by TX and RX id

    double GetDistanceByTxRxVID (int txid, int rxid);  // return the distance between two vehicles

    std::complex<double> GetFastFadingComplex (double dist);  // returns fast fading complex number

    double GetPSSCHPowerDbmFromFactor (double txPower); // return the PSSCH power (dBm)
    CSIComponents GetChannelQuality (int chid, int txid, int rxid); // return channel information
    double GetCombinedAntennaRxPowerWatt (double txPowerWatt, double h1FastFadingLinear, double h2FastFadingLinear, double pathlossLinear); // use for SINR calculation
    auto GetSINRFromInterfererList (double timeMS, int vid, std::vector<InterfererInfo> intfInfoVec);
    TXStateWithPckIDInfo IsVehicleInTXMode (double timeMS, int vid); // checks if the vehicle is transmitting for that SF
    void SetVehicleInTXMode (double timeMS, int vid, double packetid); // sets the vehicle in TX for for that SF
    /* returns interference received power using MRC */
    double GetIntfReceivedPowerWatt (std::complex<double> h1TxComplex,
                                    std::complex<double> h2TxComplex,
                                    double infTxPowerLinear,
                                    double infPlLinear,
                                    std::complex<double> h1InfComplex,
                                    std::complex<double> h2InfComplex);
    double GetAvgSINRPerRxAntennaDb (double sinrDb); // average SINR per RX antenna
    /* do sensing and resource allocation */
    void ReserveResourceForVID (double timeMS, int vid);  // sense and reserve the resource for vehicle
    auto GetCandidateChannelSF (double timeMS, int vid); // return the channel and SF to be reserverd for the VID 
    int GetWindowSizeT2 (double timeMS, int vid); // returns the selection window size T2
    void AddVehicleToChannel (double timeMS, int vid, int chid);  // add the vehicle to the channel for that SF
    void UpdateNextPacketArrivalTimeForVid (int vid); // update the next packet arrival time
    void DoSensingAndReservation (double timeMS, int vid); // perform sensing and reservation
    void DoReservationOnly (double timeMS, int vid); // perform only reservation
    int GetNeighborVehicleForVID (int vid, double distTh);  // returns the number of vehicles within the given threshold
    int GetNeighborCountBySuccReceptionForVID (double timeMS, int vid, double distTh); // get the neighbor count based on successful reception
    
    void UpdateSmoothVehicleDensity (double timeMS, int vid);  // update (smooth) vehicle density
    double GetMaxITT (double vd); // returns MAX_ITT (see Eq.7 in the SAE J3161). Input is the vehicle density
    double GetCBR (double timeMS, int vid); // returns the channel busy ratio [n-100, n-1]
    void UpdateCBRBasedWindowT2 (int vid); // update CBR depended window size
    void SaveTXRXSINRInfoForTXVID (double timeMS, int vid, double packetid); // save SINR for each TX packet
    void DoSaveTXRXSINRInfoForTXVIDByChId (double timeMS, int txid, int rxid, int chid); // save SINR for each TX packet (for a given ChId)
    double DoGetTXRXSINRInfoForTXVIDByChId (double timeMS, int txid, int rxid, int chid, std::complex<double> h1TxComplex, std::complex<double> h2TxComplex); // return SINR for each TX packet (for a given ChId)
    void SetVehicleChannelUsageMap (double timeMS, int vid, int chid); // set the vehicle channel usage
    int GetChIDBySFforTXVID (double timeMS, int vid);  // return the channel (first) usage for the TX vehicle
    std::vector<int> GetAllTXListbyChId (double timeMS, int chid); // returns list of interfere for a given TX vid
    /* returns SINR for a given TX, RX and TX vehicle list on that channel */
    double GetSINRnDistForTXnRXnInterfererList (double timeMS, int chid, int txid, int rxid, std::vector<InterfererInfo> allTXInfoVec, std::complex<double> h1TxComplex, std::complex<double> h2TxComplex);
    auto GetPositionNSpeedByVID (int vid); // returns position and spped by VID
    Vector3D GetPositionByVID (int vid); // returns position for VID
    void UpdateCBRForVID (double timeMS, int vid); // update CBR for VID
    void SaveCBRandPcktArrivalRateforVID (double timeMS, int vid); // save CBR and packet arrival rate for the VID

    void SaveResourceSelectionInfo (double timeMS, int vid); // save when vehicle do resource reselection

    /* some funtion used for logging the trace */
    void DoLogSINRResults (double simDuration);
    void DoLogPERResults (double simDuration);
    void DoLogCBRnArrivalRateResults ();
    void DoLogResourceSelectionEvents ();
    void UpdatePowerForVID (double timeMS, int vid); // do the power control
    double GetPowerMappingByCBR (double cbr); // returns the power mapping [Eq. (9) in SAE J2945]

    /* functions for In-Band-Emission */
    auto GetIBEByOccupiedChIDn (int occupiedChidN, int primaryChid, int txid, int rxid);  // returns the ibe (power*gain) in dBm 
    std::vector<double> CalcIBE (int nRB, int lcrb, int rbStart, double pRB); 
    double CalculateIBEIntfWatt (double timeMS, int primaryChid, int rxid);
    std::vector<int> GetTXListByChidN (double timeMS, int chidN); // returns the TX ids use subchannel n,n+1
    double CalculateIBEIntfForSINRWatt (double timeMS, int primaryChid, int rxid); // in-band emmission for SINR calculation (watt)
    double CalculateIBEIntfForSINRWatt (double timeMS, int primaryChid, int rxid,  std::complex<double> h1TxComplex, std::complex<double> h2TxComplex); // for ibe sinr

    /* if we don't want to use sumo */
    
    // void CheckBound(Ptr<ConstantVelocityMobilityModel> mob);
    // void CheckRoadBoundForVID (int vid);

    /* functions for SINR to PER mapping */
    int GetSINR2PERnoHARQ (double sinr, double dist); // retrun PER for a given SINR
    
    auto GetSINR2PERwithHARQ (double sinr1, double sinr2, double tt1, double tt2, double dist); // return PER and txtime for a given SINR and TX time [With HARQ] 
    void SavePERForLogging (double arrivalTime, double txTime, int txid, int rxid, double dist, int per); // save to vector (only if in aoi)
    void StoreSuccReceptionForRX (int txid, int rxid, double txtime); // store which SF and which TX packet is successful

    /* SINR to PER conversion */
    double GetInterpolatePERnoHARQ(double sinr, double dist); // for NO HARQ 
    double GetInterpolatePERwithHARQ(double sinr, double dist); // for NO HARQ 
    double DoGetInterpolatePER(double sinr, std::vector<std::pair<double, double>> &sinr2perTable); // actual interporlation logic 
    void PopulateSINR2PERTable ();
    void DoPopulatePERWithoutHARQ ();
    void DoPopulatePERWithHARQ ();
    
    int GetHARQKeyFromTwoSINR (double sinr1, double sinr2);  // return HARQ key

    /* for tracking error */

    double GetTrackingErrorForVID (double timeMS, int vid); // return the tracking error e(k) for vid
    void SaveLastTXPostionForVID (int vid);
    
    double GetTransmissionProbability (double ek); // returns transmission probability p(k) from eq (5) 

    /* for common fading -> FA19 */
    auto GetCommonFFCoef (int chid, int txid, int rxid);

}; // end of LTEV2X_System class


}

#endif /* LTE_V2X_H */
