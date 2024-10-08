#ifndef PROPDATA_H
#define PROPDATA_H

/// @file PropData.h
/// @brief PropData のヘッダファイル
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
/// @class PropData PropData.h "PropData.h"
/// @brief 故障の伝搬状況を表すデータ構造
///
/// 以下の情報を持つ．
/// - 起点となるノード
/// - 起点から到達可能なノードのリスト
/// - 故障の影響が伝搬している出力のリスト
/// - 出力ごとに関係する境界ノードのリスト
/// - 変数割り当てマップ
/// - SATのモデル
//////////////////////////////////////////////////////////////////////
class PropData
{
public:

  /// @brief 一つの出力に関係する境界ノードの情報を表すデータ構造
  struct BoundaryData
  {
    const TpgNode* mOutput;             ///< 対応する出力ノード
    vector<const TpgNode*> mNodeList;   ///< 境界ノードのリスト
  };

  /// @brief 境界ノードの値割り当てに関するデータ構造
  struct AssignData
  {
    bool mAndCond;                      ///< [in] AND条件の時 true にするフラグ
    vector<Assign> mAssignList;         ///< [in] 割り当てリスト
  };


public:

  /// @brief コンストラクタ
  PropData(
    const TpgNode* root,    ///< [in] 起点となるノード
    const VidMap& gvar_map, ///< [in] 正常値の変数番号のマップ
    const VidMap& fvar_map, ///< [in] 故障値の変数番号のマップ
    const SatModel& model   ///< [in] SATソルバの作ったモデル
  );

  /// @brief デストラクタ
  ~PropData() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力ごとのデータのリストを返す．
  const vector<BoundaryData>&
  boundary_data_list() const
  {
    return mBoundaryDataList;
  }

  /// @brief 境界ノードの割り当てデータを返す．
  const AssignData&
  assign_data(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return mAssignDataDict.at(node->id());
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

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

  // 境界ノードのリスト
  vector<BoundaryData> mBoundaryDataList;

  // 割り当てデータの辞書
  // キーはノード番号
  std::unordered_map<SizeType, AssignData> mAssignDataDict;

};

END_NAMESPACE_DRUID

#endif // PROPDATA_H
