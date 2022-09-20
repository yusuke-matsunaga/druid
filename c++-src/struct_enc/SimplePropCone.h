#ifndef SIMPLEPROPCONE_H
#define SIMPLEPROPCONE_H

/// @file SimplePropCone.h
/// @brief SimplePropCone のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PropCone.h"


BEGIN_NAMESPACE_DRUID_STRUCTENC

//////////////////////////////////////////////////////////////////////
/// @class SimplePropCone SimplePropCone.h "SimplePropCone.h"
/// @brief 単純な PropCone
//////////////////////////////////////////////////////////////////////
class SimplePropCone :
  public PropCone
{
public:

  /// @brief コンストラクタ
  ///
  /// ブロックノードより先のノードは含めない．
  /// 通常 block_node は nullptr か root_node の dominator
  /// となっているはず．
  SimplePropCone(
    StructEnc& struct_enc,     ///< [in] StructEnc ソルバ
    const TpgNode* root_node,  ///< [in] FFRの根のノード
    const TpgNode* block_node, ///< [in] ブロックノード
    bool detect		       ///< [in] 故障を検出する時に true にするフラグ
  );

  /// @brief デストラクタ
  virtual
  ~SimplePropCone();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 関係するノードの変数を作る．
  void
  make_vars() override;

  /// @brief 関係するノードの入出力の関係を表すCNFを作る．
  void
  make_cnf() override;

  /// @brief 故障の影響伝搬させる条件を作る．
  vector<SatLiteral>
  make_condition(
    const TpgNode* root ///< [in] 起点となるノード
  ) override;

};

END_NAMESPACE_DRUID_STRUCTENC

#endif // SIMPLEPROPCONE_H
