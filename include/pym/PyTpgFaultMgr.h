#ifndef PYTPGFAULTMGR_H
#define PYTPGFAULTMGR_H

/// @file PyTpgFaultMgr.h
/// @brief PyTpgFaultMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "TpgFaultMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyTpgFaultMgr PyTpgFaultMgr.h "PyTpgFaultMgr.h"
/// @brief Python 用の TpgFaultMgr 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTpgFaultMgr
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

  /// @brief PyObject が TpgFaultMgr タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief TpgFaultMgr を表す PyObject から TpgFaultMgr を取り出す．
  /// @return TpgFaultMgr を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  TpgFaultMgr&
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TpgFaultMgr を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTPGFAULTMGR_H
