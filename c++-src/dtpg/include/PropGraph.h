#ifndef PROPGRAPH_H
#define PROPGRAPH_H

/// @file PropGraph.h
/// @brief PropGraph のヘッダファイル
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
/// @class PropGraph PropGraph.h "PropGraph.h"
/// @brief 故障の伝搬状況を表すクラス
///
/// 以下の情報を持つ．
/// - 起点となるノード
/// - 起点から到達可能なノードの印
/// - 変数割り当てマップ
/// - SATのモデル
//////////////////////////////////////////////////////////////////////
class PropGraph
{
public:

  /// @brief ノードの種類
  enum Status : std::uint8_t {
    Sensitized,  ///< 故障の影響が伝搬している．
    Blocked,     ///< 故障の影響がマスクされている．
    Unsensitied, ///< 故障の影響が伝搬していない．
    SideInput    ///< 起点のノードから到達可能でない．
  };


public:

  /// @brief コンストラクタ
  PropGraph(
    const TpgNode* root,    ///< [in] 起点となるノード
    const VidMap& gvar_map, ///< [in] 正常値の変数番号のマップ
    const VidMap& fvar_map, ///< [in] 故障値の変数番号のマップ
    const SatModel& model   ///< [in] SATソルバの作ったモデル
  );

  /// @brief デストラクタ
  ~PropGraph() = default;


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

  /// @brief node の種類を返す．
  Status
  node_status(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    mStatusDict.at(node->id());
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

  // ノードの状態を持つ辞書
  // キーはノード番号
  unordered_map<SizeType, Status> mStatusDict;

  // 故障差の伝搬している外部出力のリスト
  vector<const TpgNode*> mSensitizedOutputList;

};

END_NAMESPACE_DRUID

#endif // PROPGRAPH_H
