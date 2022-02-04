/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


#include <stdlib.h>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"

#include <vector>
#include <random>
#include <time.h>
#include <string>
#include <regex>

#include "ns3/lte-v2x.h"
#include "ns3/lte-v2x-helper.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("lte-v2x-sim-main");

void
do_run_simulation(int argc, char *argv[])
{
  // std::string traceFile = "v2x_traces/mobility_trace/sumo_ns3_tcl/v2xsim_ns3_v327_l3_100kmh_shifted.tcl";
  std::string traceFile = "v2x_traces/mobility_trace/sumo_ns3_tcl/v2xsim_ns3_v2386_l3_10kmh_shifted.tcl";



  double simDuration = 3000;  // msec
  // int numVehicle = 25;
  int numVehicle = 2200;
  double pKeep = 0.8;  // pkeep (default=0.8)
  bool isSingleTX = false;  // if we want to have a single TX
  int sysBW = 20;  // system BW (20 MHz default)
  bool isHARQEnabled = false; // HARQ [default true -> always-on HARQ]
  bool isCongestControlEnabled = true; // CC [rate control]
  bool isPowerControlEnabled = false; // whether power control enabled [default false -> SAEJ3161]
  int selectionWindowFixedT2 = 20; // default window size (if fixed)
  bool isFixedT2Window = false; // CBR based or fixed window size
  double logStartTime = 0; // when to start logging

  // area of interest
  double aoiX1 = 2000, aoiX2 = 3000;
  double distPad = 200;
  double rxPad = (aoiX2 - aoiX1)/4.0;  // a pad to use which RX we consider within AoI [to avoid edge effect]

  // double aoiX1 = 2000, aoiX2 = 2500;
  // double distPad = 150;



  std::string expName = "V2X_DEFAULT_EXP_NAME";
  std::string outLogFileName = "v2x_traces/v2x_default_log.txt";


  CommandLine cmd;
  cmd.AddValue("numVehicle", "Number of vehichles", numVehicle);
  cmd.AddValue("traceFile", "SUMO tracefile", traceFile);
  cmd.AddValue("simDuration", "Simulation duration", simDuration);
  cmd.AddValue("pKeep", "pKeep", pKeep);
  cmd.AddValue("sysBW", "System bandwidth", sysBW);
  cmd.AddValue("isHARQEnabled", "HARQ info", isHARQEnabled);
  cmd.AddValue("isPowerControlEnabled", "Power control", isPowerControlEnabled);
  cmd.AddValue("isCongestControlEnabled", "CC info", isCongestControlEnabled);
  cmd.AddValue("isSingleTX", "Is single TX", isSingleTX);
  cmd.AddValue("aoiX1", "AOI X1", aoiX1);
  cmd.AddValue("aoiX2", "AOI X2", aoiX2);
  cmd.AddValue("distPad", "Dist padding from AoI", distPad);
  cmd.AddValue("rxPad", "RX Dist padding from AoI", rxPad);
  cmd.AddValue("outLogFileName", "Output log filename", outLogFileName);
  cmd.AddValue("expName", "Name of the experiment", expName);
  cmd.AddValue("isFixedT2Window", "Fixed or CBR window", isFixedT2Window);
  cmd.AddValue("selectionWindowFixedT2", "Window size if fixed", selectionWindowFixedT2);
  cmd.AddValue("logStartTime", "Logging start time", logStartTime);
  cmd.Parse (argc, argv);

  if (!LTEV2X_Helper::IsStringEndsWith (outLogFileName, ".txt"))
    {
      NS_LOG_UNCOND ("Output log file extension must be .txt! Aborting...");
      exit (EXIT_FAILURE);
    }

  /* with sumo trace */
  NodeContainer vehicleContainer;
  vehicleContainer.Create (numVehicle);



  // ns2 mobility helper
  Ns2MobilityHelper ns2 = Ns2MobilityHelper (traceFile);
  ns2.Install ();

  /* only consider the nodes that are within AoI + some pad */
  int vCountAoI = 0;
  NodeContainer vehicleContainerAoI;

  for (size_t i=0; i<vehicleContainer.GetN (); i++)
    {
      Ptr<Node> node = vehicleContainer.Get (i);

      Ptr<MobilityModel> mobModel = node->GetObject<MobilityModel> ();

      Vector3D pos = mobModel->GetPosition ();

      if ( pos.x >= aoiX1-distPad && pos.x <= aoiX2+distPad)
        {
          vehicleContainerAoI.Add (node);
          vCountAoI++;
        }
    }


  /* configure simulation parameters */
  SimParam simP;
  simP.m_simDuration = simDuration;
  simP.m_isHARQEnabled = isHARQEnabled;
  simP.m_isCongestControlEnabled = isCongestControlEnabled;
  simP.m_isPowerControlEnabled = isPowerControlEnabled;
  simP.m_isFixedT2Window = isFixedT2Window;
  simP.m_selectionWindowFixedT2 = selectionWindowFixedT2;
  simP.m_sysBwMHz = sysBW;  // system BW
  simP.m_expName = expName;
  simP.m_outLogFileName = outLogFileName;
  simP.m_isSingleTX = isSingleTX;
  simP.m_logStartTime = logStartTime;  // time to start logging
  simP.m_pKeep = pKeep;
  simP.m_aoiX1 = aoiX1;  // area of interest
  simP.m_aoiX2 = aoiX2;  // area of interest
  simP.m_rxPad = rxPad;  // padding for RX from AoI
  


  LTEV2X_System sys;
  LTEV2X_Helper::DisplaySimParams (simP);

  sys.InitSimParam (simP);
  // sys.SetVehicleInfo (numVehicle, vehicleContainer);
  sys.SetVehicleInfo (vCountAoI, vehicleContainerAoI);

  // exit(0);

  NS_LOG_INFO ("Initialization Done. Running Simulation...!!");

  if (simP.m_isSingleTX)
    sys.SimulateSubframeSingleTX ();
  else
    sys.SimulateSubframe ();


  Simulator::Stop (MilliSeconds (simDuration));
  Simulator::Run ();
  Simulator::Destroy ();

  NS_LOG_INFO ("Simulation finished!! Now logging...");
  sys.LogSimulationResults (simDuration);


  NS_LOG_INFO ("Script finished. Log file saved with prefix*: " << simP.m_outLogFileName);

}


int
main (int argc, char *argv[])
{

  /* enable logging */
  LogComponentEnable("lte-v2x-sim-main", LOG_LEVEL_ALL);

  LogComponentEnable("LTE-V2XModule", LOG_LEVEL_INFO);
  LogComponentEnable("LTE-V2XModuleHelper", LOG_LEVEL_INFO);
  LogComponentEnable("LTE-V2X-VManager", LOG_LEVEL_INFO);
  LogComponentEnable("LTE-V2X-SINR-MAPPING", LOG_LEVEL_INFO);
 
  /* run the simulations */
  do_run_simulation (argc, argv);

  return 0;


}
