#ifndef PYTPGMFFC_H
#define PYTPGMFFC_H

/// @file PyTpgMFFC.h
/// @brief PyTpgMFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "TpgMFFC.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyTpgMFFC PyTpgMFFC.h "PyTpgMFFC.h"
/// @brief Python 用の TpgMFFC 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTpgMFFC
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

  /// @brief TpgMFFC を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const TpgMFFC& val ///< [in] 値
  );

  /// @brief PyObject が TpgMFFC タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief TpgMFFC を表す PyObject から TpgMFFC を取り出す．
  /// @return TpgMFFC を返す．
  ///
  /// _check(obj) == true であると仮定している．
  static
  const TpgMFFC&
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TpgMFFC を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTPGMFFC_H
