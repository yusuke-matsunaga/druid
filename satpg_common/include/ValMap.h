#ifndef VALMAP_H
#define VALMAP_H

/// @file ValMap.h
/// @brief ValMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "Val3.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class ValMap ValMap.h "ValMap.h"
/// @brief SAT ソルバの model 配列からノードの値を読みだすためのクラス
//////////////////////////////////////////////////////////////////////
class ValMap
{
public:

  /// @brief 縮退故障用のコンストラクタ
  /// @param[in] gvar_map 正常値の変数マップ
  /// @param[in] fvar_map 故障値の変数マップ
  /// @param[in] model SATソルバの作ったモデル
  ValMap(const VidMap& gvar_map,
	 const VidMap& fvar_map,
	 const vector<SatBool3>& model);

  /// @brief 遷移故障用のコンストラクタ
  /// @param[in] hvar_map 1時刻前の正常値の変数マップ
  /// @param[in] gvar_map 正常値の変数マップ
  /// @param[in] fvar_map 故障値の変数マップ
  /// @param[in] model SATソルバの作ったモデル
  ValMap(const VidMap& hvar_map,
	 const VidMap& gvar_map,
	 const VidMap& fvar_map,
	 const vector<SatBool3>& model);

  /// @brief デストラクタ
  ~ValMap();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの1時刻前の正常値を返す．
  /// @param[in] node 対象のノード
  Val3
  hval(const TpgNode* node) const;

  /// @brief ノードの正常値を返す．
  /// @param[in] node 対象のノード
  Val3
  gval(const TpgNode* node) const;

  /// @brief ノードの故障値を返す．
  /// @param[in] node 対象のノード
  Val3
  fval(const TpgNode* node) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 1時刻前の正常値の変数マップ
  const VidMap& mHvarMap;

  // 正常値の変数マップ
  const VidMap& mGvarMap;

  // 故障値の変数マップ
  const VidMap& mFvarMap;

  // モデル
  const vector<SatBool3>& mModel;

};

END_NAMESPACE_YM_SATPG

#endif // VALMAP_H
