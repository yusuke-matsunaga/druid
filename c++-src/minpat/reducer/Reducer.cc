
/// @file Reducer.cc
/// @brief Reducer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "Reducer.h"
#include "EqDomCandMgr.h"
#include "PatGen.h"
#include "dtpg/NaiveDualEngine.h"
#include "dtpg/SuffCond.h"


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
check_equiv(
  EqDomCandMgr* candmgr,
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);
  auto verbose = option.get_bool_elem("verbose", false);
  auto debug = option.get_int_elem("debug", 0);

  SizeType check_count = 0;
  SizeType success_count = 0;
  Timer timer;
  timer.start();
  for ( ; ; ) {
    auto ng = candmgr->group_num();
    bool changed = false;
    for ( SizeType i = 0; i < ng; ++ i ) {
      auto fault_list = candmgr->fault_list(i);
      auto nf = fault_list.size();
      if ( nf == 1 ) {
	continue;
      }

      // とりあえずリファレンス用に単純なアルゴリズムを用いる．
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

	  std::vector<TestVector> tv_list;
	  tv_list.reserve(2);
	  NaiveDualEngine engine(fault1, fault2, option);
	  ++ check_count;
	  auto res1 = engine.solve(true, false, TIME_LIMIT);
	  if ( res1 == SatBool3::False ) {
	    // fault2 は fault1 に支配されている．
	    fault_info.set_dominator(fault2, fault1);
	    ++ success_count;
	    changed = true;
	    continue;
	  }
	  if ( res1 == SatBool3::True ) {
	    // この時の入力を求める．
	    auto model = engine.solver().model();
	    auto pi_assign = engine.get_pi_assign(model);
	    auto tv = TestVector(pi_assign);
	    tv_list.push_back(tv);
	  }
	  auto res2 = engine.solve(false, true, TIME_LIMIT);
	  if ( res2 == SatBool3::False ) {
	    // fault1 は fault2 に支配されている．
	    fault_info.set_dominator(fault1, fault2);
	    ++ success_count;
	    changed = true;
	    break;
	  }
	  if ( res2 == SatBool3::True ) {
	    // この時の入力を求める．
	    auto model = engine.solver().model();
	    auto pi_assign = engine.get_pi_assign(model);
	    auto tv = TestVector(pi_assign);
	    tv_list.push_back(tv);
	  }
	  // ここに来たということは fault1 と fault2 の関係は不明
	  if ( !tv_list.empty() ) {
	    // 今求まったテストパタンで細分化する．
	    if ( candmgr->subdivide(tv_list) ) {
	      changed = true;
	      goto exit_loop;
	    }
	  }
	}
      }
    }
  exit_loop:
    if ( !changed ) {
      break;
    }
  }
  timer.stop();

  if ( verbose ) {
    auto rep_num = fault_info.rep_fault_list().size();
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
  EqDomCandMgr* candmgr,
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

  for ( ; ; ) {
    bool changed = false;
    auto fault_list = fault_info.rep_fault_list();
    for ( auto fault2: fault_list ) {
      if ( !fault_info.is_rep(fault2) ) {
	continue;
      }
      auto id2 = candmgr->group_id(fault2);
      auto prev_list = candmgr->prev_list(id2);
      bool done = false;
      for ( auto id1: prev_list ) {
	auto fault_list1 = candmgr->fault_list(id1);
	for ( auto fault1: fault_list1 ) {
	  if ( !fault_info.is_rep(fault1) ) {
	    continue;
	  }

	  std::vector<TestVector> tv_list;
	  tv_list.reserve(1);
	  NaiveDualEngine engine(fault1, fault2, option);
	  auto res = engine.solve(true, false, TIME_LIMIT);
	  ++ check_count;
	  if ( res == SatBool3::False ) {
	    fault_info.set_dominator(fault2, fault1);
	    ++ succ_count;
	    done = true;
	    if ( debug > 0 ) {
	      std::cout << "    "
			<< fault2.str() << " is dominated by "
			<< fault1.str() << std::endl;
	    }
	    break;
	  }
	  if ( res == SatBool3::True ) {
	    // この時の入力を求める．
	    auto model = engine.solver().model();
	    auto pi_assign = engine.get_pi_assign(model);
	    auto tv = TestVector(pi_assign);
	    tv_list.push_back(tv);
	  }
	  // ここに来たということは fault1 と fault2 の関係は不明
	  if ( !tv_list.empty() ) {
	    // 今求まったテストパタンで細分化する．
	    changed = candmgr->subdivide(tv_list);
	    if ( changed ) {
	      goto exit_loop;
	    }
	  }
	}
	if ( done ) {
	  break;
	}
      }
    }
  exit_loop:
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
  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);
  auto verbose = option.get_bool_elem("verbose", false);
  auto debug = option.get_int_elem("debug", 0);

  // 現時点での代表故障のリスト
  auto fault_list = fault_info.rep_fault_list();

  // パタンを作るオブジェクト
  auto patgen_option = option.get_param("patgen");
  auto patgen = PatGen::new_obj(fault_info, patgen_option);

  // 等価故障/支配故障の候補を管理するオブジェクト
  auto candmgr_option = option.get_param("candmgr");
  auto candmgr = EqDomCandMgr::new_obj(fault_list, candmgr_option);

#if 0
  auto network = fault_info.network();
  auto max_size = network.max_fault_id();
#endif

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
  check_equiv(candmgr.get(), fault_info, option);

  // 支配故障の検査を行う．
  check_dominance(candmgr.get(), fault_info, option);
}

END_NAMESPACE_DRUID
