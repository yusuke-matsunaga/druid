#ifndef PYTPGFAULTSTATUSMGR_H
#define PYTPGFAULTSTATUSMGR_H

/// @file PyTpgFaultStatusMgr.h
/// @brief PyTpgFaultStatusMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "TpgFaultStatusMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyTpgFaultStatusMgr PyTpgFaultStatusMgr.h "PyTpgFaultStatusMgr.h"
/// @brief Python 用の TpgFaultStatusMgr 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTpgFaultStatusMgr
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

  /// @brief PyObject が TpgFaultStatusMgr タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief TpgFaultStatusMgr を表す PyObject から TpgFaultStatusMgr を取り出す．
  /// @return TpgFaultStatusMgr を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  TpgFaultStatusMgr&
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TpgFaultStatusMgr を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTPGFAULTSTATUSMGR_H
