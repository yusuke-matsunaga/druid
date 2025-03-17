#ifndef PYPHASESHIFTER_H
#define PYPHASESHIFTER_H

/// @file PyPhaseShifter.h
/// @brief PyPhaseShifter のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "PhaseShifter.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyPhaseShifter PyPhaseShifter.h "PyPhaseShifter.h"
/// @brief Python 用の PhaseShifter 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyPhaseShifter
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

  /// @brief PhaseShifter を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const PhaseShifter& val ///< [in] 値
  );

  /// @brief PyObject が PhaseShifter タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief PhaseShifter を表す PyObject から PhaseShifter を取り出す．
  /// @return PhaseShifter を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const PhaseShifter&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief PhaseShifter を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYPHASESHIFTER_H
