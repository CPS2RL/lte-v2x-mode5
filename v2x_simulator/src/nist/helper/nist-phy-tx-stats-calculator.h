/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Jaume Nin <jnin@cttc.es>
 * modified by: Marco Miozzo <mmiozzo@cttc.es>
 *        Convert NistMacStatsCalculator in NistPhyTxStatsCalculator
 * Modified by: NIST
 */

#ifndef NIST_PHY_TX_STATS_CALCULATOR_H_
#define NIST_PHY_TX_STATS_CALCULATOR_H_

#include "ns3/nist-lte-stats-calculator.h"
#include "ns3/nstime.h"
#include "ns3/uinteger.h"
#include <string>
#include <fstream>
#include <ns3/nist-lte-common.h>

namespace ns3 {

/**
 * \ingroup lte
 *
 * Takes care of storing the information generated at PHY layer regarding 
 * transmission. Metrics saved are:
 *
 *   - Timestamp (in seconds)
 *   - Frame index
 *   - Subframe index
 *   - C-RNTI
 *   - MCS for transport block 1
 *   - Size of transport block 1
 *   - MCS for transport block 2 (0 if not used)
 *   - Size of transport block 2 (0 if not used)
 */
class NistPhyTxStatsCalculator : public NistLteStatsCalculator
{
public:
  /**
   * Constructor
   */
  NistPhyTxStatsCalculator ();

  /**
   * Destructor
   */
  virtual ~NistPhyTxStatsCalculator ();

  // Inherited from ns3::Object
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);

  /**
   * Set the name of the file where the UL Tx PHY statistics will be stored.
   *
   * @param outputFilename string with the name of the file
   */
  void SetUlTxOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the UL RX PHY statistics will be stored.
   * @return the name of the file where the UL RX PHY statistics will be stored
   */
  std::string GetUlTxOutputFilename (void);

  /**
   * Set the name of the file where the DL TX PHY statistics will be stored.
   *
   * @param outputFilename string with the name of the file
   */
  void SetDlTxOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the DL TX PHY statistics will be stored.
   * @return the name of the file where the DL TX PHY statistics will be stored
   */
  std::string GetDlTxOutputFilename (void);

  /**
   * Set the name of the file where the SL TX PHY statistics will be stored.
   *
   * @param outputFilename string with the name of the file
   */
  void SetSlTxOutputFilename (std::string outputFilename);

  /**
   * Get the name of the file where the SL TX PHY statistics will be stored.
   * @return the name of the file where the SL TX PHY statistics will be stored
   */
  std::string GetSlTxOutputFilename (void);

  /**
   * Notifies the stats calculator that an downlink trasmission has occurred.
   * @param params Trace information regarding PHY transmission stats
   */
  void DlPhyTransmission (NistPhyTransmissionStatParameters params);

  /**
   * Notifies the stats calculator that an uplink trasmission has occurred.
   * @param params Trace information regarding PHY transmission stats
   */
  void UlPhyTransmission (NistPhyTransmissionStatParameters params);

  /**
   * Notifies the stats calculator that a sidelink trasmission has occurred.
   * @param params Trace information regarding PHY transmission stats
   */
  void SlPhyTransmission (NistPhyTransmissionStatParameters params);

  
  /** 
   * trace sink
   * 
   * \param phyTxStats 
   * \param path 
   * \param params 
   */
  static void DlPhyTransmissionCallback (Ptr<NistPhyTxStatsCalculator> phyTxStats,
                                  std::string path, NistPhyTransmissionStatParameters params);

  /** 
   * trace sink
   * 
   * \param phyTxStats 
   * \param path 
   * \param params 
   */
  static void UlPhyTransmissionCallback (Ptr<NistPhyTxStatsCalculator> phyTxStats,
                                  std::string path, NistPhyTransmissionStatParameters params);

  /** 
   * trace sink
   * 
   * \param phyTxStats 
   * \param path 
   * \param params 
   */
  static void SlPhyTransmissionCallback (Ptr<NistPhyTxStatsCalculator> phyTxStats,
                                  std::string path, NistPhyTransmissionStatParameters params);

private:
  /**
   * When writing DL TX PHY statistics first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_dlTxFirstWrite;

  /**
   * When writing UL TX PHY statistics first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_ulTxFirstWrite;

  /**
   * When writing SL TX PHY statistics first time to file,
   * columns description is added. Then next lines are
   * appended to file. This value is true if output
   * files have not been opened yet
   */
  bool m_slTxFirstWrite;
};

} // namespace ns3

#endif /* NIST_PHY_TX_STATS_CALCULATOR_H_ */
