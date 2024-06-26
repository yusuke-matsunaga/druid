#ifndef PYDTPGSTATS_H
#define PYDTPGSTATS_H

/// @file PyDtpgStats.h
/// @brief PyDtpgStats のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "DtpgStats.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyDtpgStats PyDtpgStats.h "PyDtpgStats.h"
/// @brief Python 用の DtpgStats 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyDtpgStats
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

  /// @brief DtpgStats を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const DtpgStats& val  ///< [in] 値
  );

  /// @brief PyObject が DtpgStats タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief DtpgStats を表す PyObject から DtpgStats を取り出す．
  /// @return DtpgStats を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const DtpgStats&
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief DtpgStats を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYDTPGSTATS_H
