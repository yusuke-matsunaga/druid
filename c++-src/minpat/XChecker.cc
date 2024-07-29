
/// @file XChecker.cc
/// @brief XChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "XChecker.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TpgNodeSet.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
XChecker::XChecker(
  const TpgNetwork& network
) : mNetwork{network},
    mInputListArray(network.ffr_num())
{
  // FFR の構造を解析して関係ある入力ノードを求める．
  SizeType nn = mNetwork.node_num();
  for ( auto& ffr: mNetwork.ffr_list() ) {
    auto tmp_list = TpgNodeSet::get_tfo_list(nn, ffr->root());
    auto& input_list = mInputListArray[ffr->id()];
    TpgNodeSet::get_tfi_list(nn, tmp_list,
			     [&](const TpgNode* node) {
			       if ( node->is_ppi() ) {
				 input_list.push_back(node->id());
			       }
			     });
    sort(input_list.begin(), input_list.end());
  }
}

// @brief 2つの FFR が共通部分を持つか調べる．
bool
XChecker::check_intersect(
  const TpgFFR* ffr1,
  const TpgFFR* ffr2
) const
{
  auto& list_a = mInputListArray[ffr1->id()];
  auto& list_b = mInputListArray[ffr2->id()];
  auto rpos_a = list_a.begin();
  auto rpos_b = list_b.begin();
  auto epos_a = list_a.end();
  auto epos_b = list_b.end();
  while ( rpos_a != epos_a && rpos_b != epos_b ) {
    auto a = *rpos_a;
    auto b = *rpos_b;
    if ( a < b ) {
      ++ rpos_a;
    }
    else if ( a > b ) {
      ++ rpos_b;
    }
    else { // a == b
      return true;
    }
  }
  return false;
}

// @brief 2つの故障が共通部分を持つか調べる．
bool
XChecker::check_intersect(
  const TpgFault* fault1,
  const TpgFault* fault2
) const
{
  return check_intersect(mNetwork.ffr(fault1), mNetwork.ffr(fault2));
}

// @brief 2つの故障が共通部分を持つか調べる．
bool
XChecker::check_intersect(
  const TpgFault* fault1,
  const TpgFFR* ffr2
) const
{
  return check_intersect(mNetwork.ffr(fault1), ffr2);
}

END_NAMESPACE_DRUID
