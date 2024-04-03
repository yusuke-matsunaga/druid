#ifndef PYTPGFFR_H
#define PYTPGFFR_H

/// @file PyTpgFFR.h
/// @brief PyTpgFFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "TpgFFR.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyTpgFFR PyTpgFFR.h "PyTpgFFR.h"
/// @brief Python 用の TpgFFR 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTpgFFR
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

  /// @brief TpgFFR を PyObject に変換する．
  static
  PyObject*
  ToPyObject(
    const TpgFFR* val ///< [in] 値
  );

  /// @brief TpgFFR のリストを表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyList(
    const vector<const TpgFFR*>& val_list ///< [in] 値のリスト
  );

  /// @brief PyObject が TpgFFR タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief TpgFFR を表す PyObject から TpgFFR を取り出す．
  /// @return TpgFFR を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const TpgFFR*
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TpgFFR を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTPGFFR_H
