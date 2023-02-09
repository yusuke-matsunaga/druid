#ifndef PYDTPGMFFC_H
#define PYDTPGMFFC_H

/// @file PyDtpgMFFC.h
/// @brief PyDtpgMFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "DtpgMFFC.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyDtpgMFFC PyDtpgMFFC.h "PyDtpgMFFC.h"
/// @brief Python 用の DtpgMFFC 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyDtpgMFFC
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

  /// @brief PyObject が DtpgMFFC タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief DtpgMFFC を表す PyObject から DtpgMFFC を取り出す．
  /// @return DtpgMFFC を返す．
  ///
  /// _check(obj) == true であると仮定している．
  static
  DtpgMFFC&
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief DtpgMFFC を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYDTPGMFFC_H
