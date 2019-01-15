
/// @file Justifier.cc
/// @brief Justifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Justifier.h"
#include "Just1.h"
#include "Just2.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

std::unique_ptr<JustImpl>
new_just(const string& just_type,
	 int max_id)
{
  if ( just_type == "just1" ) {
    return std::unique_ptr<JustImpl>(new Just1(max_id));
  }
  if ( just_type == "just2" ) {
    return std::unique_ptr<JustImpl>(new Just2(max_id));
  }

  // デフォルトフォールバックは Just2
  return std::unique_ptr<JustImpl>(new Just2(max_id));
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Justifier
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] just_type Justifier の種類を表す文字列
// @param[in] network 対象のネットワーク
Justifier::Justifier(const string& just_type,
		     const TpgNetwork& network) :
  mNetwork(network),
  mImpl(new_just(just_type, network.node_num()))
{
}

// @brief デストラクタ
Justifier::~Justifier()
{
}

// @brief 正当化に必要な割当を求める(縮退故障用)．
// @param[in] assign_list 値の割り当てリスト
// @param[in] var_map 変数番号のマップ
// @param[in] model SAT問題の解
// @return テストベクタ
TestVector
Justifier::operator()(const NodeValList& assign_list,
		      const VidMap& var_map,
		      const vector<SatBool3>& model)
{
  NodeValList pi_assign_list = mImpl->justify(assign_list, var_map, model);
  return TestVector::new_from_assign_list(mNetwork.input_num(), mNetwork.dff_num(),
					  FaultType::StuckAt, pi_assign_list);
}

// @brief 正当化に必要な割当を求める(遷移故障用)．
// @param[in] assign_list 値の割り当てリスト
// @param[in] var1_map 1時刻目の変数番号のマップ
// @param[in] var2_map 2時刻目の変数番号のマップ
// @param[in] model SAT問題の解
// @return テストベクタ
TestVector
Justifier::operator()(const NodeValList& assign_list,
		      const VidMap& var1_map,
		      const VidMap& var2_map,
		      const vector<SatBool3>& model)
{
  NodeValList pi_assign_list = mImpl->justify(assign_list, var1_map, var2_map, model);
  return TestVector::new_from_assign_list(mNetwork.input_num(), mNetwork.dff_num(),
					  FaultType::TransitionDelay, pi_assign_list);
}

END_NAMESPACE_DRUID
