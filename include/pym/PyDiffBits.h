#ifndef PYDIFFBITS_H
#define PYDIFFBITS_H

/// @file PyDiffBits.h
/// @brief PyDiffBits のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "DiffBits.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyDiffBits PyDiffBits.h "PyDiffBits.h"
/// @brief Python 用の DiffBits 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyDiffBits
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

  /// @brief DiffBits を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const DiffBits& val  ///< [in] 値
  );

  /// @brief PyObject が DiffBits タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief DiffBits を表す PyObject から DiffBits を取り出す．
  /// @return DiffBits を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const DiffBits&
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief DiffBits を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYDIFFBITS_H
