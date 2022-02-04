#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ns2-mobility-helper.h"
// #include “ns3/netanim-module.h”

using namespace ns3;


void showPosition (Ptr<Node> node, double deltaTime)
{
  uint32_t nodeId = node->GetId ();
  Ptr<MobilityModel> mobModel = node->GetObject<MobilityModel> ();
  Vector3D pos = mobModel->GetPosition ();
  Vector3D speed = mobModel->GetVelocity ();
  std::cout << "At " << Simulator::Now ().GetSeconds () << " node " << nodeId
            << ": Position(" << pos.x << ", " << pos.y << ", " << pos.z
            << ");   Speed(" << speed.x << ", " << speed.y << ", " << speed.z
            << ")" << std::endl;

  Simulator::Schedule (MilliSeconds (deltaTime), &showPosition, node, deltaTime);
}

int main (int argc, char *argv[])
{
// Create Ns2MobilityHelper for importing ns-2 format mobility trace
std::string traceFile = "src/mobility/examples/default.ns_movements";
double deltaTime = 1;
int numNodes = 1;

// Ns2MobilityHelper ns2 = Ns2MobilityHelper (traceFile);
// Create Moble nodes.
// NodeContainer MobileNodes;
// MobileNodes.Create (numNodes);
// configure movements for each node, while reading trace file
// ns2.Install ();

// for (int i=0; i< MobileNodes.GetN (); i++)
//   {
//     Ptr<Node> n = MobileNodes.Get(i);
//     Simulator::Schedule (Seconds (0.0), &showPosition, n, deltaTime);
//   }



Ptr<Node> n0 = CreateObject<Node> ();

  Ns2MobilityHelper ns2 = Ns2MobilityHelper (traceFile);
  ns2.Install ();

  Simulator::Schedule (Seconds (0.0), &showPosition, n0, deltaTime);

// AnimationInterface anim (“SimpleNS3SimulationWithns2-mobility-trace.xml”);
Simulator::Stop (Seconds (0.4));
Simulator::Run ();
return 0;
}
