#ifndef PYDTPGENGINE_H
#define PYDTPGENGINE_H

/// @file PyDtpgEngine.h
/// @brief PyDtpgEngine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ym/PyBase.h"
#include "DtpgEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyDtpgEngine PyDtpgEngine.h "PyDtpgEngine.h"
/// @brief Python 用の DtpgEngine 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyDtpgEngine :
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

  /// @brief DtpgEngine を表す PyObject から DtpgEngine を取り出す．
  /// @return 変換が成功したら true を返す．
  ///
  /// エラーの場合にはPython例外がセットされる．
  static
  bool
  FromPyObject(
    PyObject* obj,  ///< [in] DtpgEngine を表す PyObject
    DtpgEngine& val ///< [out] 変換された DtpgEngine を格納する変数
  );

  /// @brief PyObject が DtpgEngine タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief DtpgEngine を表す PyObject から DtpgEngine を取り出す．
  /// @return DtpgEngine を返す．
  ///
  /// _check(obj) == true であると仮定している．
  static
  DtpgEngine
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief DtpgEngine を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYDTPGENGINE_H
