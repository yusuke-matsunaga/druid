#ifndef PYDTPGRESULT_H
#define PYDTPGRESULT_H

/// @file PyDtpgResult.h
/// @brief PyDtpgResult のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "DtpgResult.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyDtpgResult PyDtpgResult.h "PyDtpgResult.h"
/// @brief Python 用の DtpgResult 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyDtpgResult
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

  /// @brief DtpgResult を表す PyObject から DtpgResult を取り出す．
  /// @return 変換が成功したら true を返す．
  ///
  /// エラーの場合にはPython例外がセットされる．
  static
  bool
  FromPyObject(
    PyObject* obj,  ///< [in] DtpgResult を表す PyObject
    DtpgResult& val ///< [out] 変換された DtpgResult を格納する変数
  );

  /// @brief DtpgResult を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const DtpgResult& val ///< [in] 値
  );

  /// @brief PyObject が DtpgResult タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief DtpgResult を表す PyObject から DtpgResult を取り出す．
  /// @return DtpgResult を返す．
  ///
  /// _check(obj) == true であると仮定している．
  static
  const DtpgResult&
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief DtpgResult を表す PyObject に値を設定する．
  ///
  /// _check(obj) == true であると仮定している．
  static
  void
  _put(
    PyObject* obj, ///< [in] 対象の PyObject
    const DtpgResult& val ///< [in] 設定する値
  );

  /// @brief DtpgResult を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYDTPGRESULT_H
