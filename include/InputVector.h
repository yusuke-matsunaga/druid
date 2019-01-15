#ifndef INPUTVECTOR_H
#define INPUTVECTOR_H

/// @file InputVector.h
/// @brief InputVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "BitVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class InputVector InputVector.h "InputVector.h"
/// @brief 入力用のビットベクタ
///
/// 実は BitVector の別名だが他のクラスと区別するために typedef は使わない．
//////////////////////////////////////////////////////////////////////
class InputVector :
  public BitVector
{
public:

  /// @brief コンストラクタ
  /// @param[in] len ベクタ長
  ///
  /// 内容は X で初期化される．
  explicit
  InputVector(int len = 0);

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のソース
  InputVector(const InputVector& src);

  /// @brief コピー代入演算子
  /// @param[in] src コピー元のソース
  InputVector&
  operator=(const InputVector& src);

  /// @brief デストラクタ
  ~InputVector();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] len ベクタ長
inline
InputVector::InputVector(int len) :
  BitVector(len)
{
}

// @brief コピーコンストラクタ
// @param[in] src コピー元のソース
inline
InputVector::InputVector(const InputVector& src) :
  BitVector(src)
{
}

// @brief コピー代入演算子
// @param[in] src コピー元のソース
inline
InputVector&
InputVector::operator=(const InputVector& src)
{
  BitVector::operator=(src);

  return *this;
}

// @brief デストラクタ
inline
InputVector::~InputVector()
{
}

END_NAMESPACE_DRUID

#endif // INPUTVECTOR_H
