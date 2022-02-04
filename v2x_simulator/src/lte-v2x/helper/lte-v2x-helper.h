/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef LTE_V2X_HELPER_H
#define LTE_V2X_HELPER_H

#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/log.h>
#include <ns3/core-module.h>

#include <random>

#include "ns3/lte-v2x.h"

namespace ns3 {


class LTEV2X_Helper
{
  public:

    void ShowPosition (Ptr<Node> node, double deltaTime);
    static void PrintPositionByNode (Ptr<Node> node);
    static void DisplaySimParams (SimParam &simP);
    static double GetDistanceByMobilityModelPair(Ptr<MobilityModel> a, Ptr<MobilityModel> b);
    static double DbmToWatt (double dbm);
    static double WattToDbm (double watt);
    static double GetNakagamiRandVar (double m, double gamma);
    static int GetRandomNumberInRange (int minval, int maxval);
    static double GetRandomRealNumberInRange (int minval, int maxval);
    static bool IsStringEndsWith (std::string const &fullString, std::string const &ending);


};  // end of helper class


/* ... */

}

#endif /* LTE_V2X_HELPER_H */
