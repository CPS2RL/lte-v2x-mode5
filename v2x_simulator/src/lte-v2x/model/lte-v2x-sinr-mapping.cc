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

NS_LOG_COMPONENT_DEFINE ("LTE-V2X-SINR-MAPPING");



    

int 
LTEV2X_System::GetHARQKeyFromTwoSINR (double sinr1, double sinr2)
{

  /* key is zero indexing */

  // some boundary checking
  if (sinr1 < _MIN_SINR_HARQ)
    sinr1 = (double) _MIN_SINR_HARQ;

  if (sinr1 > _MAX_SINR_HARQ)
    sinr1 = (double) _MAX_SINR_HARQ;

  if (sinr2 < _MIN_SINR_HARQ)
    sinr2 = (double) _MIN_SINR_HARQ;

  if (sinr2 > _MAX_SINR_HARQ)
    sinr2 = (double) _MAX_SINR_HARQ;

  int key = 0;
  for (int i=0; i<_N_SINR_HARQ_LIST_ENTRY; i+=1)
    {
      for (int j=0; j<_N_SINR_HARQ_LIST_ENTRY; j+=1)
        {
          if ( (sinr1 >= __SINR_LISTS[i] && sinr1 < __SINR_LISTS[i]+_INC_HARQ) && (sinr2 >= __SINR_LISTS[j] && sinr2 < __SINR_LISTS[j]+_INC_HARQ))
            {
              return key;
            }
          key++;
        }
        
    }
  return 0;  // this should be unusual, if so return error key
}
  

/* fill the table with mapping */

void 
LTEV2X_System::PopulateSINR2PERTable ()
{
    // for without HARQ
    DoPopulatePERWithoutHARQ ();
    
    // for HARQ
    DoPopulatePERWithHARQ ();

}


void 
LTEV2X_System::DoPopulatePERWithoutHARQ ()
{
  // (SINR, PER)

  /* populate tables */

  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(3,1));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(4,1));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(5,1));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(5.5,0.94392523364486));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(5.6,0.848739495798319));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(5.7,0.716312056737589));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(5.8,0.570621468926554));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(5.9,0.459090909090909));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(6,0.300595238095238));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(6.1,0.190566037735849));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(6.2,0.118963486454653));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(6.3,0.075));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(6.4,0.043));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(6.5,0.028));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(6.6,0.016));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(6.7,0.01));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(6.8,0.007));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(6.9,0.002));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(7,0));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(8,0));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(9,0));
  m_sinr2perTableNOHARQ__AWGN.push_back(std::make_pair(10,0));

  sort(m_sinr2perTableNOHARQ__AWGN.begin(), m_sinr2perTableNOHARQ__AWGN.end());
  

}




void 
LTEV2X_System::DoPopulatePERWithHARQ ()
{
  // (SINR, PER)

  /* populate tables */

  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(0,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(1,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(2,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(3,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(4,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(5,0.990196078));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(6,0.971153846));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(7,0.870689655));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(8,0.770992366));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(9,0.594117647));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(10,0.520618557));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(11,0.51010101));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(12,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(13,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(14,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(15,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(16,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(17,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(18,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(19,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(20,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(21,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(22,0.935185185));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(23,0.901785714));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(24,0.701388889));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(25,0.561111111));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(26,0.531578947));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(27,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(28,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(29,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(30,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(31,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(32,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(33,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(34,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(35,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(36,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(37,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(38,0.943925234));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(39,0.926605505));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(40,0.753731343));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(41,0.56741573));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(42,0.515306122));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(43,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(44,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(45,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(46,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(47,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(48,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(49,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(50,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(51,0.990196078));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(52,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(53,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(54,0.943925234));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(55,0.808));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(56,0.753731343));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(57,0.58045977));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(58,0.515306122));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(59,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(60,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(61,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(62,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(63,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(64,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(65,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(66,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(67,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(68,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(69,0.990196078));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(70,0.952830189));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(71,0.87826087));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(72,0.677852349));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(73,0.615853659));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(74,0.531578947));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(75,0.505));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(76,0.495098039));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(77,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(78,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(79,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(80,0.990196078));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(81,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(82,0.990196078));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(83,1));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(84,0.990196078));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(85,0.990196078));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(86,0.971153846));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(87,0.870689655));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(88,0.765151515));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(89,0.587209302));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(90,0.548913043));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(91,0.490291262));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(92,0.5));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(93,0.485576923));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(94,0.490291262));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(95,0.495098039));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(96,0.935185185));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(97,0.943925234));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(98,0.926605505));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(99,0.961904762));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(100,0.961904762));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(101,0.926605505));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(102,0.971153846));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(103,0.753731343));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(104,0.673333333));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(105,0.540106952));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(106,0.515306122));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(107,0.467592593));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(108,0.450892857));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(109,0.459090909));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(110,0.450892857));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(111,0.442982456));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(112,0.870689655));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(113,0.848739496));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(114,0.848739496));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(115,0.848739496));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(116,0.885964912));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(117,0.808));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(118,0.821138211));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(119,0.677852349));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(120,0.480952381));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(121,0.497536946));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(122,0.400793651));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(123,0.385496183));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(124,0.348275862));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(125,0.368613139));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(126,0.341216216));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(127,0.410569106));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(128,0.691780822));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(129,0.748148148));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(130,0.677852349));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(131,0.731884058));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(132,0.706293706));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(133,0.737226277));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(134,0.673333333));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(135,0.56741573));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(136,0.36996337));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(137,0.300595238));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(138,0.258974359));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(139,0.217672414));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(140,0.235981308));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(141,0.245145631));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(142,0.233796296));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(143,0.216738197));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(144,0.590643275));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(145,0.577142857));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(146,0.583815029));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(147,0.590643275));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(148,0.577142857));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(149,0.583815029));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(150,0.528795812));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(151,0.441048035));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(152,0.322683706));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(153,0.17003367));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(154,0.14469914));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(155,0.098));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(156,0.097));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(157,0.09));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(158,0.098));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(159,0.08));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(160,0.517948718));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(161,0.528795812));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(162,0.523316062));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(163,0.534391534));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(164,0.517948718));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(165,0.507537688));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(166,0.461187215));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(167,0.384030418));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(168,0.248157248));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(169,0.133069829));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(170,0.07));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(171,0.033));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(172,0.025));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(173,0.017));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(174,0.027));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(175,0.027));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(176,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(177,0.512690355));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(178,0.507537688));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(179,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(180,0.507537688));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(181,0.497536946));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(182,0.461187215));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(183,0.36996337));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(184,0.204868154));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(185,0.078));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(186,0.046));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(187,0.014));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(188,0.003));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(189,0.007));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(190,0.011));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(191,0.005));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(192,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(193,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(194,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(195,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(196,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(197,0.497536946));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(198,0.474178404));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(199,0.34006734));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(200,0.266490765));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(201,0.082));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(202,0.027));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(203,0.007));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(204,0.001));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(205,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(206,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(207,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(208,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(209,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(210,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(211,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(212,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(213,0.497536946));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(214,0.452914798));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(215,0.375464684));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(216,0.222958057));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(217,0.089));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(218,0.021));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(219,0.003));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(220,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(221,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(222,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(223,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(224,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(225,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(226,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(227,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(228,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(229,0.497536946));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(230,0.433476395));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(231,0.349480969));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(232,0.230068337));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(233,0.075));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(234,0.021));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(235,0.002));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(236,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(237,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(238,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(239,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(240,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(241,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(242,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(243,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(244,0.502487562));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(245,0.497536946));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(246,0.478672986));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(247,0.364620939));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(248,0.242206235));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(249,0.103589744));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(250,0.028));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(251,0.004));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(252,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(253,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(254,0));
  m_sinr2perTableWITHHARQ__AWGN.push_back(std::make_pair(255,0));

  sort(m_sinr2perTableWITHHARQ__AWGN.begin(), m_sinr2perTableWITHHARQ__AWGN.end());



}



/* ... */



}
