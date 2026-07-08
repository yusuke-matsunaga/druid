
/// @file Reducer.cc
/// @brief Reducer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "Reducer.h"
#include "EqGroupMgr.h"
#include "PatGen.h"
#include "EqChecker.h"
#include "DomMgr.h"
#include "DomChecker.h"
#include "dtpg/NaiveDualEngine.h"
#include "ym/MtMgr.h"
#include "ym/IdPool.h"
#include "ym/ExLock.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

std::string
time_str(
  double time
)
{
  std::ostringstream buf;
  buf << std::setw(11)
      << std::fixed
      << std::setprecision(2)
      << time
      << "ms";
  return buf.str();
}

std::string
time_str(
  Timer& timer
)
{
  return time_str(timer.get_time());
}

void
print_candmgr(
  const EqGroupMgr* candmgr
)
{
  SizeType act_num = 0;
  for ( SizeType i = 0; i < candmgr->group_num(); ++ i ) {
    auto fault_list = candmgr->fault_list(i);
    if ( fault_list.size() == 0 ) {
      continue;
    }
    ++ act_num;
    if ( fault_list.size() > 1 ) {
      std::cout << "Group#" << i << ":";
      for ( auto fault: fault_list ) {
	std::cout << " " << fault.str();
      }
      std::cout << std::endl;
    }
  }
  std::cout << "Total " << act_num << " groups" << std::endl;
}

void
filtering(
  EqGroupMgr& eqmgr,
  const ConfigParam& option
)
{
  // パラメータの取得
  SizeType NO_CHANGE_LIMIT = option.get_int_elem("no_change_limit", 1000);
  SizeType BATCH_SIZE = option.get_int_elem("batch_size", 64);
  auto verbose = option.get_bool_elem("verbose", false);

  // パタンを作るオブジェクト
  auto patgen_option = option.get_param("patgen");
  auto patgen = PatGen::new_obj(eqmgr.fault_info(), patgen_option);

  Timer filter_timer;
  filter_timer.start();
  SizeType tv_count = 0;
  SizeType no_change = 0;
  while ( no_change < NO_CHANGE_LIMIT ) {
    // パタンを作る．
    std::vector<TestVector> tv_list(BATCH_SIZE);
    patgen->gen(BATCH_SIZE, tv_list);

    // 故障シミュレーションを行って故障グループを細分化する．
    auto change = eqmgr.subdivide(tv_list,
				  [&](const FsimResults& res) {
				    patgen->update(res);
				  });
    tv_count += BATCH_SIZE;

    if ( change ) {
      no_change = 0;
    }
    else {
      no_change += BATCH_SIZE;
    }
  }
  filter_timer.stop();

  if ( verbose ) {
    std::cout << "# of faults:            "
	      << std::setw(8) << std::right << eqmgr.fault_info().rep_fault_list().size() << std::endl
	      << "# of Groups:            "
	      << std::setw(8) << std::right << eqmgr.group_num() << std::endl
	      << "# of DomCand pairs:     "
	      << std::setw(8) << std::right << eqmgr.domcand_num() << std::endl
	      << "Total # of patterns:    "
	      << std::setw(8) << std::right << tv_count << std::endl
	      << "No Change Limit:        "
	      << std::setw(8) << std::right << NO_CHANGE_LIMIT << std::endl
	      << "Filtering time:           " << time_str(filter_timer) << std::endl
	      << " (Fsim time):             " << time_str(eqmgr.fsim_time())
	      << std::endl;
  }
}

void
check_equiv(
  EqGroupMgr& eqmgr,
  const ConfigParam& option
)
{
  auto multi_thread = option.get_bool_elem("multi_thread", false);
  SizeType thread_num = option.get_int_elem("thread_num", 0);
  auto verbose = option.get_bool_elem("verbose", false);
  auto debug = option.get_int_elem("debug", 0);

  SizeType check_count = 0;
  SizeType success_count = 0;
  Timer timer;
  timer.start();

  Timer update_timer;
  for ( ; ; ) {
    auto ng = eqmgr.group_num();
    std::vector<TestVector> tv_list;
    if ( multi_thread ) {
      IdPool pool(ng);
      ExLock lock;
      MtMgr::run(
	[&](SizeType th_id) {
	  EqChecker checker;
	  SizeType id;
	  while ( pool.get(id) ) {
	    checker.check_equiv(eqmgr, id, option);
	  }
	  lock.run([&]() {
	    checker.update_results(check_count, success_count, tv_list);
	  });
	}, thread_num
      );
    }
    else {
      EqChecker checker;
      for ( SizeType id = 0; id < ng; ++ id ) {
	checker.check_equiv(eqmgr, id, option);
      }
      checker.update_results(check_count, success_count, tv_list);
    }
    if ( tv_list.empty() ) {
      break;
    }
    // 反例を用いて細分化する．
    update_timer.start();
    eqmgr.subdivide(tv_list);
    update_timer.stop();
  }
  timer.stop();

  if ( verbose ) {
    auto rep_num = eqmgr.fault_info().rep_fault_list().size();
    std::cout << std::endl
	      << "Equivalence check end:    " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right
	      << check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right
	      << success_count << std::endl
	      << "# of faults:              " << std::setw(8) << std::right
	      << rep_num << std::endl
	      << "Equivalent check Time:    " << time_str(timer) << std::endl
	      << " (Update Time):           " << time_str(update_timer) << std::endl;

  }
}

void
check_dominance(
  DomMgr& dommgr,
  const ConfigParam& option
)
{
  auto multi_thread = option.get_bool_elem("multi_thread", false);
  SizeType thread_num = option.get_int_elem("thread_num", 0);
  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);
  auto verbose = option.get_bool_elem("verbose", false);
  auto debug = option.get_int_elem("debug", 0);

  SizeType check_count = 0;
  SizeType succ_count = 0;
  Timer timer;
  timer.start();

  Timer update_timer;
  for ( ; ; ) {
    std::vector<TestVector> tv_list;
    auto fault_list = dommgr.fault_info().rep_fault_list();
    if ( multi_thread ) {
      auto nf = fault_list.size();
      IdPool pool(nf);
      ExLock lock;
      MtMgr::run(
	[&](SizeType th_id) {
	  DomChecker checker;
	  SizeType id;
	  while ( pool.get(id) ) {
	    auto fault = fault_list[id];
	    checker.check_dominance(dommgr, fault, option);
	  }
	  lock.run([&]() {
	    checker.update_results(check_count, succ_count, tv_list);
	  });
	}, thread_num
      );
    }
    else {
      DomChecker checker;
      for ( auto fault2: fault_list ) {
	checker.check_dominance(dommgr, fault2, option);
      }
      checker.update_results(check_count, succ_count, tv_list);
    }
    if ( tv_list.empty() ) {
      break;
    }
    // 反例を用いて更新する．
    update_timer.start();
    dommgr.update(tv_list);
    update_timer.stop();
  }
  timer.stop();

  if ( verbose ) {
    std::cout << std::endl
	      << "Dominance check end:      " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right
	      << check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right
	      << succ_count << std::endl
	      << "Dominate check Time:      " << time_str(timer) << std::endl
	      << " (Update Time):           " << time_str(update_timer) << std::endl;
  }
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス Reducer
//////////////////////////////////////////////////////////////////////

// @brief 故障グループの細分化を行ってから支配関係を調べる．
void
Reducer::run(
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  // パラメータの取得
  auto verbose = option.get_bool_elem("verbose", false);
  auto debug = option.get_int_elem("debug", 0);

  // 現時点での代表故障のリスト
  auto fault_list = fault_info.rep_fault_list();

  // 故障シミュレータ
  auto fsim_option = option.get_param("fsim");
  Fsim fsim(fault_list, fsim_option);

  // 等価故障の候補を管理するオブジェクト
  auto eqmgr_option = option.get_param("eqmgr");
  auto eqmgr_ptr = EqGroupMgr::new_obj(fault_info, fsim, eqmgr_option);
  auto& eqmgr = *eqmgr_ptr;

  // 故障シミュレータを用いて支配故障の候補のフィルタリングを行う．
  filtering(eqmgr, option);

  // 等価故障の検査を行う．
  check_equiv(eqmgr, option);

  // 支配故障の検査を行う．
  DomMgr dommgr(eqmgr);
  check_dominance(dommgr, option);
}

// @brief 支配関係を用いて対象の故障を削減する．
void
Reducer::naive_run(
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);
  auto verbose = option.get_bool_elem("verbose", false);
  auto debug = option.get_int_elem("debug", 0);

  SizeType check_count = 0;
  SizeType succ_count = 0;

  Timer timer;
  timer.start();

  // 現時点での代表故障のリスト
  auto fault_list = fault_info.rep_fault_list();
  auto nf = fault_list.size();
  for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
    auto fault1 = fault_list[i1];
    if ( !fault_info.is_rep(fault1) ) {
      continue;
    }
    for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
      auto fault2 = fault_list[i2];
      if ( !fault_info.is_rep(fault2) ) {
	continue;
      }
      NaiveDualEngine engine(fault1, fault2, option);
      auto res1 = engine.solve(true, false, TIME_LIMIT);
      auto res2 = engine.solve(false, true, TIME_LIMIT);
      ++ check_count;
      if ( res1 == SatBool3::False && res2 == SatBool3::False ) {
	// fault1 と fault2 は等価故障
	fault_info.set_rep(fault2, fault1);
	++ succ_count;
	continue;
      }
      if ( res1 == SatBool3::False ) {
	// fault2 は fault1 に支配されている．
	fault_info.set_dominator(fault2, fault1);
	++ succ_count;
	continue;
      }
      if ( res2 == SatBool3::False ) {
	// fault1 は fault2 に支配されている．
	fault_info.set_dominator(fault1, fault2);
	++ succ_count;
	break;
      }
    }
  }
  timer.stop();

  if ( verbose ) {
    std::cout << std::endl
	      << "Dominance check end:      " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right
	      << check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right
	      << succ_count << std::endl
	      << "Dominate check Time:      " << time_str(timer) << std::endl;
  }
}

END_NAMESPACE_DRUID
