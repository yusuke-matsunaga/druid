#ifndef PYFAULTTYPE_H
#define PYFAULTTYPE_H

/// @file PyFaultType.h
/// @brief PyFaultType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ym/PyBase.h"
#include "FaultType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyFaultType PyFaultType.h "PyFaultType.h"
/// @brief Python 用の FaultType 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyFaultType :
  public PyBase
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

  /// @brief FaultType を表す PyObject から FaultType を取り出す．
  /// @return 変換が成功したら true を返す．
  ///
  /// エラーの場合にはPython例外がセットされる．
  static
  bool
  FromPyObject(
    PyObject* obj,  ///< [in] FaultType を表す PyObject
    FaultType& val ///< [out] 変換された FaultType を格納する変数
  );

  /// @brief FaultType を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    FaultType val ///< [in] 値
  );

  /// @brief PyObject が FaultType タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief FaultType を表す PyObject から FaultType を取り出す．
  /// @return FaultType を返す．
  ///
  /// _check(obj) == true であると仮定している．
  static
  FaultType
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief FaultType を表す PyObject に値を設定する．
  ///
  /// _check(obj) == true であると仮定している．
  static
  void
  _put(
    PyObject* obj, ///< [in] 対象の PyObject
    FaultType val ///< [in] 設定する値
  );

  /// @brief FaultType を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYFAULTTYPE_H
