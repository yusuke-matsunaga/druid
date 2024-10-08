#ifndef EXDATA_H
#define EXDATA_H

/// @file ExData.h
/// @brief ExData のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNode.h"
#include "Assign.h"
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
/// - 起点から到達可能なノードのリスト
/// - 故障の影響が伝搬している出力のリスト
/// - 出力ごとに関係する境界ノードのリスト
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

  /// @brief 故障差の伝搬している出力のリストを返す．
  const vector<const TpgNode*>&
  sensitized_output_list() const
  {
    return mSensitizedOutputList;
  }

  /// @brief 出力に関係する境界ノードのリスト
  const vector<const TpgNode*>&
  boundary_node_list(
    const TpgNode* node ///< [in] 出力ノード
  ) const
  {
    return mBoundaryNodeListDict.at(node->id());
  }

  /// @brief ノードの side input を求める．
  void
  get_side_inputs(
    const TpgNode* node, ///< [in] 対象のノード
    vector<const TpgNode*>& side_inputs,
    vector<const TpgNode*>& cnode_list
  ) const;

  /// @brief root() から到達可能なノードの時に true を返す．
  bool
  is_in_fcone(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return mFconeMark.count(node->id()) > 0;
  }

  /// @brief ノードの値割り当てを返す．
  Assign
  get_assign(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    bool val = (gval(node) == Val3::_1);
    return Assign{node, 1, val};
  }

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

  /// @brief 起点のノードを返す．
  const TpgNode*
  root() const
  {
    return mRoot;
  }

  /// @brief root() から到達可能なノードのリストを返す．
  const vector<const TpgNode*>&
  fcone_node_list() const
  {
    return mFconeNodeList;
  }

  /// @brief 境界ノードを求める．
  void
  backtrace(
    const TpgNode* node,              ///< [in] 対象のノード
    vector<const TpgNode*>& node_list ///< [out] 境界ノードを格納するリスト
  );


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

  // mRoot の TFO のノードのリスト
  vector<const TpgNode*> mFconeNodeList;

  // 故障差の伝搬している外部出力のリスト
  vector<const TpgNode*> mSensitizedOutputList;

  // 境界ノードのリストの辞書
  // キーはノード番号
  std::unordered_map<SizeType, vector<const TpgNode*>> mBoundaryNodeListDict;

};

END_NAMESPACE_DRUID

#endif // EXDATA_H
