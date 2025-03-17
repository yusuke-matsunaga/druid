#ifndef PYFAULTSTATUS_H
#define PYFAULTSTATUS_H

/// @file PyFaultStatus.h
/// @brief PyFaultStatus のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "FaultStatus.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyFaultStatus PyFaultStatus.h "PyFaultStatus.h"
/// @brief Python 用の FaultStatus 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyFaultStatus
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

  /// @brief FaultStatus を表す PyObject から FaultStatus を取り出す．
  /// @return 変換が成功したら true を返す．
  ///
  /// エラーの場合にはPython例外がセットされる．
  static
  bool
  FromPyObject(
    PyObject* obj,  ///< [in] FaultStatus を表す PyObject
    FaultStatus& val ///< [out] 変換された FaultStatus を格納する変数
  );

  /// @brief FaultStatus を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    FaultStatus val ///< [in] 値
  );

  /// @brief PyObject が FaultStatus タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief FaultStatus を表す PyObject から FaultStatus を取り出す．
  /// @return FaultStatus を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  FaultStatus
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief FaultStatus を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYFAULTSTATUS_H
