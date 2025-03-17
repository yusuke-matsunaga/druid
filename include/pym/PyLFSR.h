#ifndef PYLFSR_H
#define PYLFSR_H

/// @file PyLFSR.h
/// @brief PyLFSR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "LFSR.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyLFSR PyLFSR.h "PyLFSR.h"
/// @brief Python 用の LFSR 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyLFSR
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

  /// @brief LFSR を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const LFSR& val ///< [in] 値
  );

  /// @brief PyObject が LFSR タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief LFSR を表す PyObject から LFSR を取り出す．
  /// @return LFSR を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const LFSR&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief LFSR を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYLFSR_H
