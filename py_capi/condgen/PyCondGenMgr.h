#ifndef PYCONDGENMGR_H
#define PYCONDGENMGR_H

/// @file PyCondGenMgr.h
/// @brief PyCondGenMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "CondGenMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyCondGenMgr PyCondGenMgr.h "PyCondGenMgr.h"
/// @brief Python 用の CondGenMgr 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyCondGenMgr
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

  /// @brief PyObject が CondGenMgr タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief CondGenMgr を表す PyObject から CondGenMgr を取り出す．
  /// @return CondGenMgr を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  CondGenMgr&
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief CondGenMgr を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYCONDGENMGR_H
