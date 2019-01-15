#ifndef TPGNODEFACTORY_H
#define TPGNODEFACTORY_H

/// @file TpgNodeFactory.h
/// @brief TpgNodeFactory のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"
#include "ym/Alloc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgNodeFactory TpgNodeFactory.h "TpgNodeFactory.h"
/// @brief TpgNode(の派生クラス)を作るファクトリクラス
///
/// このクラスの機能を TpgNetwork に組み込んでも良いが，これだけ切り離したほうが
/// 見通しがよくなるのと TpgNode 単体のテストプログラムが書きやすくなる．
/// また，TpgNetwork.cc 中で TpgNode の各継承クラスのヘッダファイルを
/// インクルードする必要がなくなる．
//////////////////////////////////////////////////////////////////////
class TpgNodeFactory
{
public:

  /// @brief コンストラクタ
  /// @param[in] alloc メモリアロケータ
  TpgNodeFactory(Alloc& alloc);

  /// @brief デストラクタ
  ~TpgNodeFactory();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] iid 入力番号
  /// @param[in] fanout_num ファンアウト数
  /// @return 作成したノードを返す．
  TpgNode*
  make_input(int id,
	     int iid,
	     int fanout_num);

  /// @brief 出力ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] oid 出力番号
  /// @param[in] inode 入力ノード
  /// @return 作成したノードを返す．
  TpgNode*
  make_output(int id,
	      int oid,
	      TpgNode* inode);

  /// @brief DFFの入力ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] oid 出力番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] inode 入力ノード
  /// @return 作成したノードを返す．
  TpgNode*
  make_dff_input(int id,
		 int oid,
		 const TpgDff* dff,
		 TpgNode* inode);

  /// @brief DFFの出力ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] iid 入力番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] fanout_num ファンアウト数
  /// @return 作成したノードを返す．
  TpgNode*
  make_dff_output(int id,
		  int iid,
		  const TpgDff* dff,
		  int fanout_num);

  /// @brief DFFのクロック端子を作る．
  /// @param[in] id ノード番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] inode 入力ノード
  /// @return 作成したノードを返す．
  TpgNode*
  make_dff_clock(int id,
		 const TpgDff* dff,
		 TpgNode* inode);

  /// @brief DFFのクリア端子を作る．
  /// @param[in] id ノード番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] inode 入力ノード
  /// @return 作成したノードを返す．
  TpgNode*
  make_dff_clear(int id,
		 const TpgDff* dff,
		 TpgNode* inode);

  /// @brief DFFのプリセット端子を作る．
  /// @param[in] id ノード番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] inode 入力ノード
  /// @return 作成したノードを返す．
  TpgNode*
  make_dff_preset(int id,
		  const TpgDff* dff,
		  TpgNode* inode);

  /// @brief 論理ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] gate_type ゲートタイプ
  /// @param[in] inode_list 入力ノードのリスト
  /// @param[in] fanout_num ファンアウト数
  /// @return 作成したノードを返す．
  TpgNode*
  make_logic(int id,
	     GateType gate_type,
	     const vector<TpgNode*>& inode_list,
	     int fanout_num);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // メモリアロケータ
  Alloc& mAlloc;

};

END_NAMESPACE_DRUID

#endif // TPGNODEFACTORY_H
