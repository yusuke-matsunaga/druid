#ifndef CONEINFO_H
#define CONEINFO_H

/// @file ConeInfo.h
/// @brief ConeInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNetwork.h"
#include "TpgNodeSet.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ConeInfo ConeInfo.h "ConeInfo.h"
/// @brief DTPG 用のコーンの構造情報を持つ構造体
///
/// メンバは初期化されたら不変
//////////////////////////////////////////////////////////////////////
class ConeInfo
{
public:

  /// @brief コンストラクタ
  ConeInfo(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgNode* root        ///< [in] 故障伝搬の起点となるノード
  ) : mNetwork{network},
      mRoot{root},
      mTFOList{TpgNodeSet::get_tfo_list(network.node_num(), root,
					[&](const TpgNode* node) {
					  if ( node->is_ppo() ) {
					    mPPOList.push_back(node);
					  }
					})},
      mTFIList{TpgNodeSet::get_tfi_list(network.node_num(), mTFOList,
					[&](const TpgNode* node) {
					  if ( node->is_dff_output() ) {
					    mDffOutputList.push_back(node);
					  }
					})}
  {
  }

  /// @brief デストラクタ
  ~ConeInfo() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 起点のノードを返す．
  const TpgNode*
  root() const
  {
    return mRoot;
  }

  /// @brief TFO コーンのノードリストを返す．
  const vector<const TpgNode*>&
  tfo_list() const
  {
    return mTFOList;
  }

  /// @brief tfo_list() 中の PPO ノードのリストを返す．
  const vector<const TpgNode*>&
  ppo_list() const
  {
    return mPPOList;
  }

  /// @brief TFO コーンの TFI コーンのリストを返す．
  const vector<const TpgNode*>&
  tfi_list() const
  {
    return mTFIList;
  }

  /// @brief tfi_list() 中の DFF出力ノードのリストを返す．
  const vector<const TpgNode*>&
  dff_output_list() const
  {
    return mDffOutputList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 起点のノード
  const TpgNode* mRoot;

  // mRoot の TFO のノードのリスト
  vector<const TpgNode*> mTFOList;

  // mTFOList に含まれる PPO のリスト
  vector<const TpgNode*> mPPOList;

  // mTFOList の TFI のノードのリスト
  vector<const TpgNode*> mTFIList;

  // mTFIList 中の DFF出力ノードのリスト
  vector<const TpgNode*> mDffOutputList;

};

END_NAMESPACE_DRUID

#endif // CONEINFO_H
