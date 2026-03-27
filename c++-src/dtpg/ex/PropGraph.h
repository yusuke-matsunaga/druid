#ifndef PROPGRAPH_H
#define PROPGRAPH_H

/// @file PropGraph.h
/// @brief PropGraph のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgNode.h"
#include "types/TpgNodeList.h"
#include "types/AssignList.h"
#include "dtpg/VidMap.h"
#include "types/Val3.h"
#include "ym/SatModel.h"
#include "PgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PropGraph PropGraph.h "PropGraph.h"
/// @brief 故障の伝搬状況を表すグラフ
///
/// TpgNode に対する正常値/故障値を保持する PgNode を管理する．
/// さらにその値が事前の割り当てによって確定しているかの情報も
/// 持つ
//////////////////////////////////////////////////////////////////////
class PropGraph
{
public:

  /// @brief コンストラクタ
  PropGraph(
    const TpgNode& root,          ///< [in] 起点となるノード
    const VidMap& gvar_map,       ///< [in] 正常値の変数番号のマップ
    const VidMap& fvar_map,       ///< [in] 故障値の変数番号のマップ
    const SatModel& model,        ///< [in] SATソルバの作ったモデル
    const AssignList& assign_list ///< [in] 事前の割り当て
  );

  /// @brief デストラクタ
  ~PropGraph() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 起点のノードを返す．
  TpgNode
  root() const
  {
    return mRoot;
  }

  /// @brief 正当化されている故障差の伝搬している出力のリストを返す．
  const TpgNodeList&
  justified_sensitized_output_list() const
  {
    return mJustifiedSensitizedOutputList;
  }

  /// @brief 故障差の伝搬している出力のリストを返す．
  const TpgNodeList&
  sensitized_output_list() const
  {
    return mSensitizedOutputList;
  }

  /// @brief 事前の割り当てを返す．
  const AssignList&
  assign_list() const
  {
    return mAssignList;
  }

  /// @brief PgNode を取り出す．
  PgNode*
  get_node(
    SizeType id ///< [in] ノード番号
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief node の TFO を登録する．
  void
  get_tfo(
    const TpgNode& node,    ///< [in] 対象のノード
    const VidMap& gvar_map, ///< [in] 正常値の変数番号のマップ
    const SatModel& model   ///< [in] SATソルバの作ったモデル
  );

  /// @brief 値が確定しているかチェックする．
  bool
  check_fixed(
    PgNode* pg_node,
    const TpgNode& node,
    bool gval
  );

  /// @brief PgNode を登録する．
  void
  put_node(
    PgNode* pg_node ///< [in] 登録するノード
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 起点となるノード
  TpgNode mRoot;

  // 正当化されている故障差の伝搬している外部出力のリスト
  TpgNodeList mJustifiedSensitizedOutputList;

  // 故障差の伝搬している外部出力のリスト
  TpgNodeList mSensitizedOutputList;

  // 事前の割り当て
  AssignList mAssignList;

  // PgNode の所有権を持つリスト
  std::vector<std::unique_ptr<PgNode>> mNodePool;

  // ノード番号をキーにして PgNode を持つ辞書
  std::unordered_map<SizeType, PgNode*> mNodeDict;

};

END_NAMESPACE_DRUID

#endif // PROPGRAPH_H
