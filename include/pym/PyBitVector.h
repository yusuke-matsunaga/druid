#ifndef PYBITVECTOR_H
#define PYBITVECTOR_H

/// @file PyBitVector.h
/// @brief PyBitVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "BitVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyBitVectorConv PyBitVector.h "PyBitVector.h"
/// @brief const BitVector* を PyObject* に変換するファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyBitVectorConv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief BitVector を PyObject* に変換する．
  PyObject*
  operator()(
    BitVector val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyBitVectorDeconv PyBitVector.h "PyBitVector.h"
/// @brief BitVector を取り出すファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyBitVectorDeconv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief PyObject* から BitVector を取り出す．
  bool
  operator()(
    PyObject* obj,
    BitVector& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyBitVector PyBitVector.h "PyBitVector.h"
/// @brief Python 用の BitVector 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyBitVector
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  /// @return 初期化が成功したら true を返す．
  static
  bool
  init(
    PyObject* m ///< [in] 親のモジュールを表す PyObject
  );

  /// @brief BitVector を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    BitVector val ///< [in] 値
  )
  {
    PyBitVectorConv conv;
    return conv(val);
  }

  /// @brief PyObject が BitVector タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief BitVector を表す PyObject から BitVector を取り出す．
  /// @return BitVector を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const BitVector&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief BitVector を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYBITVECTOR_H
