#ifndef TPGFAULT_H
#define TPGFAULT_H

/// @file TpgFault.h
/// @brief TpgFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
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
  TpgFault(const TpgFault& src) = delete;

  /// @brief コピー代入演算子も禁止
  TpgFault&
  operator=(const TpgFault& src) = delete;

  /// @brief ムーブコンストラクタは禁止
  TpgFault(TpgFault&& src) = delete;

  /// @brief ムーブ代入演算子も禁止
  TpgFault&
  operator=(TpgFault&& src) = delete;

  /// @brief デストラクタ
  virtual
  ~TpgFault() { }


public:
  //////////////////////////////////////////////////////////////////////
  // read-only のメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  virtual
  int
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
  is_branch_fault() const;

  /// @brief ブランチの入力位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  virtual
  int
  fault_pos() const = 0;

  /// @brief tpg_onode 上の故障位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  /// tpg_onode()->fanin(tpg_pos()) == tpg_inode() が成り立つ．
  virtual
  int
  tpg_pos() const = 0;

  /// @brief 故障値を返す．
  /// @note 返す値は 0 か 1
  virtual
  int
  val() const = 0;

  /// @brief 故障値を3値型で返す．
  Val3
  val3() const;

  /// @brief 故障の内容を表す文字列を返す．
  virtual
  string
  str() const = 0;

  /// @brief 代表故障の時 true を返す．
  bool
  is_rep() const;

  /// @brief 代表故障を返す．
  ///
  /// 代表故障の時は自分自身を返す．
  virtual
  const TpgFault*
  rep_fault() const = 0;

};

/// @relates TpgFault
/// @brief 故障が励起してFFRの根まで伝搬する条件を求める．
/// @param[in] fault 故障
/// @param[in] fault_type 故障の種類
NodeValList
ffr_propagate_condition(const TpgFault* fault,
			FaultType fault_type);

/// @relates TpgFault
/// @brief ストリーム出力演算子
/// @param[in] s 出力先のストリーム
/// @param[in] f 故障
ostream&
operator<<(ostream& s,
	   const TpgFault* f);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ブランチの故障の時 true を返す．
inline
bool
TpgFault::is_branch_fault() const
{
  return !is_stem_fault();
}

// @brief 代表故障の時 true を返す．
inline
bool
TpgFault::is_rep() const
{
  return rep_fault() == this;
}

// @brief 故障値を3値型で返す．
inline
Val3
TpgFault::val3() const
{
  if ( val() ) {
    return Val3::_1;
  }
  else {
    return Val3::_0;
  }
}

// @brief ストリーム出力演算子
// @param[in] s 出力先のストリーム
// @param[in] f 故障
inline
ostream&
operator<<(ostream& s,
	   const TpgFault* f)
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
