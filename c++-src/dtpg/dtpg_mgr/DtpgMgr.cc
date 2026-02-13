
/// @file DtpgMgr.cc
/// @brief DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/DtpgMgr.h"
#include "DtpgDriver.h"
#include "types/TpgNetwork.h"
#include "types/TpgFFR.h"
#include "types/TpgMFFC.h"
#include "types/TpgNode.h"
#include "types/OpBase.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// FFR に関係する故障を求める．
bool
get_faults(
  const TpgFFR& ffr,
  const std::vector<TpgFaultList>& node_fault_list_array,
  TpgFaultList& fault_list
)
{
  bool has_faults = false;
  for ( auto node: ffr.node_list() ) {
    for ( auto fault: node_fault_list_array[node.id()] ) {
      fault_list.push_back(fault);
      has_faults = true;
    }
  }
  return has_faults;
}

// MFFC に関係する故障を求める．
//
// 故障が唯一のFFR内にのみ存在する場合にはそのFFRを返す．
TpgFFR
get_faults(
  const TpgMFFC& mffc,
  const std::vector<TpgFaultList>& node_fault_list_array,
  TpgFaultList& fault_list
)
{
  bool ffr_mode = true;
  TpgFFR ffr1;
  for ( auto ffr: mffc.ffr_list() ) {
    if ( get_faults(ffr, node_fault_list_array, fault_list) ) {
      if ( !ffr1.is_valid() ) {
	ffr1 = ffr;
      }
      else {
	ffr_mode = false;
      }
    }
  }
  if ( !ffr_mode ) {
    ffr1 = TpgFFR();
  }
  return ffr1;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス DtpgMgr
//////////////////////////////////////////////////////////////////////

// @brief テスト生成を行う．
DtpgStats
DtpgMgr::run(
  const TpgFaultList& fault_list,
  DtpgResults& dtpg_results,
  const JsonValue& option
)
{
  std::string group_mode = "ffr";
  bool multi = false;
  // option の解析
  OpBase::get_string(option, "group_mode", group_mode);
  OpBase::get_bool(option, "multi_thread", multi);

  auto network = fault_list.network();

  // ノード番号をキーにして関係する故障番号のリストを格納する配列
  std::vector<TpgFaultList> node_fault_list_array(network.node_num());
  for ( auto fault: fault_list ) {
    auto node = fault.origin_node();
    node_fault_list_array[node.id()].push_back(fault);
  }

  // 結果をクリアしておく．
  dtpg_results.clear();

  DtpgStats stats;
  if ( group_mode == "node" ) {
    // ノード単位で処理を行う．
    for ( auto node: network.node_list() ) {
      auto driver = DtpgDriver::node_driver(node, option);
      for ( auto fault: node_fault_list_array[node.id()] ) {
	driver.gen_pattern(fault, dtpg_results, stats);
      }
      auto cnf_time = driver.cnf_time();
      stats.update_cnf(cnf_time);
      auto sat_stats = driver.sat_stats();
      stats.update_sat_stats(sat_stats);
    }
  }
  else if ( group_mode == "ffr" ) {
    // FFR 単位で処理を行う．
    for ( auto ffr: network.ffr_list() ) {
      // ffr に関係する故障を集める．
      TpgFaultList fault_list;
      if ( !get_faults(ffr, node_fault_list_array, fault_list) ) {
	continue;
      }

      auto driver = DtpgDriver::ffr_driver(ffr, option);
      for ( auto fault: fault_list ) {
	driver.gen_pattern(fault, dtpg_results, stats);
      }
      auto cnf_time = driver.cnf_time();
      stats.update_cnf(cnf_time);
      auto sat_stats = driver.sat_stats();
      stats.update_sat_stats(sat_stats);
    }
  }
  else if ( group_mode == "mffc" ) {
    // MFFC 単位で処理を行う．
    for ( auto mffc: network.mffc_list() ) {
      TpgFaultList fault_list;
      auto ffr = get_faults(mffc, node_fault_list_array, fault_list);
      if ( fault_list.empty() ) {
	continue;
      }
      if ( ffr.is_valid() ) {
	auto driver = DtpgDriver::ffr_driver(ffr, option);
	for ( auto fault: fault_list ) {
	  driver.gen_pattern(fault, dtpg_results, stats);
	}
	auto cnf_time = driver.cnf_time();
	stats.update_cnf(cnf_time);
	auto sat_stats = driver.sat_stats();
	stats.update_sat_stats(sat_stats);
      }
      else {
	auto driver = DtpgDriver::mffc_driver(mffc, option);
	for ( auto fault: fault_list ) {
	  driver.gen_pattern(fault, dtpg_results, stats);
	}
	auto cnf_time = driver.cnf_time();
	stats.update_cnf(cnf_time);
	auto sat_stats = driver.sat_stats();
	stats.update_sat_stats(sat_stats);
      }
    }
  }
  else {
    std::ostringstream buf;
    buf << group_mode << ": unknown value for 'group_mode'";
    throw std::invalid_argument{buf.str()};
  }

  return stats;
}

END_NAMESPACE_DRUID
