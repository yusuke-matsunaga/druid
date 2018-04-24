#ifndef INPUTVECTOR_H
#define INPUTVECTOR_H

/// @file InputVector.h
/// @brief InputVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "BitVector.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class InputVector InputVector.h "InputVector.h"
/// @brief 入力用のビットベクタ
///
/// 実は BitVector の別名だが他のクラスと区別するために typedef は使わない．
//////////////////////////////////////////////////////////////////////
class InputVector :
  public BitVector
{
  friend class TestVector;

private:

  /// @brief コンストラクタ
  /// @param[in] vect_len ベクタ長
  explicit
  InputVector(int vect_len);

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のソース
  ///
  /// この関数は実装しない．
  InputVector(const InputVector& src) = delete;

  /// @brief ムーブコンストラクタ
  /// @param[in] src ムーブ元のソース
  ///
  /// この関数は実装しない．
  InputVector(InputVector&& src) = delete;

  /// @brief ムーブ代入演算子
  /// @param[in] src ムーブ元のソース
  ///
  /// この関数は実装しない．
  InputVector&
  operator=(InputVector&& src) = delete;

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
// @param[in] vect_len ベクタ長
inline
InputVector::InputVector(int vect_len) :
  BitVector(vect_len)
{
}

// @brief デストラクタ
inline
InputVector::~InputVector()
{
}

END_NAMESPACE_YM_SATPG

#endif // INPUTVECTOR_H
