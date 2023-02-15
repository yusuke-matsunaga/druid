#ifndef TPGFAULT_H
#define TPGFAULT_H

/// @file TpgFault.h
/// @brief TpgFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Fval2.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgFault TpgFault.h "TpgFault.h"
/// @brief 単一縮退故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgFault
{
public:

  /// @brief 空のコンストラクタ
  TpgFault() = default;

  /// @brief コピーコンストラクタは禁止
  TpgFault(
    const TpgFault& src
  ) = delete;

  /// @brief コピー代入演算子も禁止
  TpgFault&
  operator=(
    const TpgFault& src
  ) = delete;

  /// @brief ムーブコンストラクタは禁止
  TpgFault(
    TpgFault&& src
  ) = delete;

  /// @brief ムーブ代入演算子も禁止
  TpgFault&
  operator=(
    TpgFault&& src
  ) = delete;

  /// @brief デストラクタ
  virtual
  ~TpgFault() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // read-only のメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  virtual
  SizeType
  id() const = 0;

  /// @brief 故障の入力側の TpgNode を返す．
  virtual
  const TpgNode*
  tpg_inode() const = 0;

  /// @brief 故障の出力側の TpgNode を返す．
  ///
  /// is_stem_fault() == true の時は tpg_inode() と同じになる．
  virtual
  const TpgNode*
  tpg_onode() const = 0;

  /// @brief ステムの故障の時 true を返す．
  virtual
  bool
  is_stem_fault() const = 0;

  /// @brief ブランチの故障の時 true を返す．
  bool
  is_branch_fault() const
  {
    return !is_stem_fault();
  }

  /// @brief ブランチの入力位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  virtual
  SizeType
  fault_pos() const = 0;

  /// @brief tpg_onode 上の故障位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  /// tpg_onode()->fanin(tpg_pos()) == tpg_inode() が成り立つ．
  virtual
  SizeType
  tpg_pos() const = 0;

  /// @brief 故障値を返す．
  virtual
  Fval2
  val() const = 0;

  /// @brief 故障値を3値型で返す．
  Val3
  val3() const
  {
    switch ( val() ) {
    case Fval2::zero: return Val3::_0;
    case Fval2::one:  return Val3::_1;
    }
    ASSERT_NOT_REACHED;
    return Val3::_0;
  }

  /// @brief 故障が励起してノードの出力まで伝搬する条件を求める．
  NodeValList
  node_propagate_condition(
    FaultType fault_type   ///< [in] 故障の種類
  ) const;

  /// @brief 故障が励起してFFRの根まで伝搬する条件を求める．
  NodeValList
  ffr_propagate_condition(
    FaultType fault_type   ///< [in] 故障の種類
  ) const;

  /// @brief 故障の内容を表す文字列を返す．
  virtual
  string
  str() const = 0;

  /// @brief 代表故障の時 true を返す．
  bool
  is_rep() const
  {
    return rep_fault() == this;
  }

  /// @brief 代表故障を返す．
  ///
  /// 代表故障の時は自分自身を返す．
  virtual
  const TpgFault*
  rep_fault() const = 0;

};

/// @relates TpgFault
/// @brief ストリーム出力演算子
inline
ostream&
operator<<(
  ostream& s,       ///< [in] 出力先のストリーム
  const TpgFault* f ///< [in] 故障
)
{
  return s << f->str();
}

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_STD

// TpgFault へのポインタをキーにしたハッシュ関数クラスの定義
template <>
struct hash<DRUID_NAMESPACE::TpgFault*>
{
  SizeType
  operator()(
    DRUID_NAMESPACE::TpgFault* fault
  ) const
  {
    return fault->id();
  }
};

END_NAMESPACE_STD

#endif // TPGFAULT_H
