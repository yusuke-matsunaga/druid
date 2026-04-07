
/// @file TpgFaultList.cc
/// @brief TpgFaultList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgFaultList.h"
#include "types/TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgFaultList
//////////////////////////////////////////////////////////////////////

// @brief ノードごとに分割した故障リストのリストを返す．
std::vector<TpgFaultList>
TpgFaultList::node_split() const
{
  auto network = TpgBase::network();
  std::vector<TpgFaultList> fault_list_array(network.node_num());
  for ( auto fault: *this ) {
    auto node = fault.origin_node();
    auto node_id = node.id();
    fault_list_array[node_id].push_back(fault);
  }
  return fault_list_array;
}

// @brief FFR ごとに分割した故障リストのリストを返す．
std::vector<TpgFaultList>
TpgFaultList::ffr_split() const
{
  auto network = TpgBase::network();
  std::vector<TpgFaultList> fault_list_array(network.ffr_num());
  for ( auto fault: *this ) {
    auto ffr = network.ffr(fault);
    auto ffr_id = ffr.id();
    fault_list_array[ffr_id].push_back(fault);
  }
  return fault_list_array;
}

// @brief MFFC ごとに分割した故障リストのリストを返す．
//
std::vector<std::pair<TpgFFR, TpgFaultList>>
TpgFaultList::mffc_split() const
{
  auto network = TpgBase::network();
  auto ffr_fault_list_array = ffr_split();
  std::vector<std::pair<TpgFFR, TpgFaultList>> ans_array(network.mffc_num());
  for ( auto mffc: network.mffc_list() ) {
    TpgFaultList fault_list;
    TpgFFR the_ffr;
    SizeType ffr_count = 0;
    for ( auto ffr: mffc.ffr_list() ) {
      auto& ffr_fault_list = ffr_fault_list_array[ffr.id()];
      if ( !ffr_fault_list.empty() ) {
	++ ffr_count;
	the_ffr = ffr;
	for ( auto fault: ffr_fault_list ) {
	  fault_list.push_back(fault);
	}
      }
    }
    if ( ffr_count != 1 ) {
      the_ffr = TpgFFR(); // 不正値
    }
    ans_array[mffc.id()] = {the_ffr, fault_list};
  }
  return ans_array;
}

END_NAMESPACE_DRUID
