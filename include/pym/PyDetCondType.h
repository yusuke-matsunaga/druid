#ifndef PYDETCONDTYPE_H
#define PYDETCONDTYPE_H

/// @file PyDetCondType.h
/// @brief PyDetCondType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "DetCond.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyDetCondType PyDetCondType.h "PyDetCondType.h"
/// @brief Python 用の DetCondType 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyDetCondType
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

  /// @brief DetCond::Type を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    DetCond::Type val ///< [in] 値
  );

  /// @brief PyObject が DetCond::Type タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief DetCond::Type を表す PyObject から DetCond::Type を取り出す．
  /// @return DetCondType を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  DetCond::Type
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief DetCond::Type を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYDETCONDTYPE_H
