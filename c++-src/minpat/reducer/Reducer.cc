
/// @file Reducer.cc
/// @brief Reducer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "Reducer.h"
#include "EqDomMgr.h"
#include "PatGen.h"
#include "EqDomChecker.h"
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
  const EqDomMgr* candmgr
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
check_equiv(
  EqDomMgr* candmgr,
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

  for ( ; ; ) {
    auto ng = candmgr->group_num();
    bool changed = false;
    std::vector<TestVector> tv_list;
    if ( multi_thread ) {
      IdPool id_pool(ng);
      ExLock lock;
      MtMgr::run(
	[&]() {
	  for ( ; ; ) {
	    SizeType id;
	    if ( !id_pool.get(id) ) {
	      // 終わり
	      break;
	    }
	    EqDomChecker checker;
	    auto changed1 = checker.check_equiv(candmgr, id, option);
	    lock.run([&]() {
	      if ( changed1 ) {
		changed = true;
	      }
	      checker.update_results(check_count, success_count, tv_list);
	    });
	  }
	}, thread_num
      );
    }
    else {
      for ( SizeType i = 0; i < ng; ++ i ) {
	EqDomChecker checker;
	if ( checker.check_equiv(candmgr, i, option) ) {
	  changed = true;
	}
	checker.update_results(check_count, success_count, tv_list);
      }
    }
    if ( !tv_list.empty() ) {
      // 反例を用いて細分化する．
      if ( candmgr->subdivide(tv_list) ) {
	changed = true;
      }
    }
    if ( !changed ) {
      break;
    }
  }
  timer.stop();

  if ( verbose ) {
    auto rep_num = 0;
    for ( auto fault: candmgr->fault_list() ) {
      if ( candmgr->is_rep(fault) ) {
	++ rep_num;
      }
    }
    std::cout << std::endl
	      << "Equivalence check end:    " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right
	      << check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right
	      << success_count << std::endl
	      << "# of faults:              " << std::setw(8) << std::right
	      << rep_num << std::endl
	      << "CPU time:                 " << time_str(timer) << std::endl;

  }
}

void
check_dominance(
  EqDomMgr* candmgr,
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

  for ( ; ; ) {
    bool changed = false;
    std::vector<TestVector> tv_list;
    auto fault_list = candmgr->fault_list();
    if ( multi_thread ) {
      auto nf = fault_list.size();
      IdPool id_pool(nf);
      ExLock lock;
      MtMgr::run(
	[&]() {
	  for ( ; ; ) {
	    SizeType id;
	    if ( !id_pool.get(id) ) {
	      // 終わり
	      break;
	    }
	    auto fault = fault_list[id];
	    if ( !candmgr->is_rep(fault) ) {
	      continue;
	    }
	    EqDomChecker checker;
	    auto changed1 = checker.check_dominance(candmgr, fault, option);
	    lock.run([&]() {
	      if ( changed1 ) {
		changed = true;
	      }
	      checker.update_results(check_count, succ_count, tv_list);
	    });
	  }
	}, thread_num
      );
    }
    else {
      for ( auto fault2: fault_list ) {
	if ( !candmgr->is_rep(fault2) ) {
	  continue;
	}
	EqDomChecker checker;
	if ( checker.check_dominance(candmgr, fault2, option) ) {
	  changed = true;
	}
	checker.update_results(check_count, succ_count, tv_list);
      }
    }
    if ( !tv_list.empty() ) {
      // 反例を用いて細分化する．
      if ( candmgr->subdivide(tv_list) ) {
	changed = true;
      }
    }
    if ( !changed ) {
      break;
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
  SizeType NO_CHANGE_LIMIT = option.get_int_elem("no_change_limit", 1000);
  SizeType BATCH_SIZE = std::min(64, option.get_int_elem("batch_size", 64));
  auto verbose = option.get_bool_elem("verbose", false);
  auto debug = option.get_int_elem("debug", 0);

  // 現時点での代表故障のリスト
  auto fault_list = fault_info.rep_fault_list();

  // パタンを作るオブジェクト
  auto patgen_option = option.get_param("patgen");
  auto patgen = PatGen::new_obj(fault_info, patgen_option);

  // 等価故障/支配故障の候補を管理するオブジェクト
  auto candmgr_option = option.get_param("candmgr");
  auto candmgr = EqDomMgr::new_obj(fault_list, candmgr_option);

  Timer filter_timer;
  filter_timer.start();
  SizeType tv_count = 0;
  SizeType no_change = 0;
  while ( no_change < NO_CHANGE_LIMIT ) {
    // パタンを作る．
    std::vector<TestVector> tv_list(BATCH_SIZE);
    patgen->gen(BATCH_SIZE, tv_list);

    // 故障シミュレーションを行って故障グループを細分化する．
    auto change = candmgr->subdivide(tv_list,
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
	      << std::setw(8) << std::right << fault_list.size() << std::endl
	      << "# of Groups:            "
	      << std::setw(8) << std::right << candmgr->group_num() << std::endl
	      << "# of DomCand pairs:     "
	      << std::setw(8) << std::right << candmgr->domcand_num() << std::endl
	      << "Total # of patterns:    "
	      << std::setw(8) << std::right << tv_count << std::endl
	      << "No Change Limit:        "
	      << std::setw(8) << std::right << NO_CHANGE_LIMIT << std::endl
	      << "Filtering time:           " << time_str(filter_timer) << std::endl
	      << " (Fsim time):             " << time_str(candmgr->fsim_time())
	      << std::endl;
  }

  // 等価故障の検査を行う．
  check_equiv(candmgr.get(), option);

  // 支配故障の検査を行う．
  check_dominance(candmgr.get(), option);

  // candmgr の結果を FaultInfo に転送する．
  for ( auto fault: fault_info.rep_fault_list() ) {
    if ( !candmgr->is_rep(fault) ) {
      auto rep = candmgr->rep_fault(fault);
      fault_info.set_dominator(fault, rep);
    }
  }
}

END_NAMESPACE_DRUID
