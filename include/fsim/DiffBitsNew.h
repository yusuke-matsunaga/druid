#ifndef DIFFBITSNEW_H
#define DIFFBITSNEW_H

/// @file DiffBitsNew.h
/// @brief DiffBitsNew のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DiffBitsNew DiffBitsNew.h "DiffBitsNew.h"p
/// @brief 故障の影響が伝搬したかどうかを表すビットベクタ
///
/// 意味的には出力数ぶんのビットベクタだが実際にはほぼゼロなので
/// 工夫している．
//////////////////////////////////////////////////////////////////////
class DiffBitsNew
{
public:

  /// @brief 空のコンストラクタ
  DiffBitsNew() = default;

  /// @brief コンストラクタ
  ///
  /// 全て0で初期化される．
  explicit
  DiffBitsNew(
    SizeType size ///< [in] サイズ
  ) : mSize{size}
  {
  }

  /// @brief デストラクタ
  ~DiffBitsNew() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素数を返す．
  SizeType
  size() const
  {
    return mSize;
  }

  /// @brief 要素を取り出す．
  bool
  operator[](
    SizeType pos ///< [in] 位置 ( 0 <= pos < size() )
  ) const
  {
    // 非効率的な線形探索
    for ( auto p = mBody.begin(); p != mBody.end(); ) {
      auto elem = get(p);
      if ( elem == pos ) {
	return true;
      }
      if ( elem > pos ) {
	break;
      }
    }
    return false;
  }

  /// @brief 値を true にする．
  void
  set_val(
    SizeType pos ///< [in] 位置 ( 0 <= pos < size() )
  )
  {
    for ( ; ; ) {
      if ( pos <= 0x7F ) {
	mBody.push_back(static_cast<std::uint8_t>(pos));
	break;
      }
      std::uint8_t v = (pos & 0x7F) | 0x80;
      mBody.push_back(v);
      pos >>= 8;
    }
  }

  /// @brief true のビットがあるか調べる．
  bool
  any() const
  {
    return !mBody.empty();
  }

  /// @brief 等価比較演算
  bool
  operator==(
    const DiffBitsNew& right
  ) const
  {
    return mSize == right.mSize && mBody == right.mBody;
  }

  /// @brief 非等価比較演算
  bool
  operator!=(
    const DiffBitsNew& right
  ) const
  {
    return !operator==(right);
  }

  /// @brief 内容を出力する．
  void
  print(
    ostream& s
  ) const
  {
    SizeType start = 0;
    auto p = mBody.begin();
    while ( p != mBody.end() ) {
      auto end = get(p);
      for ( auto pos = start; pos < end; ++ pos ) {
	s << "0";
      }
      s << "1";
      start = end + 1;
    }
    for ( auto pos = start; pos < mSize; ++ pos ) {
      s << "0";
    }
  }

  /// @brief ハッシュ関数
  SizeType
  hash() const
  {
    SizeType N = sizeof(SizeType) * 8;
    SizeType ans = 0;
    for ( auto pos: mBody ) {
      ans *= pos;
    }
    return ans;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 次の要素を取り出す．
  SizeType
  get(
    vector<std::uint8_t>::const_iterator& p
  ) const
  {
    SizeType ans = 0;
    SizeType w = 0;
    for ( ; ; ) {
      auto v = *p;
      ans += (v & 0x7F) << w;
      ++ p;
      if ( (v & 0x80) == 0 ) {
	break;
      }
      w += 8;
    }
    return ans;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // サイズ
  SizeType mSize;

  // 本体
  vector<std::uint8_t> mBody;

};

/// @brief ストリーム出力演算子
inline
ostream&
operator<<(
  ostream& s,
  const DiffBitsNew& dbits
)
{
  dbits.print(s);
  return s;
}

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_STD

// DiffBitsNew をキーとしたハッシュ関数クラス
template <>
struct hash<DRUID_NAMESPACE::DiffBitsNew>
{
  SizeType
  operator()(
    const DRUID_NAMESPACE::DiffBitsNew& dbits
  ) const
  {
    return dbits.hash();
  }
};

END_NAMESPACE_STD

#endif // DIFFBITSNEW_H
