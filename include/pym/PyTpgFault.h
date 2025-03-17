#ifndef PYTPGFAULT_H
#define PYTPGFAULT_H

/// @file PyTpgFault.h
/// @brief PyTpgFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "pym/PyList.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyTpgFaultConv PyTpgFault.h "PyTpgFault.h"
/// @brief const TpgFault* を PyObject* に変換するファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyTpgFaultConv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief const TpgFault* を PyObject* に変換する．
  PyObject*
  operator()(
    const TpgFault* val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyTpgFaultDeconv PyTpgFaultDeconv.h "PyTpgFaultDeconv.h"
/// @brief PyObject* を const TpgFault* に変換するファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyTpgFaultDeconv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief PyObject* を const TpgFault* に変換する
  /// @return 正しく変換できた時に true を返す．
  bool
  operator()(
    PyObject* obj,
    const TpgFault*& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyTpgFault PyTpgFault.h "PyTpgFault.h"
/// @brief Python 用の TpgFault 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTpgFault
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

  /// @brief TpgFault を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const TpgFault* val ///< [in] 値
  )
  {
    PyTpgFaultConv conv;
    return conv(val);
  }

  /// @breif PyObject から TogFault を取り出す．
  static
  bool
  FromPyObject(
    PyObject* obj,
    const TpgFault*& val
  )
  {
    PyTpgFaultDeconv deconv;
    return deconv(obj, val);
  }

  /// @brief PyObject が TpgFault タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  )
  {
    return Py_IS_TYPE(obj, _typeobject());
  }

  /// @brief TpgFault を表す PyObject から TpgFault を取り出す．
  /// @return TpgFault を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const TpgFault*
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TpgFault のリストを表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyList(
    const vector<const TpgFault*>& val_list ///< [in] 値のリスト
  )
  {
    return PyList::ToPyObject<const TpgFault*, PyTpgFaultConv>(val_list);
  }

  /// @brief TpgFault のリストを表す PyObject から TpgFault のリストを取り出す．
  /// @return 結果のリストを返す．
  ///
  /// obj は単一の TpgFault か TpgFault のシーケンスタイプである必要がある．
  static
  bool
  FromPyList(
    PyObject* obj,                         ///< [in] 変換元の PyObject
    std::vector<const TpgFault*>& val_list ///< [out] 変換結果を格納するリスト
  )
  {
    return PyList::FromPyObject<const TpgFault*, PyTpgFaultDeconv>(obj, val_list);
  }

  /// @brief TpgFault を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTPGFAULT_H
