#ifndef PYDTPGFFR_H
#define PYDTPGFFR_H

/// @file PyDtpgFFR.h
/// @brief PyDtpgFFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "DtpgFFR.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyDtpgFFR PyDtpgFFR.h "PyDtpgFFR.h"
/// @brief Python 用の DtpgFFR 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyDtpgFFR
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

  /// @brief PyObject が DtpgFFR タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief DtpgFFR を表す PyObject から DtpgFFR を取り出す．
  /// @return DtpgFFR を返す．
  ///
  /// _check(obj) == true であると仮定している．
  static
  DtpgFFR&
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief DtpgFFR を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYDTPGFFR_H
