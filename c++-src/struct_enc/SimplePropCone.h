#ifndef SIMPLEPROPCONE_H
#define SIMPLEPROPCONE_H

/// @file SimplePropCone.h
/// @brief SimplePropCone のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
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
  /// @param[in] struct_sat StructEnc ソルバ
  /// @param[in] root_node FFRの根のノード
  /// @param[in] block_node ブロックノード
  /// @param[in] detect 故障を検出する時に true にするフラグ
  ///
  /// ブロックノードより先のノードは含めない．
  /// 通常 block_node は nullptr か root_node の dominator
  /// となっているはず．
  SimplePropCone(StructEnc& struct_sat,
		 const TpgNode* root_node,
		 const TpgNode* block_node,
		 bool detect);

  /// @brief デストラクタ
  virtual
  ~SimplePropCone();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 関係するノードの変数を作る．
  virtual
  void
  make_vars();

  /// @brief 関係するノードの入出力の関係を表すCNFを作る．
  virtual
  void
  make_cnf();

  /// @brief 故障の影響伝搬させる条件を作る．
  /// @param[in] root 起点となるノード
  /// @param[out] assumptions 結果の仮定を表すリテラルのリスト
  virtual
  void
  make_prop_condition(const TpgNode* root,
		      vector<SatLiteral>& assumptions);


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_DRUID_STRUCTENC

#endif // SIMPLEPROPCONE_H
