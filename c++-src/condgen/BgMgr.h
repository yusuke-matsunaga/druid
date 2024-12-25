#ifndef BGMGR_H
#define BGMGR_H

/// @file BgMgr.h
/// @brief BgMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/Bdd.h"


BEGIN_NAMESPACE_DRUID

class BgNode;

//////////////////////////////////////////////////////////////////////
/// @class BgMgr BgMgr.h "BgMgr.h"
/// @brief BG(BooleanGraph) を管理するグラフ
//////////////////////////////////////////////////////////////////////
class BgMgr
{
public:

  /// @brief コンストラクタ
  BgMgr(
    const Bdd& bdd ///< [in] BDD
  ) : BgMgr{vector<Bdd>{bdd}}
  {
  }

  /// @brief コンストラクタ
  BgMgr(
    const vector<Bdd>& bdd_list ///< [in] BDDのリスト
  );

  /// @brief デストラクタ
  ///
  /// 内部に持っているノードを開放する．
  ~BgMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 根のノードのリストを返す．
  const vector<const BgNode*>&
  root_list() const
  {
    return mRootList;
  }

  /// @brief 全ノードのリストを返す．
  const vector<const BgNode*>&
  node_list() const
  {
    return mNodeList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief Bdd を BgNode に変換する．
  const BgNode*
  convert(
    const Bdd& bdd ///< [in] BDD
  );

  /// @brief PLIT ノードを作る．
  const BgNode*
  new_PLIT(
    SizeType varid ///< [in] 変数番号
  );

  /// @brief NLIT ノードを作る．
  const BgNode*
  new_NLIT(
    SizeType varid ///< [in] 変数番号
  );

  /// @brief AND ノードを作る．
  const BgNode*
  new_AND(
    const Bdd& bdd ///< [in] 始点のノード
  );

  /// @brief OR ノードを作る．
  const BgNode*
  new_OR(
    const Bdd& bdd ///< [in] 始点のノード
  );

  /// @brief DEC ノードを作る．
  const BgNode*
  new_DEC(
    SizeType varid,          ///< [in] 変数番号
    const BgNode* cofactor0, ///< [in] 負のコファクター
    const BgNode* cofactor1  ///< [in] 正のコファクター
  );

  /// @brief AND ノードの境界を求める．
  const BgNode*
  search_AND(
    Bdd bdd,                            ///< [in] 始点のノード
    vector<const BgNode*>& operand_list ///< [out] オペランドリストを格納するリスト
  );

  /// @brief OR ノードの境界を求める．
  const BgNode*
  search_OR(
    Bdd bdd,                            ///< [in] 始点のノード
    vector<const BgNode*>& operand_list ///< [out] オペランドリストを格納するリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // BDDをキーにして対応するノードを保持する辞書
  std::unordered_map<Bdd, const BgNode*> mNodeDict;

  // 根のノードのリスト
  vector<const BgNode*> mRootList;

  // 全てのノードのリスト
  vector<const BgNode*> mNodeList;

};

END_NAMESPACE_DRUID

#endif // BGMGR_H
