
/// @file DtpgMgr.cc
/// @brief DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/DtpgMgr.h"
#include "dtpg/BdEngine.h"
#include "DtpgDriver.h"
#include "types/TpgNetwork.h"
#include "types/TpgFFR.h"
#include "types/TpgMFFC.h"
#include "types/TpgNode.h"
#include "ym/Timer.h"
#include "ym/MtMgr.h"
#include "ym/IdPool.h"
#include "ym/ExLock.h"
#include <thread>


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
DtpgResults
DtpgMgr::run(
  const TpgFaultList& fault_list,
  const JsonValue& option
)
{
  // option の解析
  auto group_mode = get_string(option, "group_mode", "ffr");
  auto multi = get_bool(option, "multi_thread", false);

  // ノード番号をキーにして関係する故障番号のリストを格納する配列
  auto network = fault_list.network();
  std::vector<TpgFaultList> node_fault_list_array(network.node_num());
  for ( auto fault: fault_list ) {
    auto node = fault.origin_node();
    node_fault_list_array[node.id()].push_back(fault);
  }

  // 結果を格納するオブジェクト
  DtpgResults dtpg_results;

  if ( group_mode == "node" ) { // ノード単位で処理を行う．
    for ( auto node: network.node_list() ) {
      auto& fault_list = node_fault_list_array[node.id()];
      auto driver = DtpgDriver(node, fault_list, option);
      driver.run();
      dtpg_results.merge(driver.results());
    }
  }
  else if ( group_mode == "node_mt" ) { // ノード単位でマルチスレッド実行を行う．
    SizeType thread_num = get_int(option, "thread_num", 0);
    IdPool id_pool(network.node_num());
    ExLock r_lock;
    MtMgr::run(
      [&](){
	for ( ; ; ) {
	  SizeType id;
	  if ( !id_pool.get(id) ) {
	    // 終わり
	    break;
	  }
	  auto node = network.node(id);
	  auto& fault_list = node_fault_list_array[node.id()];
	  auto driver = DtpgDriver(node, fault_list, option);
	  driver.run();
	  r_lock.run([&](){ dtpg_results.merge(driver.results()); });
	}
      }, thread_num
    );
  }
  else if ( group_mode == "ffr" ) { // FFR 単位で処理を行う．
    for ( auto ffr: network.ffr_list() ) {
      // ffr に関係する故障を集める．
      TpgFaultList fault_list;
      if ( !get_faults(ffr, node_fault_list_array, fault_list) ) {
	continue;
      }
      auto driver = DtpgDriver(ffr, fault_list, option);
      driver.run();
      dtpg_results.merge(driver.results());
    }
  }
  else if ( group_mode == "ffr_mt" ) { // FFR 単位でマルチスレッド実行を行う．
    // スレッド数
    SizeType thread_num = get_int(option, "thread_num", 0);
    IdPool id_pool(network.ffr_num());
    ExLock r_lock;
    MtMgr::run(
      [&](){
	for ( ; ; ) {
	  SizeType id;
	  if ( !id_pool.get(id) ) {
	    // 終わり
	    break;
	  }
	  auto ffr = network.ffr(id);
	  TpgFaultList fault_list;
	  if ( !get_faults(ffr, node_fault_list_array, fault_list) ) {
	    continue;
	  }
	  auto driver = DtpgDriver(ffr, fault_list, option);
	  driver.run();
	  r_lock.run([&](){ dtpg_results.merge(driver.results()); });
	}
      }, thread_num
    );
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
	auto driver = DtpgDriver(ffr, fault_list, option);
	driver.run();
	dtpg_results.merge(driver.results());
      }
      else {
	auto driver = DtpgDriver(mffc, fault_list, option);
	driver.run();
	dtpg_results.merge(driver.results());
      }
    }
  }
  else if ( group_mode == "mffc_mt" ) {
    SizeType thread_num = get_int(option, "thread_num", 0);
    IdPool id_pool(network.mffc_num());
    ExLock r_lock;
    MtMgr::run(
      [&](){
	for ( ; ; ) {
	  SizeType id;
	  if ( !id_pool.get(id) ) {
	    // 終わり
	    break;
	  }
	  auto mffc = network.mffc(id);
	  TpgFaultList fault_list;
	  auto ffr = get_faults(mffc, node_fault_list_array, fault_list);
	  if ( fault_list.empty() ) {
	    continue;
	  }
	  if ( ffr.is_valid() ) {
	    auto driver = DtpgDriver(ffr, fault_list, option);
	    driver.run();
	    r_lock.run([&](){ dtpg_results.merge(driver.results()); });
	  }
	  else {
	    auto driver = DtpgDriver(mffc, fault_list, option);
	    driver.run();
	    r_lock.run([&](){ dtpg_results.merge(driver.results()); });
	  }
	}
      }, thread_num
    );
  }
  else {
    std::ostringstream buf;
    buf << group_mode << ": unknown value for 'group_mode'";
    throw std::invalid_argument{buf.str()};
  }

  return dtpg_results;
}

// @brief 検出可能かチェックする．
std::vector<bool>
DtpgMgr::check(
  const TpgFault& fault,
  const std::vector<AssignList>& assign_list_array
)
{
  auto network = fault.network();
  auto root = fault.ffr_root();
  BdEngine engine(root);

  { // assign_list_array に含まれるノードを登録しておく．
    for ( auto& assign_list: assign_list_array ) {
      for ( auto nv: assign_list ) {
	auto node = nv.node();
	auto time = nv.time();
	if ( time == 1 ) {
	  engine.add_cur_node(node);
	}
	else {
	  engine.add_prev_node(node);
	}
      }
    }
    engine.update();
  }

  // 故障の伝搬しない条件を作る．
  auto pvar = engine.prop_var();
  auto prop_cond = fault.ffr_propagate_condition();
  auto prop_lits = engine.conv_to_literal_list(prop_cond);
  std::vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(prop_lits.size() + 1);
  tmp_lits.push_back(~pvar);
  for ( auto lit: prop_lits ) {
    tmp_lits.push_back(~lit);
  }
  engine.solver().add_clause(tmp_lits);

  std::vector<bool> res_array;
  res_array.reserve(assign_list_array.size());
  for ( auto& assign_list: assign_list_array ) {
    auto assumptions = engine.conv_to_literal_list(assign_list);
    auto res = engine.solve(assumptions);
    auto ok = (res == SatBool3::False);
    res_array.push_back(ok);
  }
  return res_array;
}

END_NAMESPACE_DRUID
