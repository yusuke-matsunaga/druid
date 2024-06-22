#ifndef SIMPLEPROPCONE_H
#define SIMPLEPROPCONE_H

/// @file SimplePropCone.h
/// @brief SimplePropCone のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "PropCone.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class SimplePropCone SimplePropCone.h "SimplePropCone.h"
/// @brief 単純な PropCone
//////////////////////////////////////////////////////////////////////
class SimplePropCone :
  public PropCone
{
public:

  /// @brief コンストラクタ
  SimplePropCone(
    StructEnc& struct_enc,   ///< [in] StructEnc ソルバ
    const TpgNode* root_node ///< [in] FFRの根のノード
  );

  /// @brief デストラクタ
  ~SimplePropCone();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 関係するノードの変数を作る．
  void
  make_vars(
    const vector<const TpgNode*>& node_list
  ) override;

  /// @brief 関係するノードの入出力の関係を表すCNFを作る．
  void
  make_cnf() override;

  /// @brief 故障の影響伝搬させる条件を作る．
  vector<SatLiteral>
  make_condition(
    const TpgNode* root ///< [in] 起点となるノード
  ) override;

};

END_NAMESPACE_DRUID

#endif // SIMPLEPROPCONE_H
