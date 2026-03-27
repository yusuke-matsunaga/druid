
/// @file MpAnalyze.cc
/// @brief MpAnalyze の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpAnalyze.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"
#include "fsim/Fsim.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
static int debug = 0;
END_NONAMESPACE

BEGIN_NONAMESPACE

// list1 に list2 をマージする．
// list1 と list2 は昇順にソートされていると仮定する．
std::vector<SizeType>
merge(
  const std::vector<SizeType>& list1,
  const std::vector<SizeType>& list2
)
{
  auto n1 = list1.size();
  auto n2 = list2.size();
  std::vector<SizeType> new_list;
  new_list.reserve(n1 + n2);
  SizeType i1 = 0;
  SizeType i2 = 0;
  while ( i1 < n1 && i2 < n2 ) {
    auto v1 = list1[i1];
    auto v2 = list2[i2];
    if ( v1 < v2 ) {
      new_list.push_back(v1);
      ++ i1;
    }
    else if ( v1 > v2 ) {
      new_list.push_back(v2);
      ++ i2;
    }
    else { // v1 == v2
      new_list.push_back(v1);
      ++ i1;
      ++ i2;
    }
  }
  for ( ; i1 < n1; ++ i1 ) {
    auto v1 = list1[i1];
    new_list.push_back(v1);
  }
  for ( ; i2 < n2; ++ i2 ) {
    auto v2 = list2[i2];
    new_list.push_back(v2);
  }
  return new_list;
}

END_NONAMESPACE

// @brief コンストラクタ
MpAnalyze::MpAnalyze(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  auto network = fault_list.network();
  auto fsim_option = option.get_param("fsim");
  fsim_option.add("has_x", true);
  Fsim fsim(fault_list, fsim_option);
  auto ntv = tv_list.size();
  mDetListArray.reserve(ntv);
  mExListArray.reserve(ntv);

  // Phase1:
  // 各テストベクタの検出する故障のリストを mDetListArray
  // に入れる．
  for ( auto& tv: tv_list ) {
    auto res = fsim.sppfp(tv);
    auto det_list = res.fault_list(0);
    mDetListArray.push_back(det_list);
  }

  // Phase2:
  // acc_fault_list1[i] に [0: i-1] のテストベクタ
  // で検出される故障リストを作る．
  std::vector<std::vector<SizeType>> acc_fault_list1(ntv);
  std::vector<SizeType> acc_faults;
  for ( SizeType i = 0; i < ntv; ++ i ) {
    auto& det_list = mDetListArray[i];
    acc_fault_list1[i] = acc_faults;
    acc_faults = merge(acc_faults, det_list);
  }

  // Phase3:
  // acc_fault_list2[i] に [i+1: ntv] のテストベクタ
  // で検出される故障のリストを作る．
  std::vector<std::vector<SizeType>> acc_fault_list2(ntv);
  acc_faults.clear();
  for ( SizeType i = 0; i < ntv; ++ i ) {
    auto pos = ntv - i - 1;
    auto& det_list = mDetListArray[pos];
    acc_fault_list2[pos] = acc_faults;
    acc_faults = merge(acc_faults, det_list);
  }

  // Phase4:
  // acc_fault_list1[i] と acc_fault_list2[i]
  // で検出されていない故障を求め mExListArray[i]
  // に格納する．
  for ( SizeType i = 0; i < ntv; ++ i ) {
    auto acc_list1 = acc_fault_list1[i];
    auto acc_list2 = acc_fault_list2[i];
    std::vector<bool> mark(network.max_fault_id(), false);
    for ( auto id: acc_fault_list1[i] ) {
      mark[id] = true;
    }
    for ( auto id: acc_fault_list2[i] ) {
      mark[id] = true;
    }
    std::vector<SizeType> ex_list;
    for ( auto fault: fault_list ) {
      auto id = fault.id();
      if ( !mark[id] ) {
	ex_list.push_back(id);
      }
    }
    mExListArray.push_back(ex_list);
  }
  if ( debug ) {
    std::cout << "MpAnalyze" << std::endl;
    auto pos_list = ex_pos_list();
    for ( SizeType i = 0; i < ntv; ++ i ) {
      auto pos = pos_list[i];
      std::cout << "----------------" << std::endl
		<< "TV#" << i << ": " << tv_list[pos].bin_str() << std::endl
		<< "Faults:" << std::endl;
      auto& fid_list = mDetListArray[pos];
      auto& ex_list = mExListArray[pos];
      std::unordered_set<SizeType> mark;
      for ( auto fid: ex_list ) {
	mark.insert(fid);
      }
      for ( auto fid: fid_list ) {
	auto fault = network.fault(fid);
	if ( mark.count(fid) > 0 ) {
	  std::cout << "*";
	}
	else {
	  std::cout << " ";
	}
	std::cout << fault.str() << std::endl;
      }
    }
  }
}

// @brief exclusive_num() の降順の番号リストを返す．
std::vector<SizeType>
MpAnalyze::ex_pos_list() const
{
  auto ntv = mExListArray.size();
  std::vector<SizeType> pos_array(ntv);
  for ( SizeType i = 0; i < ntv; ++ i ) {
    pos_array[i] = i;
  }
  std::sort(pos_array.begin(), pos_array.end(),
	    [&](SizeType a, SizeType b) -> bool {
	      auto v1 = exclusive_num(a);
	      auto v2 = exclusive_num(b);
	      if ( v1 > v2 ) {
		return true;
	      }
	      else if ( v1 < v2 ) {
		return false;
	      }
	      return det_num(a) > det_num(b);
	    });
  return pos_array;
}

END_NAMESPACE_DRUID
