#ifndef NODEVAL_H
#define NODEVAL_H

/// @file NodeVal.h
/// @brief NodeVal のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NodeVal NodeVal.h "td/NodeVal.h"
/// @brief ノードに対する値の割当を表すクラス
///
/// 昔の C でよく使われていたポインタの下位ビットが0であることを
/// 利用して，そこにフラグを埋め込むテクニック
/// C++ の時代では醜いことこのうえない．
///
/// なお，縮退故障モードのときは時刻は 1 となる．
//////////////////////////////////////////////////////////////////////
class NodeVal
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 内容は不定
  NodeVal(
  ) : mPackVal{0UL}
  {
  }

  /// @brief 値を指定したコンストラクタ
  NodeVal(
    const TpgNode* node, ///< [in] ノード
    int time,		 ///< [in] 時刻 ( 0 or 1 )
    bool val		 ///< [in] 値
  ) : mPackVal{reinterpret_cast<ympuint>(node) | (time << 1) | val}
  {
  }

  /// @brief コピーコンストラクタ
  NodeVal(const NodeVal& src) = default;

  /// @brief コピー代入演算子
  NodeVal&
  operator=(const NodeVal& src) = default;


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
  int
  time() const
  {
    return static_cast<int>((mPackVal >> 1) & 1U);
  }

  /// @brief ノードと時刻をパックした値を返す．
  ///
  /// 結果は等価比較のみに用いる．
  ympuint
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

  /// @brief 大小関係の比較関数
  friend
  bool
  operator<(
    const NodeVal& left,
    const NodeVal& right
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パックした値
  ympuint mPackVal;

};

/// @brief 割当の内容を出力する．
/// @return s を返す．
ostream&
operator<<(
  ostream& s, ///< [in] 出力先のストリーム
  NodeVal nv  ///< [in] 値の割り当て
);


// @brief 大小関係の比較関数
inline
bool
operator>(
  const NodeVal& left,
  const NodeVal& right
)
{
  return operator<(right, left);
}

// @brief 大小関係の比較関数
inline
bool
operator<=(
  const NodeVal& left,
  const NodeVal& right
)
{
  return !operator<(right, left);
}

// @brief 大小関係の比較関数
inline
bool
operator>=(
  const NodeVal& left,
  const NodeVal& right
)
{
  return !operator<(left, right);
}

END_NAMESPACE_DRUID

#endif // NODEVAL_H
