#ifndef PYVAL3_H
#define PYVAL3_H

/// @file PyVal3.h
/// @brief PyVal3 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyVal3 PyVal3.h "PyVal3.h"
/// @brief Python 用の Val3 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyVal3
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

  /// @brief Val3 を表す PyObject から Val3 を取り出す．
  /// @return 変換が成功したら true を返す．
  ///
  /// エラーの場合にはPython例外がセットされる．
  static
  bool
  FromPyObject(
    PyObject* obj,  ///< [in] Val3 を表す PyObject
    Val3& val ///< [out] 変換された Val3 を格納する変数
  );

  /// @brief Val3 を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    Val3 val ///< [in] 値
  );

  /// @brief PyObject が Val3 タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief Val3 を表す PyObject から Val3 を取り出す．
  /// @return Val3 を返す．
  ///
  /// _check(obj) == true であると仮定している．
  static
  Val3
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief Val3 を表す PyObject に値を設定する．
  ///
  /// _check(obj) == true であると仮定している．
  static
  void
  _put(
    PyObject* obj, ///< [in] 対象の PyObject
    Val3 val ///< [in] 設定する値
  );

  /// @brief Val3 を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYVAL3_H
