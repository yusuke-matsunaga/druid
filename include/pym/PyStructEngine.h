#ifndef PYSTRUCTENGINE_H
#define PYSTRUCTENGINE_H

/// @file PyStructEngine.h
/// @brief PyStructEngine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "StructEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyStructEngine PyStructEngine.h "PyStructEngine.h"
/// @brief Python 用の StructEngine 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyStructEngine
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

  /// @brief PyObject が StructEngine タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief StructEngine を表す PyObject から StructEngine を取り出す．
  /// @return StructEngine を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  StructEngine&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief StructEngine を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYSTRUCTENGINE_H
