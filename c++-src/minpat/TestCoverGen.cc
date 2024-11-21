
/// @file TestCoverGen.cc
/// @brief TestCoverGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TestCoverGen.h"
#include "TestCover.h"
#include "FaultInfoMgr.h"
#include "FFRFaultList.h"
#include "ExCubeGen.h"
#include "OpBase.h"
#include "ym/Bdd.h"
#include "ym/BddVar.h"
#include "ym/BddMgr.h"
#include "ym/Timer.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

class CountBdd
{
public:

  CountBdd() = default;

  ~CountBdd() = default;


public:

  void
  run(
    const Bdd& bdd
  );

  SizeType
  clause_num() const
  {
    return mClauseNum;
  }

  SizeType
  literal_num() const
  {
    return mLiteralNum;
  }


private:

  void
  sub(
    const Bdd& bdd
  );


private:

  SizeType mClauseNum;

  SizeType mLiteralNum;

  std::unordered_set<Bdd> mMark;

};

void
CountBdd::run(
  const Bdd& bdd
)
{
  mClauseNum = 0;
  mLiteralNum = 0;
  if ( bdd.is_const() ) {
    return;
  }
  mMark.clear();
  sub(bdd);
}

void
CountBdd::sub(
  const Bdd& bdd
)
{
  if ( mMark.count(bdd) > 0 ) {
    return;
  }
  mMark.emplace(bdd);

  Bdd bdd0;
  Bdd bdd1;
  (void) bdd.root_decomp(bdd0, bdd1);
  if ( bdd0.is_const() ) {
    if ( bdd1.is_const() ) {
      mClauseNum += 1;
      mLiteralNum += 2;
    }
    else {
      sub(bdd1);
      if ( bdd0.is_zero() ) {
	mClauseNum += 2;
	mLiteralNum += 4;
      }
      else {
	mClauseNum += 1;
	mLiteralNum += 3;
      }
    }
  }
  else {
    sub(bdd0);
    if ( bdd1.is_zero() ) {
      mClauseNum += 2;
      mLiteralNum += 4;
    }
    else if ( bdd1.is_one() ) {
      mClauseNum += 1;
      mLiteralNum += 3;
    }
    else {
      sub(bdd1);
    }
  }
}

END_NONAMESPACE

// @brief 各故障のテストカバーを生成する．
vector<TestCover>
TestCoverGen::run(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  int debug = OpBase::get_debug(option);

  FFRFaultList ffr_fault_list{network, fault_list};

  vector<TestCover> cover_list;
  cover_list.reserve(fault_list.size());
  SizeType total_cube_num = 0;
  SizeType total_clause_num = 0;
  SizeType total_literal_num = 0;
  SizeType total_clause_num2 = 0;
  SizeType total_literal_num2 = 0;
  SizeType total_clause_num3 = 0;
  SizeType total_literal_num3 = 0;
  SizeType sop_count = 0;
  SizeType bdd_count = 0;
  const TpgFFR* max_ffr = nullptr;
  double max_time = 0.0;
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    if ( debug > 1 ) {
      DBG_OUT << "FFR#" << ffr->id()
	      << " [" << ffr_fault_list.fault_list(ffr).size() << "]"
	      << " / "
	      << ffr_fault_list.ffr_list().size() << endl;
    }
    Timer timer;
    timer.start();
    ExCubeGen gen{network, ffr, option};
    vector<Bdd> bdd_list;
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      auto testcover = gen.run(fault);
      cover_list.push_back(testcover);
      auto nc = testcover.cube_num();
      if ( nc > 1 ) {
	SizeType clause_num  = 0;
	SizeType literal_num = 0;
	cout << fault->str() << endl;
	total_cube_num += nc;
	clause_num = nc + 1;
	total_clause_num += clause_num;
	for ( auto& cube: testcover.cube_list() ) {
	  literal_num += cube.size() + 1;
	}
	total_literal_num += literal_num;
	if ( debug > 2 ) {
	  cout << "common cube: ";
	  for ( auto nv: testcover.common_cube() ) {
	    cout << " " << nv;
	  }
	  cout << endl;
	  for ( auto& cube: testcover.cube_list() ) {
	    for ( auto nv: cube ) {
	      cout << " " << nv;
	    }
	    cout << endl;
	  }
	}
#if 1
	BddMgr mgr;
	auto bdd = gen.make_bdd(mgr, testcover);
	if ( debug > 2 ) {
	  bdd.display(cout);
	}
	//bdd_list.push_back(bdd);
	auto n = bdd.size();
	cout << " " << testcover.cube_num()
	     << " | " << testcover.literal_num()
	     << " | " << n << endl;
	CountBdd count_bdd;
	count_bdd.run(bdd);
	SizeType clause_num2 = count_bdd.clause_num();
	SizeType literal_num2 = count_bdd.literal_num();
	total_clause_num2 += clause_num2;
	total_literal_num2 += literal_num2;
	if ( literal_num < literal_num2 ) {
	  total_clause_num3 += clause_num;
	  total_literal_num3 += literal_num;
	  ++ sop_count;
	}
	else {
	  total_clause_num3 += clause_num2;
	  total_literal_num3 += literal_num2;
	  ++ bdd_count;
	}
	cout << clause_num << " " << literal_num << endl
	     << clause_num2 << " " << literal_num2 << endl;
#endif
      }
      if ( debug > 1 ) {
	DBG_OUT << "  " << testcover.cube_num()
		<< " | " << testcover.literal_num() << endl;
      }
    }
    timer.stop();
    auto time = timer.get_time();
    if ( max_time < time ) {
      max_time = time;
      max_ffr = ffr;
#if 0
      cout << "max updated: FFR#" << max_ffr->id()
	   << " CPU Time: " << max_time << endl;
#endif
    }
  }

  timer.stop();
  if ( debug > 0 ) {
    DBG_OUT << "Total # of cubes:    " << total_cube_num << endl
	    << "Total # of literals: " << total_literal_num << endl
	    << "CPU time:            "
	    << (timer.get_time() / 1000.0) << endl;
  }
  cout << "Total # of cubes:          " << total_cube_num << endl
       << "Total # of clauses:        " << setw(10) << total_clause_num
       << " | " << setw(10) << total_literal_num << endl
       << "Total # of clauses(BDD):   " << setw(10) << total_clause_num2
       << " | " << setw(10) << total_literal_num2 << endl
       << "Total # of clauses(Best):  " << setw(10) << total_clause_num3
       << " | " << setw(10) << total_literal_num3 << endl;
  cout << "SOP count:                   " << sop_count << endl
       << "BDD count:                   " << bdd_count << endl;
  return cover_list;
}

// @brief 各故障のテストカバーを生成する．
vector<TestCover>
TestCoverGen::run(
  const FaultInfoMgr& finfo_mgr,
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  int debug = OpBase::get_debug(option);

  auto& network = finfo_mgr.network();
  auto& fault_list = finfo_mgr.active_fault_list();
  FFRFaultList ffr_fault_list{network, fault_list};

  vector<TestCover> cover_list;
  cover_list.reserve(fault_list.size());
  SizeType nc = 0;
  SizeType nl = 0;
  const TpgFFR* max_ffr = nullptr;
  double max_time = 0.0;
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    if ( debug > 1 ) {
      DBG_OUT << "FFR#" << ffr->id()
	      << " [" << ffr_fault_list.fault_list(ffr).size() << "]"
	      << " / "
	      << ffr_fault_list.ffr_list().size() << endl;
    }
    Timer timer;
    timer.start();
    ExCubeGen gen{network, ffr,
		  finfo_mgr.root_mandatory_condition(ffr),
		  option};
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      auto testcover = gen.run(fault);
      cover_list.push_back(testcover);
      nc += testcover.cube_num();
      nl += testcover.literal_num();
      if ( debug > 1 ) {
	DBG_OUT << "  " << testcover.cube_num()
		<< " | " << testcover.literal_num() << endl;
      }
    }
    timer.stop();
    auto time = timer.get_time();
    if ( max_time < time ) {
      max_time = time;
      max_ffr = ffr;
#if 0
      cout << "max updated: FFR#" << max_ffr->id()
	   << " CPU Time: " << max_time << endl;
#endif
    }
  }

  timer.stop();
  if ( debug > 0 ) {
    DBG_OUT << "Total # of cubes:    " << nc << endl
	    << "Total # of literals: " << nl << endl
	    << "CPU time:            "
	    << (timer.get_time() / 1000.0) << endl;
  }

  return cover_list;
}

END_NAMESPACE_DRUID
