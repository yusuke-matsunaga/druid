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

#include "pym/PyList.h"
#include "TpgFFR.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyTpgFFRConv PyTpgFFR.h "PyTpgFFR.h"
/// @brief const TpgFFR* を PyObject* に変換するファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyTpgFFRConv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief const TpgFFR* を PyObject* に変換する．
  PyObject*
  operator()(
    const TpgFFR* val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyTpgFFRDeconv PyTpgFFR.h "PyTpgFFR.h"
/// @brief TpgFFR を取り出すファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyTpgFFRDeconv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief PyObject* から const TpgFFR* を取り出す．
  bool
  operator()(
    PyObject* obj,
    const TpgFFR*& val
  );

};


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
  )
  {
    PyTpgFFRConv conv;
    return conv(val);
  }

  /// @brief PyObject が TpgFFR タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief TpgFFR を表す PyObject から TpgFFR を取り出す．
  /// @return TpgFFR を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const TpgFFR*
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TpgFFR のリストを表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyList(
    const vector<const TpgFFR*>& val_list ///< [in] 値のリスト
  )
  {
    return PyList::ToPyObject<const TpgFFR*, PyTpgFFRConv>(val_list);
  }

  /// @brief TpgFFR を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTPGFFR_H
