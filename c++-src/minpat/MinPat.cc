
/// @file MinPat.cc
/// @brief MinPat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MinPat.h"
#include "dtpg/StructEngine.h"
#include "fsim/Fsim.h"
#include "types/TestVector.h"
#include "ym/MinCov.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MinPat
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MinPat::MinPat(
  const TpgNetwork& network,
  const TpgFaultList& fault_list,
  const std::vector<AssignList>& init_list
) : mNetwork{network},
    mFaultList{fault_list},
    mInitList{init_list},
    mEngine{mNetwork}
{
  mFidList.reserve(mFaultList.size());
  for ( auto fault: mFaultList ) {
    auto fid = fault.id();
    SizeType lid = mFidList.size();
    mFidList.push_back(fid);
    mFidMap.emplace(fid, lid);
  }
  for ( auto node: mNetwork.node_list() ) {
    mEngine.add_cur_node(node);
  }
  mEngine.update();
}

// @brief パタン圧縮を行う．
std::vector<TestVector>
MinPat::run(
  const JsonValue& option
)
{
  { // 初期解が正しいかチェックする．
    auto fsim_option = JsonValue::object();
    fsim_option.add("has_x", JsonValue(true));
    Fsim fsim(mNetwork, mFaultList, fsim_option);
    auto n = mFaultList.size();
    bool ok = true;
    for ( SizeType i = 0; i < n; ++ i ) {
      auto fault = mFaultList[i];
      auto& assign_list = mInitList[i];
      DiffBits dbits;
      auto res = fsim.xspsfp(assign_list, fault, dbits);
      if ( !res ) {
	std::cout << fault.str() << " is not detected" << std::endl;
	ok = false;
      }
    }
    if ( !ok ) {
      exit(1);
    }
  }

  // 最小被覆を求める．
  auto pat_list = mincov();

  // パタン圧縮を行う．
  auto pat_list2 = packing(pat_list);

  // TestVector に変換する．
  std::vector<TestVector> tv_list;
  tv_list.reserve(pat_list2.size());
  for ( auto& assign_list: pat_list2 ) {
    auto assumptions = mEngine.conv_to_literal_list(assign_list);
    auto res = mEngine.solve(assumptions);
    if ( res != SatBool3::True ) {
      throw std::logic_error{"assign_list is not satisfiable"};
    }
    auto pi_assign_list = mEngine.get_pi_assign();
    //auto pi_assign_list = mEngine.justify(assign_list, aux_side_inputs);
    auto tv = TestVector(pi_assign_list);
    tv_list.push_back(tv);
  }
  { // 最終チェック
    auto fsim_option = JsonValue::object();
    fsim_option.add("has_x", true);
    Fsim fsim(mNetwork, mFaultList, fsim_option);
    std::vector<bool> det_mark(mNetwork.max_fault_id(), false);
    for ( auto& tv: tv_list ) {
      auto res = fsim.sppfp(tv);
      for ( auto fid: res.fault_list(0) ) {
	det_mark[fid] = true;
	auto fault = mNetwork.fault(fid);
	fsim.set_skip(fault);
      }
    }
    for ( auto fid: mFidList ) {
      if ( !det_mark[fid] ) {
	auto fault = mNetwork.fault(fid);
	std::cout << fault.str()
		  << " is not detected" << std::endl;
      }
    }
  }
  return tv_list;
}

std::vector<AssignList>
MinPat::mincov()
{
  auto fsim_option = JsonValue::object();
  fsim_option.add("has_x", true);
  Fsim fsim(mNetwork, mFaultList, fsim_option);

  SizeType drop_limit = 4000;
  std::vector<SizeType> det_count(mNetwork.max_fault_id(), 0);
  MinCov mincov;
  SizeType col_pos = 0;
  for ( auto& assign_list: mInitList ) {
#if 1
    auto res = fsim.xsppfp(assign_list);
    for ( auto fid: res.fault_list(0) ) {
      auto row_pos = mFidMap.at(fid);
      mincov.insert_elem(row_pos, col_pos);
      ++ det_count[fid];
      if ( det_count[fid] >= drop_limit ) {
	auto fault = mNetwork.fault(fid);
	fsim.set_skip(fault);
      }
    }
#else
    for ( auto fault: mFaultList ) {
      if ( fsim.get_skip(fault) ) {
	continue;
      }
      DiffBits dbits;
      if ( fsim.xspsfp(assign_list, fault, dbits) ) {
	auto fid = fault.id();
	auto row_pos = mFidMap.at(fid);
	mincov.insert_elem(row_pos, col_pos);
	++ det_count[fid];
	if ( det_count[fid] >= drop_limit ) {
	  auto fault = mNetwork.fault(fid);
	  fsim.set_skip(fault);
	}
      }
    }
#endif
    ++ col_pos;
  }
  std::vector<SizeType> solution;
  auto nc = mincov.solve(solution);
  std::vector<AssignList> new_list;
  new_list.reserve(nc);
  for ( auto i: solution ) {
    new_list.push_back(mInitList[i]);
  }
  return new_list;
}

std::vector<AssignList>
MinPat::packing(
  const std::vector<AssignList>& pat_list
)
{
  //return pat_list;
  auto np = pat_list.size();

  // n^2 で両立ペアを求める．
  std::cout << "enumerating compatible pairs start" << std::endl;
  std::vector<std::vector<SizeType>> compat_list(np);
  for ( SizeType i1 = 0; i1 < np - 1; ++ i1 ) {
    auto& pat1 = pat_list[i1];
    auto lit_list1 = mEngine.conv_to_literal_list(pat1);
    for ( SizeType i2 = i1 + 1; i2 < np; ++ i2 ) {
      auto& pat2 = pat_list[i2];
      if ( check_conflict(pat1, pat2) ) {
	// パタンそのものが矛盾している．
	continue;
      }
      auto lit_list2 = mEngine.conv_to_literal_list(pat2);
      auto assumptions = lit_list1;
      assumptions.insert(assumptions.end(), lit_list2.begin(), lit_list2.end());
      auto res = mEngine.solve(assumptions);
      if ( res == SatBool3::True ) {
	// 両立
	compat_list[i1].push_back(i2);
	compat_list[i2].push_back(i1);
      }
    }
  }
  std::cout << "enumerating compatible pairs end" << std::endl;

  // 両立する数が最大のパタンを選ぶ．
  SizeType max_num = 0;
  std::vector<std::pair<SizeType, SizeType>> max_list;
  for ( SizeType i1 = 0; i1 < np - 1; ++ i1 ) {
    SizeType n1 = compat_list[i1].size();
    for ( auto i2: compat_list[i1] ) {
      SizeType n2 = compat_list[i2].size();
      auto num = n1 + n2;
      if ( max_num <= num ) {
	if ( max_num < num ) {
	  max_num = num;
	  max_list.clear();
	}
	max_list.push_back({i1, i2});
      }
    }
  }
  if ( max_list.size() == 0 ) {
    return pat_list;
  }

  // とりあえず先頭を選ぶ．
  auto& p = max_list.front();
  SizeType i1 = p.first;
  SizeType i2 = p.second;
  auto& pat1 = pat_list[i1];
  auto& pat2 = pat_list[i2];
  // マージする．
  auto new_pat = pat1;
  new_pat.merge(pat2);
  // pat_list を作り直す．
  std::vector<AssignList> new_pat_list;
  new_pat_list.reserve(np - 1);
  for ( SizeType i = 0; i < np; ++ i ) {
    if ( i == i1 || i == i2 ) {
      continue;
    }
    auto& pat = pat_list[i];
    new_pat_list.push_back(pat);
  }
  new_pat_list.push_back(new_pat);

  return packing(new_pat_list);
}

END_NAMESPACE_DRUID
