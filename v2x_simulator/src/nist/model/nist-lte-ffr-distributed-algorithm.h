/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Piotr Gawlowicz
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
 * Author: Piotr Gawlowicz <gawlowicz.p@gmail.com>
 *
 */

#ifndef NIST_LTE_FFR_DISTRIBUTED_ALGORITHM_H
#define NIST_LTE_FFR_DISTRIBUTED_ALGORITHM_H

#include <ns3/nist-lte-ffr-algorithm.h>
#include <ns3/nist-lte-ffr-sap.h>
#include <ns3/nist-lte-ffr-rrc-sap.h>
#include <ns3/nist-lte-rrc-sap.h>

namespace ns3 {

/**
 * \brief Distributed Fractional Frequency Reuse algorithm implementation.
 */
class NistLteFfrDistributedAlgorithm : public NistLteFfrAlgorithm
{
public:
  NistLteFfrDistributedAlgorithm ();
  virtual ~NistLteFfrDistributedAlgorithm ();

  // inherited from Object
  static TypeId GetTypeId ();

  // inherited from NistLteFfrAlgorithm
  virtual void SetNistLteFfrSapUser (NistLteFfrSapUser* s);
  virtual NistLteFfrSapProvider* GetNistLteFfrSapProvider ();

  virtual void SetNistLteFfrRrcSapUser (NistLteFfrRrcSapUser* s);
  virtual NistLteFfrRrcSapProvider* GetNistLteFfrRrcSapProvider ();

  // let the forwarder class access the protected and private members
  friend class MemberNistLteFfrSapProvider<NistLteFfrDistributedAlgorithm>;
  friend class MemberNistLteFfrRrcSapProvider<NistLteFfrDistributedAlgorithm>;

protected:
  // inherited from Object
  virtual void DoInitialize ();
  virtual void DoDispose ();

  virtual void Reconfigure ();

  // FFR SAP PROVIDER IMPLEMENTATION
  virtual std::vector <bool> DoGetAvailableDlRbg ();
  virtual bool DoIsDlRbgAvailableForUe (int i, uint16_t rnti);
  virtual std::vector <bool> DoGetAvailableUlRbg ();
  virtual bool DoIsUlRbgAvailableForUe (int i, uint16_t rnti);
  virtual void DoReportDlCqiInfo (const struct NistFfMacSchedSapProvider::NistSchedDlCqiInfoReqParameters& params);
  virtual void DoReportUlCqiInfo (const struct NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters& params);
  virtual void DoReportUlCqiInfo ( std::map <uint16_t, std::vector <double> > ulCqiMap );
  virtual uint8_t DoGetTpc (uint16_t rnti);
  virtual uint8_t DoGetMinContinuousUlBandwidth ();

  // FFR SAP RRC PROVIDER IMPLEMENTATION
  virtual void DoReportUeMeas (uint16_t rnti, NistLteRrcSap::NistMeasResults measResults);
  virtual void DoRecvLoadInformation (NistEpcX2Sap::NistLoadInformationParams params);

private:
  void SetDownlinkConfiguration (uint16_t cellId, uint8_t bandwidth);
  void SetUplinkConfiguration (uint16_t cellId, uint8_t bandwidth);
  void InitializeDownlinkRbgMaps ();
  void InitializeUplinkRbgMaps ();

  void UpdateNeighbourMeasurements (uint16_t rnti, uint16_t cellId, uint8_t rsrp, uint8_t rsrq);

  void Calculate ();
  void SendLoadInformation (uint16_t targetCellId);

  // FFR SAP
  NistLteFfrSapUser* m_ffrSapUser;
  NistLteFfrSapProvider* m_ffrSapProvider;

  // FFR RRF SAP
  NistLteFfrRrcSapUser* m_ffrRrcSapUser;
  NistLteFfrRrcSapProvider* m_ffrRrcSapProvider;

  std::vector <bool> m_dlRbgMap;
  std::vector <bool> m_ulRbgMap;

  uint8_t m_edgeRbNum;
  std::vector <bool> m_dlEdgeRbgMap;
  std::vector <bool> m_ulEdgeRbgMap;

  enum UePosition
  {
    AreaUnset,
    CenterArea,
    EdgeArea
  };

  std::map< uint16_t, uint8_t > m_ues;

  uint8_t m_egdeSubBandRsrqThreshold;

  uint8_t m_centerPowerOffset;
  uint8_t m_edgePowerOffset;

  uint8_t m_centerAreaTpc;
  uint8_t m_edgeAreaTpc;

  Time m_calculationInterval;
  EventId m_calculationEvent;

  // The expected measurement identity
  uint8_t m_rsrqMeasId;
  uint8_t m_rsrpMeasId;

  /**
   * \brief Measurements reported by a UE for a cell ID.
   *
   * The values are quantized according 3GPP TS 36.133 section 9.1.4 and 9.1.7.
   */
  class NistUeMeasure : public SimpleRefCount<NistUeMeasure>
  {
public:
    uint16_t m_cellId;
    uint8_t m_rsrp;
    uint8_t m_rsrq;
  };

  //               cellId
  typedef std::map<uint16_t, Ptr<NistUeMeasure> > MeasurementRow_t;
  //               rnti
  typedef std::map<uint16_t, MeasurementRow_t> MeasurementTable_t;
  MeasurementTable_t m_ueMeasures;

  std::vector<uint16_t> m_neigborCell;

  uint8_t m_rsrpDifferenceThreshold;

  std::map<uint16_t, uint32_t> m_cellWeightMap;

  std::map<uint16_t, std::vector <bool> > m_rntp;

}; // end of class NistLteFfrDistributedAlgorithm

} // end of namespace ns3

#endif /* LTE_FR_DISTRIBUTED_ALGORITHM_H */
