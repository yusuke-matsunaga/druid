#ifndef JUSTDATA_H
#define JUSTDATA_H

/// @file JustData.h
/// @brief JustData のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNode.h"
#include "VidMap.h"
#include "NodeValList.h"
#include "Val3.h"
#include "ym/SatModel.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class JustData JustData.h "JustData.h"
/// @brief Justify 用の情報を保持するクラス
//////////////////////////////////////////////////////////////////////
class JustData
{
public:

  /// @brief コンストラクタ(縮退故障用)
  JustData(
    const VidMap& var_map, ///< [in] 変数番号のマップ
    const SatModel& model  ///< [in] SATソルバの作ったモデル
  ) : mVar1Map{var_map},
      mVar2Map{var_map},
      mSatModel{model},
      mTdMode{false}
  {
  }

  /// @brief コンストラクタ(遷移故障用)
  JustData(
    const VidMap& var1_map, ///< [in] 1時刻目の変数番号のマップ
    const VidMap& var2_map, ///< [in] 2時刻目の変数番号のマップ
    const SatModel& model   ///< [in] SATソルバの作ったモデル
  ) : mVar1Map{var1_map},
      mVar2Map{var2_map},
      mSatModel{model},
      mTdMode{true}
  {
  }

  /// @brief デストラクタ
  ~JustData() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 遷移故障モードの時 true を返す．
  bool
  td_mode() const
  {
    return mTdMode;
  }

  /// @brief ノードの正常値を返す．
  Val3
  val(
    const TpgNode* node, ///< [in] ノード
    int time		 ///< [in] 時刻 ( 0 or 1 )
  ) const
  {
    const VidMap& varmap = (time == 0) ? mVar1Map : mVar2Map;
    return bool3_to_val3(mSatModel[varmap(node)]);
  }

  /// @brief 入力ノードの値を記録する．
  void
  record_value(
    const TpgNode* node,     ///< [in] 対象の外部入力ノード
    int time,		     ///< [in] 時刻 ( 0 or 1 )
    NodeValList& assign_list ///< [out] 値の割当リスト
  ) const
  {
    Val3 v = val(node, time);
    if ( v != Val3::_X ) {
      bool bval = (v == Val3::_1);
      assign_list.add(node, time, bval);
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 1時刻目の変数マップ
  const VidMap& mVar1Map;

  // 2時刻目の変数マップ
  const VidMap& mVar2Map;

  // SAT ソルバの解
  const SatModel& mSatModel;

  // 遷移故障モード
  bool mTdMode;

};

END_NAMESPACE_DRUID

#endif // JUSTDATA_H
