#ifndef PYDTPGMGR_H
#define PYDTPGMGR_H

/// @file PyDtpgMgr.h
/// @brief PyDtpgMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "DtpgMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyDtpgMgr PyDtpgMgr.h "PyDtpgMgr.h"
/// @brief Python 用の DtpgMgr 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyDtpgMgr
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

  /// @brief PyObject が DtpgMgr タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief DtpgMgr を表す PyObject から DtpgMgr を取り出す．
  /// @return DtpgMgr を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  DtpgMgr&
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief DtpgMgr を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYDTPGMGR_H
