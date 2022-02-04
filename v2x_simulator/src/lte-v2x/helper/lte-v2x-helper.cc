/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"

#include <random>
#include <math.h>


#include "lte-v2x-helper.h"
#include "ns3/lte-v2x.h"

namespace ns3 {


NS_LOG_COMPONENT_DEFINE ("LTE-V2XModuleHelper");

// mhasan: implementation of LTE-V2X

/* 
* code source:
* https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
*/
bool 
LTEV2X_Helper::IsStringEndsWith (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

// helper function obtained from bonnmotion-ns2-example.cc
void
LTEV2X_Helper::ShowPosition (Ptr<Node> node, double deltaTime)
{
  uint32_t nodeId = node->GetId ();
  Ptr<MobilityModel> mobModel = node->GetObject<MobilityModel> ();
  // NS_LOG_INFO ("Vehicle " << nodeId << " MobModel: " << mobModel );

  Vector3D pos = mobModel->GetPosition ();
  Vector3D speed = mobModel->GetVelocity ();
  NS_LOG_INFO ("At " << Simulator::Now ().GetMilliSeconds () << " msec. Vehicle " << nodeId
            << ": Position(" << pos.x << ", " << pos.y << ", " << pos.z
            << ");   Speed(" << speed.x << ", " << speed.y << ", " << speed.z
            << ")" );

  Simulator::Schedule (MilliSeconds (deltaTime), &LTEV2X_Helper::ShowPosition, this, node, deltaTime);
}

void
LTEV2X_Helper::PrintPositionByNode (Ptr<Node> node)
{
  uint32_t nodeId = node->GetId ();
  Ptr<MobilityModel> mobModel = node->GetObject<MobilityModel> ();
  // NS_LOG_INFO ("Vehicle " << nodeId << " MobModel: " << mobModel );

  Vector3D pos = mobModel->GetPosition ();
  Vector3D speed = mobModel->GetVelocity ();
  NS_LOG_INFO ("At " << Simulator::Now ().GetMilliSeconds () << " msec. Vehicle " << nodeId
            << ": Position(" << pos.x << ", " << pos.y << ", " << pos.z
            << ");   Speed(" << speed.x << ", " << speed.y << ", " << speed.z
            << ")" );

}

void
LTEV2X_Helper::DisplaySimParams (SimParam &simP)
{
  NS_LOG_INFO ("HARQ Enabled: " << simP.m_isHARQEnabled );
  NS_LOG_INFO ("Power Control Enabled: " << simP.m_isPowerControlEnabled );
  NS_LOG_INFO ("Rate Control Enabled: " << simP.m_isCongestControlEnabled );
  NS_LOG_INFO ("Fixed T2 Window: " << simP.m_isFixedT2Window );
  NS_LOG_INFO ("pKeep: " << simP.m_pKeep );
}


double
LTEV2X_Helper::GetDistanceByMobilityModelPair (Ptr<MobilityModel> a, Ptr<MobilityModel> b)
{
  double dist = a->GetDistanceFrom (b);
  return dist;
}


double
LTEV2X_Helper::DbmToWatt (double dbm)
{
  return  std::pow (10, (dbm - 30)/10); // in watt
}


double
LTEV2X_Helper::WattToDbm (double watt)
{
  return 10 * std::log10 (watt) + 30; // in dBm
}


// return a Nakagami variable with parameter m and Gamma
double
LTEV2X_Helper::GetNakagamiRandVar (double m, double gamma)
{
  // note: we can get Nakagami from Gamma (See Wiki)
  double k = m;
  double theta = gamma/m;
  Ptr<GammaRandomVariable> x = CreateObject<GammaRandomVariable> ();
  x->SetAttribute ("Alpha", DoubleValue (k));
  x->SetAttribute ("Beta", DoubleValue (1/theta));

  return std::sqrt (x->GetValue ());

}

/* returns a random integer within the given range */
int
LTEV2X_Helper::GetRandomNumberInRange (int minval, int maxval)
{
  std::random_device rand_dev;
  std::mt19937 generator(rand_dev());
  std::uniform_int_distribution<int>  distr(minval, maxval);
  return distr(generator);

}

/* returns a random real number within the given range */
double
LTEV2X_Helper::GetRandomRealNumberInRange (int minval, int maxval)
{
  std::random_device rand_dev;
  std::mt19937 generator(rand_dev());
  std::uniform_real_distribution<>  distr(minval, maxval);
  return distr(generator);

}





/* ... */


}
