
/// @file FaultEnc.cc
/// @brief FaultEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/FaultEnc.h"
#include "types/TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FaultEnc::FaultEnc(
  const TpgFault& fault
) : mFault{fault}
{
  auto ex_cond = mFault.excitation_condition();
  mNodeList.reserve(ex_cond.size());
  for ( auto nv: ex_cond ) {
    auto node = nv.node();
    auto time = nv.time();
    if ( time == 1 ) {
      mNodeList.push_back(node);
    }
    else {
      mPrevList.push_back(node);
    }
  }
}

// @brief データ構造の初期化を行う．
void
FaultEnc::init()
{
}

// @brief 必要な変数を割り当てCNF式を作る．
void
FaultEnc::make_cnf()
{
  mPropVar = new_variable(true);
  auto ex_cond = mFault.excitation_condition();
  auto tmp_lits = conv_to_literal_list(ex_cond);
  solver().add_andgate(mPropVar, tmp_lits);
}

// @brief 関連するノードのリストを返す．
const TpgNodeList&
FaultEnc::node_list() const
{
  return mNodeList;
}

// @brief 1時刻前の値に関連するノードのリストを返す．
const TpgNodeList&
FaultEnc::prev_node_list() const
{
  return mPrevList;
}

END_NAMESPACE_DRUID
