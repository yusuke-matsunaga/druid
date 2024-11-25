#ifndef ASSIGN_H
#define ASSIGN_H

/// @file Assign.h
/// @brief Assign のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Assign Assign.h "td/Assign.h"
/// @brief ノードに対する値の割当を表すクラス
///
/// 昔の C でよく使われていたポインタの下位ビットが0であることを
/// 利用して，そこにフラグを埋め込むテクニック
/// C++ の時代では醜いことこのうえない．
///
/// なお，縮退故障モードのときは時刻は 1 となる．
//////////////////////////////////////////////////////////////////////
class Assign
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 内容は不定
  Assign(
  ) : mPackVal{0UL}
  {
  }

  /// @brief 値を指定したコンストラクタ
  Assign(
    const TpgNode* node, ///< [in] ノード
    int time,		 ///< [in] 時刻 ( 0 or 1 )
    bool val		 ///< [in] 値
  ) : mPackVal{reinterpret_cast<PtrIntType>(node) | (time << 1) | val}
  {
  }

  /// @brief コピーコンストラクタ
  Assign(const Assign& src) = default;

  /// @brief コピー代入演算子
  Assign&
  operator=(const Assign& src) = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードを返す．
  const TpgNode*
  node() const
  {
    return reinterpret_cast<const TpgNode*>(mPackVal & ~3UL);
  }

  /// @brief 時刻を返す．
  ///
  /// 返される値は 0 か 1
  int
  time() const
  {
    return static_cast<int>((mPackVal >> 1) & 1U);
  }

  /// @brief ノードと時刻をパックした値を返す．
  ///
  /// 結果は等価比較のみに用いる．
  PtrIntType
  node_time() const
  {
    return mPackVal & ~1UL;
  }

  /// @brief 値を返す．
  bool
  val() const
  {
    return static_cast<bool>(mPackVal & 1UL);
  }

  /// @brief 反対の割り当てを返す．
  Assign
  operator~() const
  {
    return Assign{mPackVal ^ 1UL};
  }

  /// @brief ハッシュ値を返す．
  SizeType
  hash() const
  {
    return static_cast<SizeType>(mPackVal);
  }

  /// @brief 等価関係の比較関数
  friend
  bool
  operator==(
    const Assign& left,
    const Assign& right
  )
  {
    return left.mPackVal == right.mPackVal;
  }

  /// @brief 大小関係の比較関数
  friend
  bool
  operator<(
    const Assign& left,
    const Assign& right
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を指定したコンストラクタ
  Assign(
    PtrIntType val
  ) : mPackVal{val}
  {
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パックした値
  PtrIntType mPackVal;

};

/// @brief 非等価比較演算子
inline
bool
operator!=(
  const Assign& left,
  const Assign& right
)
{
  return !operator==(left, right);
}

/// @brief 割当の内容を出力する．
/// @return s を返す．
ostream&
operator<<(
  ostream& s,    ///< [in] 出力先のストリーム
  Assign nv ///< [in] 値の割り当て
);

// @brief 大小関係の比較関数
inline
bool
operator>(
  const Assign& left,
  const Assign& right
)
{
  return operator<(right, left);
}

// @brief 大小関係の比較関数
inline
bool
operator<=(
  const Assign& left,
  const Assign& right
)
{
  return !operator<(right, left);
}

// @brief 大小関係の比較関数
inline
bool
operator>=(
  const Assign& left,
  const Assign& right
)
{
  return !operator<(left, right);
}

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_STD

// Assign をキーにしたハッシュ関数クラスの定義
template <>
struct hash<DRUID_NAMESPACE::Assign>
{
  SizeType
  operator()(
    const DRUID_NAMESPACE::Assign& nv
  ) const
  {
    return nv.hash();
  }
};

END_NAMESPACE_STD

#endif // ASSIGN_H
