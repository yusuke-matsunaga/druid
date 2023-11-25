#ifndef TD_BACKTRACER_H
#define TD_BACKTRACER_H

/// @file td/BackTracer.h
/// @brief BackTracer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/td_nsdef.h"


BEGIN_NAMESPACE_DRUID_TD

class BtImpl;

//////////////////////////////////////////////////////////////////////
/// @class BackTracer BackTracer.h "td/BackTracer.h"
/// @brief テストパタンを求めるためのバックトレースを行なうファンクター
//////////////////////////////////////////////////////////////////////
class BackTracer
{
public:

  /// @brief コンストラクタ
  /// @param[in] xmode モード
  /// @param[in] max_id ID番号の最大値
  BackTracer(ymuint xmode,
	     ymuint max_id);

  /// @brief デストラクタ
  ~BackTracer();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief バックトレースを行なう．
  /// @param[in] ffr_root 故障のあるFFRの根のノード
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] output_list 故障に関係する出力ノードのリスト
  /// @param[in] val_map ノードの値を保持するクラス
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  ///
  /// assign_list には故障の活性化条件と ffr_root までの故障伝搬条件
  /// を入れる．
  /// val_map には ffr_root のファンアウトコーン上の故障値と関係する
  /// 回路全体の正常値が入っている．
  void
  operator()(const TpgNode* ffr_root,
	     const NodeValList& assign_list,
	     const vector<const TpgNode*>& output_list,
	     const ValMap& val_map,
	     NodeValList& pi_assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // モード
  ymuint mXmode;

  // ノード番号の最大値
  ymuint mMaxId;

};

END_NAMESPACE_DRUID_TD

#endif // TD_BACKTRACER_H
