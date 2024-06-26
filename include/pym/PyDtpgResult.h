#ifndef PYDTPGRESULT_H
#define PYDTPGRESULT_H

/// @file PyDtpgResult.h
/// @brief PyDtpgResult のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
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

  /// @brief DtpgResult を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const DtpgResult& val  ///< [in] 値
  );

  /// @brief PyObject が DtpgResult タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief DtpgResult を表す PyObject から DtpgResult を取り出す．
  /// @return DtpgResult を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const DtpgResult&
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief DtpgResult を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYDTPGRESULT_H
