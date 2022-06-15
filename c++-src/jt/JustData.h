#ifndef JUSTDATA_H
#define JUSTDATA_H

/// @file JustData.h
/// @brief JustData のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
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
  /// @param[in] var_map 変数番号のマップ
  /// @param[in] model SATソルバの作ったモデル
  JustData(const VidMap& var_map,
	   const SatModel& model);

  /// @brief コンストラクタ(遷移故障用)
  /// @param[in] var1_map 1時刻目の変数番号のマップ
  /// @param[in] var2_map 2時刻目の変数番号のマップ
  /// @param[in] model SATソルバの作ったモデル
  JustData(const VidMap& var1_map,
	   const VidMap& var2_map,
	   const SatModel& model);

  /// @brief デストラクタ
  ~JustData();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 遷移故障モードの時 true を返す．
  bool
  td_mode() const;

  /// @brief ノードの正常値を返す．
  /// @param[in] node ノード
  /// @param[in] time 時刻 ( 0 or 1 )
  Val3
  val(const TpgNode* node,
      int time) const;

  /// @brief 入力ノードの値を記録する．
  /// @param[in] node 対象の外部入力ノード
  /// @param[in] time 時刻 ( 0 or 1 )
  /// @param[out] assign_list 値の割当リスト
  void
  record_value(const TpgNode* node,
	       int time,
	       NodeValList& assign_list) const;

private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 1時刻目の変数マップ
  const VidMap& mVar0Map;

  // 2時刻目の変数マップ
  const VidMap& mVar1Map;

  // SAT ソルバの解
  const SatModel& mSatModel;

  // 遷移故障モード
  bool mTdMode;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ(縮退故障用)
// @param[in] var_map 変数番号のマップ
// @param[in] model SATソルバの作ったモデル
inline
JustData::JustData(const VidMap& var_map,
		   const SatModel& model) :
  mVar0Map(var_map),
  mVar1Map(var_map),
  mSatModel(model),
  mTdMode(false)
{
}

// @brief コンストラクタ(遷移故障用)
// @param[in] var0_map 1時刻目の変数番号のマップ
// @param[in] var1_map 2時刻目の変数番号のマップ
// @param[in] model SATソルバの作ったモデル
inline
JustData::JustData(const VidMap& var0_map,
		   const VidMap& var1_map,
		   const SatModel& model) :
  mVar0Map(var0_map),
  mVar1Map(var1_map),
  mSatModel(model),
  mTdMode(true)
{
}

// @brief デストラクタ
inline
JustData::~JustData()
{
}

// @brief 遷移故障モードの時 true を返す．
inline
bool
JustData::td_mode() const
{
  return mTdMode;
}

// @brief ノードの正常値を返す．
// @param[in] node ノード
// @param[in] time 時刻 ( 0 or 1 )
inline
Val3
JustData::val(const TpgNode* node,
	      int time) const
{
  const VidMap& varmap = (time == 0) ? mVar0Map : mVar1Map;
  return bool3_to_val3(mSatModel[varmap(node)]);
}

// @brief 入力ノードの値を記録する．
// @param[in] node 対象の外部入力ノード
// @param[in] time 時刻 (0 or 1)
// @param[out] assign_list 値の割当リスト
inline
void
JustData::record_value(const TpgNode* node,
		       int time,
		       NodeValList& assign_list) const
{
  Val3 v = val(node, time);
  if ( v != Val3::_X ) {
    bool bval = (v == Val3::_1);
    assign_list.add(node, time, bval);
  }
}

END_NAMESPACE_DRUID

#endif // JUSTDATA_H
