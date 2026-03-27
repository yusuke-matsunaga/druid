#ifndef PGNODE_H
#define PGNODE_H

/// @file PgNode.h
/// @brief PgNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PgNode PgNode.h "PgNode.h"
/// @brief PropGraph のノードを表すクラス
///
/// このクラスは故障伝搬に関する値を持つ．
//////////////////////////////////////////////////////////////////////
class PgNode
{
public:

  /// @brief コンストラクタ
  PgNode(
    SizeType id,      ///< [in] ノード番号
    bool gval,        ///< [in] 正常値
    bool fval         ///< [in] 故障値
  ) : mId{id}
  {
    set_gval(gval);
    set_fval(fval);
    set_fcone();
  }

  /// @brief コンストラクタ
  PgNode(
    SizeType id,      ///< [in] ノード番号
    bool gval         ///< [in] 正常値
  ) : mId{id}
  {
    set_gval(gval);
    set_fval(gval);
  }

  /// @brief デストラクタ
  ~PgNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief SATの解の値を返す．
  bool
  val(
    bool gf ///< [in] 正常値の時 true，故障値の時 false
  ) const
  {
    if ( gf ) {
      return gval();
    }
    else {
      return fval();
    }
  }

  /// @brief SATの解の正常値を返す．
  bool
  gval() const
  {
    return mFlags[GVAL];
  }

  /// @brief SATの解の故障値を返す．
  bool
  fval() const
  {
    return mFlags[FVAL];
  }

  /// @brief SATの解の値を返す(Val3版)．
  Val3
  val3(
    bool gf ///< [in] 正常値の時 true，故障値の時 false
  ) const
  {
    return bool_to_val3(val(gf));
  }

  /// @brief SATの解の正常値を返す(Val3版)．
  Val3
  gval3() const
  {
    return bool_to_val3(gval());
  }

  /// @brief SATの解の故障値を返す(Val3版)．
  Val3
  fval3() const
  {
    return bool_to_val3(fval());
  }

  /// @brief 故障の起点ノードの TFO に含まれる時に true を返す．
  bool
  is_in_fcone() const
  {
    return mFlags[FCONE];
  }

  /// @brief 事前割り当てによって値が確定している時に true を返す．
  bool
  is_val_fixed(
    bool gf ///< [in] 正常値の時 true，故障値の時 false
  ) const
  {
    if ( gf ) {
      return is_gval_fixed();
    }
    else {
      return is_fval_fixed();
    }
  }

  /// @brief 事前割り当てによって正常値が確定している時に true を返す．
  bool
  is_gval_fixed() const
  {
    return mFlags[GVALFIXED];
  }

  /// @brief 事前割り当てによって故障値が確定している時に true を返す．
  bool
  is_fval_fixed() const
  {
    return mFlags[FVALFIXED];
  }

  /// @brief SATの解の正常値をセットする．
  void
  set_gval(
    bool val ///< [in] 設定する値
  )
  {
    mFlags[GVAL] = val;
  }

  /// @brief SATの解の故障値をセットする．
  void
  set_fval(
    bool val ///< [in] 設定する値
  )
  {
    mFlags[FVAL] = val;
  }

  /// @brief fcone マークを付ける．
  void
  set_fcone()
  {
    mFlags.set(FCONE);
  }

  /// @brief 正常値の fixed マークを付ける．
  void
  set_gval_fixed()
  {
    mFlags.set(GVALFIXED);
  }

  /// @brief 故障値の fixed マークを付ける．
  void
  set_fval_fixed()
  {
    mFlags.set(FVALFIXED);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief bool値を Val3 に変換する．
  static
  Val3
  bool_to_val3(
    bool bval
  )
  {
    if ( bval ) {
      return Val3::_1;
    }
    else {
      return Val3::_0;
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SAT問題の解の正常値
  static
  const int GVAL = 0;

  // SAT問題の解の故障値
  static
  const int FVAL = 1;

  // FCONE
  static
  const int FCONE = 2;

  // 正常値確定
  static
  const int GVALFIXED = 3;

  // 故障値確定
  static
  const int FVALFIXED = 4;

  // 総ビット数
  static
  const int NBITS = 5;

  // ノード番号
  SizeType mId;

  // 種々の値を保持するフラグ
  std::bitset<NBITS> mFlags{0};

};

END_NAMESPACE_DRUID

#endif // PGNODE_H
