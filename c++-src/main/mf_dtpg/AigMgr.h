#ifndef AIGMGR_H
#define AIGMGR_H

/// @file AigMgr.h
/// @brief AigMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "jester.h"
#include "ym/logic.h"


BEGIN_NAMESPACE_JESTER

class AigHandle;
class AigNode;

//////////////////////////////////////////////////////////////////////
/// @class AigMgr AigMgr.h "AigMgr.h"
/// @brief AigNode を管理するクラス
//////////////////////////////////////////////////////////////////////
class AigMgr
{
public:

  /// @brief コンストラクタ
  AigMgr();

  /// @brief デストラクタ
  ~AigMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 定数０のハンドルを返す．
  AigHandle
  make_zero();

  /// @brief 定数１のハンドルを返す．
  AigHandle
  make_one();

  /// @brief 入力ノードのハンドルを返す．
  AigHandle
  make_input();

  /// @brief ANDノードのハンドルを返す．
  /// @param[in] fanin_array ファンインのハンドルの配列
  AigHandle
  make_and(const vector<AigHandle>& fanin_array);

  /// @brief ORノードのハンドルを返す．
  /// @param[in] fanin_array ファンインのハンドルの配列
  AigHandle
  make_or(const vector<AigHandle>& fanin_array);

  /// @brief XORノードのハンドルを返す．
  /// @param[in] fanin_array ファンインのハンドルの配列
  AigHandle
  make_xor(const vector<AigHandle>& fanin_array);

  /// @brief EXPRノードのハンドルを返す．
  /// @param[in] expr 論理式
  /// @param[in] fanin_array ファンインのハンドルの配列
  AigHandle
  make_expr(const Expr& expr,
	    const vector<AigHandle>& fanin_array);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ANDノードのハンドルを返す．
  /// @param[in] fanin_array ファンインのハンドルの配列
  /// @param[in] fanin_inv ファンインを反転させる時 true にするフラグ
  AigHandle
  make_and_sub(const vector<AigHandle>& fanin_array,
	       bool fanin_inv);

  /// @brief make_xor() の下請け関数
  AigHandle
  make_xor_sub(const vector<AigHandle>& fanin_array,
	       int start,
	       int end);

  /// @brief ハッシュ表を拡大する．
  /// @param[in] req_size 要求サイズ
  void
  expand_table(int req_size);

  /// @brief ノードを登録する．
  void
  reg_node(AigNode* node);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノードのリスト
  vector<AigNode*> mNodeList;

  // ハッシュ表
  vector<AigNode*> mHashTable;

  // ハッシュ表を拡大する目安
  int mNextLimit;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_JESTER

#endif // AIGMGR_H
