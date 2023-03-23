﻿#ifndef TPGBRANCHFAULT_H
#define TPGBRANCHFAULT_H

/// @file TpgBranchFault.h
/// @brief TpgBranchFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2007, 2012-2014, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#error "obsolete"
#include "TpgFaultBase.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgBranchFault TpgBranchFault.h "TpgBranchFault.h"
/// @brief 入力の故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgBranchFault :
  public TpgFaultBase
{
public:

  /// @brief コンストラクタ
  TpgBranchFault(
    SizeType id,          ///< [in] ID番号
    Fval2 val,            ///< [in] 故障値(0/1)
    const TpgNode* onode, ///< [in] 出力側の TpgNode
    const string& name,	  ///< [in] ノード名
    SizeType pos,	  ///< [in] 故障の入力位置
    const TpgNode* inode, ///< [in] 入力側の TpgNode
    SizeType tpg_pos,	  ///< [in] onode 上の故障位置
    TpgFault* rep_fault	  ///< [in] 代表故障
  );

  /// @brief デストラクタ
  ~TpgBranchFault();


public:
  //////////////////////////////////////////////////////////////////////
  // read-only のメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の入力側の TpgNode を返す．
  const TpgNode*
  tpg_inode() const override;

  /// @brief 故障の出力側の TpgNode を返す．
  const TpgNode*
  tpg_onode() const override;

#if 0
  /// @brief ステムの故障の時 true を返す．
  bool
  is_stem_fault() const override;

  /// @brief ブランチの入力位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  SizeType
  fault_pos() const override;

  /// @brief tpg_inode 上の故障位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  SizeType
  tpg_pos() const override;
#endif

  /// @brief 故障伝搬の起点となるノードを返す．
  const TpgNode*
  origin_node() const override;

  /// @brief 故障が励起してノードの出力まで伝搬する条件を求める．
  NodeValList
  node_propagate_condition(
    FaultType fault_type   ///< [in] 故障の種類
  ) const override;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障の入力位置
  // もとの BnNode に対するもの
  SizeType mPos;

  // 入力側の TpgNode
  const TpgNode* mInode;

  // mInode 上の入力位置
  SizeType mTpgPos;

};

END_NAMESPACE_DRUID

#endif // TPGBRANCHFAULT_H
