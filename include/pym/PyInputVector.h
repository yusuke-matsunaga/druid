#ifndef PYINPUTVECTOR_H
#define PYINPUTVECTOR_H

/// @file PyInputVector.h
/// @brief PyInputVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "InputVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyInputVectorConv PyInputVector.h "PyInputVector.h"
/// @brief const InputVector* を PyObject* に変換するファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyInputVectorConv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief const InputVector& を PyObject* に変換する．
  PyObject*
  operator()(
    const InputVector& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyInputVectorDeconv PyInputVector.h "PyInputVector.h"
/// @brief InputVector を取り出すファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyInputVectorDeconv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief PyObject* から InputVector を取り出す．
  bool
  operator()(
    PyObject* obj,
    InputVector& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyInputVector PyInputVector.h "PyInputVector.h"
/// @brief Python 用の InputVector 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyInputVector
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

  /// @brief InputVector を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const InputVector& val ///< [in] 値
  )
  {
    PyInputVectorConv conv;
    return conv(val);
  }

  /// @brief PyObject が InputVector タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief InputVector を表す PyObject から InputVector を取り出す．
  /// @return InputVector を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const InputVector&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief InputVector を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYINPUTVECTOR_H
