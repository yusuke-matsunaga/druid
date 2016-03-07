﻿#ifndef TPGOUTPUTFAULT_H
#define TPGOUTPUTFAULT_H

/// @file TpgOutputFault.h
/// @brief TpgOutputFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2007, 2012-2014 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFault.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgOutputFault TpgOutputFault.h "TpgOutputFault.h"
/// @brief 出力の故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgOutputFault :
  public TpgFault
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] bnnode 故障位置の BnNode
  /// @param[in] tpgnode 故障位置の TpgNode
  /// @param[in] val 故障値
  /// @param[in] rep_fault 代表故障
  TpgOutputFault(ymuint id,
		 const BnNode* bnnode,
		 const TpgNode* tpgnode,
		 int val,
		 const TpgFault* rep_fault);

  /// @brief デストラクタ
  virtual
  ~TpgOutputFault();


public:
  //////////////////////////////////////////////////////////////////////
  // read-only のメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障位置のゲートを返す．
  virtual
  const BnNode*
  node() const;

  /// @brief node() に対応する TpgNode を返す．
  virtual
  const TpgNode*
  tpg_node() const;

  /// @brief 故障の入力側の TpgNode を返す．
  ///
  /// 出力の故障の場合には tpg_node() と同じ値を返す．
  virtual
  const TpgNode*
  tpg_inode() const;

  /// @brief 出力の故障の時 true を返す．
  virtual
  bool
  is_output_fault() const;

  /// @brief 故障位置を返す．
  ///
  /// is_input_fault() == true の時のみ意味を持つ．
  virtual
  ymuint
  pos() const;

  /// @brief tpg_inode 上の故障位置を返す．
  ///
  /// is_input_fault() == true の時のみ意味を持つ．
  virtual
  ymuint
  tpg_pos() const;

  /// @brief 故障の内容を表す文字列を返す．
  virtual
  string
  str() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の BnNode
  const BnNode* mBnNode;

  // 対象の TpgNode
  const TpgNode* mTpgNode;

};

END_NAMESPACE_YM_SATPG

#endif // TPGOUTPUTFAULT_H
