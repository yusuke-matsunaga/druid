﻿#ifndef MFFCCONE_H
#define MFFCCONE_H

/// @file MffcCone.h
/// @brief MffcCone のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "FoCone.h"
#include "ym/HashMap.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MffcCone MffcCone.h "MffcCone.h"
/// @brief MFFC内の故障をひとまとめに扱うためのクラス
//////////////////////////////////////////////////////////////////////
class MffcCone :
  public FoCone
{
public:

  /// @brief コンストラクタ
  /// @param[in] struct_sat StructSat ソルバ
  /// @param[in] mffc MFFC の情報
  /// @param[in] block_node ブロックノード
  /// @param[in] detect 故障を検出する時に true にするフラグ
  ///
  /// ブロックノードより先のノードは含めeない．
  /// 通常 block_node は nullptr か root_node の dominator
  /// となっているはず．
  MffcCone(StructSat& struct_sat,
	   const TpgMFFC* mffc,
	   const TpgNode* block_node,
	   bool detect);

  /// @brief デストラクタ
  ~MffcCone();


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

  /// @brief 故障検出に必要な割り当てを求める．
  /// @param[in] model SAT のモデル
  /// @param[in] root 起点のノード
  /// @param[out] 値の割り当て結果を入れるリスト
  virtual
  void
  extract(const vector<SatBool3>& model,
	  const TpgNode* root,
	  NodeValList& assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief node に関する故障伝搬条件を作る．
  /// @param[in] node 対象のノード
  void
  make_dchain_cnf(const TpgNode* node);

  /// @brief 故障挿入回路のCNFを作る．
  /// @param[in] elem_pos 要素番号
  /// @param[in] ovar ゲートの出力の変数
  void
  inject_fault(ymuint elem_pos,
	       SatVarId ovar);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFR の根のリスト
  // [0] は MFFC の根でもある．
  vector<const TpgNode*> mElemArray;

  // 各FFRの根に反転イベントを挿入するための変数
  // サイズは mElemArray.size()
  vector<SatVarId> mElemVarArray;

  // ノード番号をキーにしてFFR番号を入れる連想配列
  HashMap<ymuint, ymuint> mElemPosMap;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_YM_SATPG

#endif // MFFCCONE_H
