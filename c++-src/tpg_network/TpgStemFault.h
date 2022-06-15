#ifndef TPGSTEMFAULT_H
#define TPGSTEMFAULT_H

/// @file TpgStemFault.h
/// @brief TpgStemFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2007, 2012-2014, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFaultBase.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgStemFault TpgStemFault.h "TpgStemFault.h"
/// @brief 出力の故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgStemFault :
  public TpgFaultBase
{
public:

  /// @brief コンストラクタ
  TpgStemFault(
    int id,              ///< [in] ID番号
    int val,             ///< [in] 故障値
    const TpgNode* node, ///< [in] 故障位置のノード
    const string& name,  ///< [in] 故障位置のノード名
    TpgFault* rep_fault  ///< [in] 代表故障
  );

  /// @brief デストラクタ
  ~TpgStemFault();


public:
  //////////////////////////////////////////////////////////////////////
  // read-only のメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の入力側の TpgNode を返す．
  const TpgNode*
  tpg_inode() const override;

  /// @brief 故障の出力側の TpgNode を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  const TpgNode*
  tpg_onode() const override;

  /// @brief ステムの故障の時 true を返す．
  bool
  is_stem_fault() const override;

  /// @brief ブランチの入力位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  SizeType
  fault_pos() const override;

  /// @brief tpg_onode 上の故障位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  SizeType
  tpg_pos() const override;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const override;

};

END_NAMESPACE_DRUID

#endif // TPGSTEMFAULT_H
