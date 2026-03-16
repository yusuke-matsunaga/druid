
/// @file MpComp_Merge.cc
/// @brief MpComp_Merge の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpComp_Merge.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "fsim/Fsim.h"
#include "ym/MinCov.h"
#include "ym/UdGraph.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MpComp_Merge
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MpComp_Merge::MpComp_Merge()
{
}

// @brief デストラクタ
MpComp_Merge::~MpComp_Merge()
{
}

BEGIN_NONAMESPACE

// 各テストベクタの検出する故障のリストを作る．
std::vector<std::vector<SizeType>>
make_fault_list(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list
)
{
  auto fsim_option = JsonValue::object();
  fsim_option.add("has_x", true);
  Fsim fsim(fault_list, fsim_option);
  std::vector<std::vector<SizeType>> fault_list_array;
  auto ntv = tv_list.size();
  fault_list_array.reserve(ntv);
  for ( auto& tv: tv_list ) {
    auto res = fsim.sppfp(tv);
    fault_list_array.push_back(res.fault_list());
  }
  return fault_list_array;
}

// 各テストベクタが唯一検出故障のリストを作る．
std::vector<std::vector<SizeType>>
make_ex_list(
  const TpgFaultList& fault_list,
  const std::vector<std::vector<SizeType>>& fault_list_array
)
{
  auto n = fault_list_array.size();
  std::vector<SizeType> fault_list1(n);
  {
    std::vector<SizeType> acc_fault_list;
    for ( SizeType i = 0; i < n; ++ i ) {
      fault_list1[i] = acc_fault_list;
      acc_fault_list = merge(acc_fault_list, fault_list_array[i]);
    }
  }
  std::vector<SizeType> fault_list2(n);
  {
    std::vector<SizeType> acc_fault_list;
    for ( SizeType i = 0; i < n; ++ i ) {
      auto pos = n - i - 1;
      fault_list1[pos] = acc_fault_list;
      acc_fault_list = merge(acc_fault_list, fault_list_array[pos]);
    }
  }
  auto network = fault_list.network();
  std::vector<std::vector<SizeType>> ex_list_array(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    std::vector<bool> mark(network.max_fault_id(), false);
    for ( auto id: fault_list1 ) {
      mark[id] = true;
    }
    for ( auto id: fault_list2 ) {
      mark[id] = true;
    }
    for ( auto fault: fault_list ) {
      auto id = fault.id();
      if ( !mark[id] ) {
	ex_list_array[i].push_back(id);
      }
    }
  }
  return ex_list_array;
}

END_NONAMESPACE

// @brief パタン圧縮の本体
std::vector<TestVector>
MpComp_Merge::_run(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const JsonValue& option
)
{
  auto network = fault_list.network();
  auto n = tv_list.size();

  // 各テストベクタの検出する故障を求める．
  auto fault_list_array = make_fault_list(tv_list, fault_list);

  // 各テストベクタが唯一検出する故障を求める．
  auto ex_list_array = make_ex_list(fault_list, fault_list_array);

  // ex_list_array の要素数の昇順にソートする．
  std::vector<SizeType> pos_list(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    pos_list[i] = i;
  }
  std::sort(pos_list.begin(), pos_list.end(),
	    [&](SizeType a, SizeType b) -> bool {
	      return ex_list_array[a].size() < ex_list_array[b].size();
	    });

  SizeType limit = 1;

  // ex_list の要素数の少ないテストベクタの削除を試みる．
  while ( true ) {
    for ( SizeType i = 0; i < n; ++ i ) {
      auto tgt_id = pos_list[i];
      // tgt_id 番目のテストベクタの削除を試みる．
      auto& ex_list = ex_list_array[tgt_id];
      if ( ex_list.size() > limit ) {
	// 諦める．
	break;
      }
      //
      for ( auto fid: ex_list ) {
      }
    }
  }

  return tv_list3;
}

END_NAMESPACE_DRUID
