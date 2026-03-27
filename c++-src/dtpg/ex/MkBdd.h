#ifndef MKBDD_H
#define MKBDD_H

/// @file MkBdd.h
/// @brief MkBdd のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/BddMgr.h"


BEGIN_NAMESPACE_DRUID

class PropGraph;

//////////////////////////////////////////////////////////////////////
/// @class MkBdd MkBdd.h "MkBdd.h"pp
/// @brief 故障伝搬関数のBDDを作るためのクラス
//////////////////////////////////////////////////////////////////////
class MkBdd
{
public:

  /// @brief コンストラクタ
  MkBdd(
    const PropGraph& data, ///< [in] 故障伝搬の情報
    const std::vector<TpgNode>& node_list,
    const std::vector<TpgNode>& aux_side_inputs
  );

  /// @brief デストラクタ
  ~MkBdd() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの論理関数を表すBDDを作る．
  Bdd
  make_bdd(
    const TpgNode& node
  );

  /// @brief 起点のノードの変数
  BddVar
  root_var();

  /// @brief aux_side_input の変数
  BddVar
  aux_side_input_var(
    SizeType pos
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障伝搬の情報
  const PropGraph& mData;

  // BDDマネージャ
  BddMgr mMgr;

  // ノード番号をキーにしてBDDを持つ辞書
  std::unordered_map<SizeType, Bdd> mBddDict;

  // 次の変数番号
  SizeType mNextVar;

};

END_NAMESPACE_DRUID

#endif // MKBDD_H
