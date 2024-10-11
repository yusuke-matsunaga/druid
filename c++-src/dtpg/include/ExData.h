#ifndef EXDATA_H
#define EXDATA_H

/// @file ExData.h
/// @brief ExData のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNode.h"
#include "VidMap.h"
#include "Val3.h"
#include "ym/SatModel.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ExData ExData.h "ExData.h"
/// @brief Extractor が用いるデータを表すクラス
///
/// 以下の情報を持つ．
/// - 起点となるノード
/// - 起点から到達可能なノードの印
/// - 変数割り当てマップ
/// - SATのモデル
//////////////////////////////////////////////////////////////////////
class ExData
{
public:

  /// @brief コンストラクタ
  ExData(
    const TpgNode* root,    ///< [in] 起点となるノード
    const VidMap& gvar_map, ///< [in] 正常値の変数番号のマップ
    const VidMap& fvar_map, ///< [in] 故障値の変数番号のマップ
    const SatModel& model   ///< [in] SATソルバの作ったモデル
  );

  /// @brief デストラクタ
  ~ExData() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 起点のノードを返す．
  const TpgNode*
  root() const
  {
    return mRoot;
  }

  /// @brief 故障差の伝搬している出力のリストを返す．
  const vector<const TpgNode*>&
  sensitized_output_list() const
  {
    return mSensitizedOutputList;
  }

  /// @brief ノードの出力に故障の影響を伝搬させる side input を求める．
  vector<const TpgNode*>
  backtrace(
    const TpgNode* node,
    vector<vector<const TpgNode*>>& cgroup_list
  );

  /// @brief 正常回路の値を返す．
  Val3
  gval(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return bool3_to_val3(mSatModel[mGvarMap(node)]);
  }

  /// @brief 故障回路の値を返す．
  Val3
  fval(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return bool3_to_val3(mSatModel[mFvarMap(node)]);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの出力の値を確定させる side input を求める．
  void
  backtrace2(
    const TpgNode* node,
    vector<const TpgNode*>& ncnode_list,
    vector<vector<const TpgNode*>>& cgroup_list
  );

  /// @brief root() から到達可能なノードの時に true を返す．
  bool
  is_in_fcone(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return mFconeMark.count(node->id()) > 0;
  }

  /// @brief ノードの種類を求める．
  /// @retval 1 故障差が伝搬している．
  /// @retval 2 故障差が伝搬していない．
  /// @retval 3 fcone の外側
  int
  type(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    if ( is_in_fcone(node) ) {
      if ( gval(node) != fval(node) ) {
	return 1;
      }
      return 2;
    }
    return 3;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 起点となるノード
  const TpgNode* mRoot;

  // 正常値を表す変数のマップ
  const VidMap& mGvarMap;

  // 故障値を表す変数のマップ
  const VidMap& mFvarMap;

  // SAT ソルバの解
  const SatModel& mSatModel;

  // 故障の fanout cone のマーク
  unordered_set<SizeType> mFconeMark;

  // 故障差の伝搬している外部出力のリスト
  vector<const TpgNode*> mSensitizedOutputList;

};

END_NAMESPACE_DRUID

#endif // EXDATA_H
