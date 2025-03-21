#ifndef PYFSIM_H
#define PYFSIM_H

/// @file PyFsim.h
/// @brief PyFsim のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "Fsim.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyFsim PyFsim.h "PyFsim.h"
/// @brief Python 用の Fsim 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyFsim
{
  // このクラスは Conv/Deconv を持たない．

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

  /// @brief PyObject が Fsim タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief Fsim を表す PyObject から Fsim を取り出す．
  /// @return Fsim を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  Fsim&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief Fsim を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYFSIM_H
