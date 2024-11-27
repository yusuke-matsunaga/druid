#ifndef PYTESTCOND_H
#define PYTESTCOND_H

/// @file PyTestCond.h
/// @brief PyTestCond のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "TestCond.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyTestCond PyTestCond.h "PyTestCond.h"
/// @brief Python 用の TestCond 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTestCond
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

  /// @brief TestCond を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const TestCond& val ///< [in] 値
  );

  /// @brief PyObject が TestCond タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief TestCond を表す PyObject から TestCond を取り出す．
  /// @return TestCond を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  TestCond
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TestCond を表す PyObject から TestCond を取り出す．
  /// @return TestCond を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const TestCond&
  _Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TestCond を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTESTCOND_H
