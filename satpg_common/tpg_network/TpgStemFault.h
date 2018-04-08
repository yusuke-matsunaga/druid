﻿#ifndef TPGSTEMFAULT_H
#define TPGSTEMFAULT_H

/// @file TpgStemFault.h
/// @brief TpgStemFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2007, 2012-2014, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFaultBase.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgStemFault TpgStemFault.h "TpgStemFault.h"
/// @brief 出力の故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgStemFault :
  public TpgFaultBase
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] val 故障値
  /// @param[in] node 故障位置のノード
  /// @param[in] name 故障位置のノード名
  /// @param[in] rep_fault 代表故障
  TpgStemFault(int id,
	       int val,
	       const TpgNode* node,
	       const char* name,
	       TpgFault* rep_fault);

  /// @brief デストラクタ
  virtual
  ~TpgStemFault();


public:
  //////////////////////////////////////////////////////////////////////
  // read-only のメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の入力側の TpgNode を返す．
  virtual
  const TpgNode*
  tpg_inode() const override;

  /// @brief 故障の出力側の TpgNode を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  virtual
  const TpgNode*
  tpg_onode() const override;

  /// @brief ステムの故障の時 true を返す．
  virtual
  bool
  is_stem_fault() const override;

  /// @brief ブランチの入力位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  virtual
  int
  fault_pos() const override;

  /// @brief tpg_onode 上の故障位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  virtual
  int
  tpg_pos() const override;

  /// @brief 故障の内容を表す文字列を返す．
  virtual
  string
  str() const override;

};

END_NAMESPACE_YM_SATPG

#endif // TPGSTEMFAULT_H
