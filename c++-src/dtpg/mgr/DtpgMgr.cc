
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

//////////////////////////////////////////////////////////////////////
// クラス DtpgMgr
//////////////////////////////////////////////////////////////////////

// @brief テスト生成を行う．
DtpgResults
DtpgMgr::run(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  // option の解析
  auto group_mode = option.get_string_elem("group_mode", "ffr");
  auto multi = option.get_bool_elem("multi_thread", false);

  auto network = fault_list.network();

  // 結果を格納するオブジェクト
  DtpgResults dtpg_results(network.max_fault_id());
  if ( group_mode == "node" ) { // ノード単位で処理を行う．
    auto fault_list_array = fault_list.node_split();
    if ( multi ) {
      SizeType thread_num = option.get_int_elem("thread_num", 0);
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
	    auto& fault_list = fault_list_array[node.id()];
	    auto driver = DtpgDriver(node, fault_list, option);
	    auto stats = driver.run(dtpg_results);
	    r_lock.run([&](){ dtpg_results.merge_stats(stats); });
	  }
	}, thread_num
      );
    }
    else {
      for ( auto node: network.node_list() ) {
	auto& fault_list = fault_list_array[node.id()];
	auto driver = DtpgDriver(node, fault_list, option);
	auto stats = driver.run(dtpg_results);
	dtpg_results.merge_stats(stats);
      }
    }
  }
  else if ( group_mode == "ffr" ) { // FFR 単位で処理を行う．
    auto fault_list_array = fault_list.ffr_split();
    // 故障を持つ FFR番号のリスト
    std::vector<SizeType> id_list;
    id_list.reserve(network.ffr_num());
    for ( auto ffr: network.ffr_list() ) {
      auto& fault_list = fault_list_array[ffr.id()];
      if ( fault_list.size() > 0 ) {
	id_list.push_back(ffr.id());
      }
    }
    auto nid = id_list.size();
    if ( multi ) {
      // スレッド数
      SizeType thread_num = option.get_int_elem("thread_num", 0);
      IdPool id_pool(nid);
      ExLock r_lock;
      MtMgr::run(
	[&](){
	  for ( ; ; ) {
	    SizeType pos;
	    if ( !id_pool.get(pos) ) {
	      // 終わり
	      break;
	    }
	    auto id = id_list[pos];
	    auto ffr = network.ffr(id);
	    auto& fault_list = fault_list_array[id];
	    auto driver = DtpgDriver(ffr, fault_list, option);
	    auto stats = driver.run(dtpg_results);
	    r_lock.run([&](){ dtpg_results.merge_stats(stats); });
	  }
	}, thread_num
      );
    }
    else {
      for ( auto id: id_list ) {
	auto ffr = network.ffr(id);
	// ffr に関係する故障を集める．
	auto& fault_list = fault_list_array[id];
	auto driver = DtpgDriver(ffr, fault_list, option);
	auto stats = driver.run(dtpg_results);
	dtpg_results.merge_stats(stats);
      }
    }
  }
  else if ( group_mode == "mffc" ) { // MFFC 単位で処理を行う．
    auto fault_list_array = fault_list.mffc_split();
    if ( multi ) {
      SizeType thread_num = option.get_int_elem("thread_num", 0);
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
	    TpgFFR ffr;
	    TpgFaultList fault_list;
	    std::tie(ffr, fault_list) = fault_list_array[mffc.id()];
	    if ( fault_list.empty() ) {
	      continue;
	    }
	    if ( ffr.is_valid() ) {
	      auto driver = DtpgDriver(ffr, fault_list, option);
	      auto stats = driver.run(dtpg_results);
	      r_lock.run([&](){ dtpg_results.merge_stats(stats); });
	    }
	    else {
	      auto driver = DtpgDriver(mffc, fault_list, option);
	      auto stats = driver.run(dtpg_results);
	      r_lock.run([&](){ dtpg_results.merge_stats(stats); });
	    }
	    if ( fault_list.empty() ) {
	      continue;
	    }
	  }
	}, thread_num
      );
    }
    else {
      for ( auto mffc: network.mffc_list() ) {
	TpgFFR ffr;
	TpgFaultList fault_list;
	std::tie(ffr, fault_list) = fault_list_array[mffc.id()];
	if ( fault_list.empty() ) {
	  continue;
	}
	if ( ffr.is_valid() ) {
	  auto driver = DtpgDriver(ffr, fault_list, option);
	  auto stats = driver.run(dtpg_results);
	  dtpg_results.merge_stats(stats);
	}
	else {
	  auto driver = DtpgDriver(mffc, fault_list, option);
	  auto stats = driver.run(dtpg_results);
	  dtpg_results.merge_stats(stats);
	}
      }
    }
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
