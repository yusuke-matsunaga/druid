#ifndef PYDETCOND_H
#define PYDETCOND_H

/// @file PyDetCond.h
/// @brief PyDetCond のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "pym/PyList.h"
#include "DetCond.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyDetCondConv PyDetCond.h "PyDetCond.h"
/// @brief const DetCond* を PyObject* に変換するファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyDetCondConv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief const DetCond& を PyObject* に変換する．
  PyObject*
  operator()(
    const DetCond& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyDetCondDeconv PyDetCond.h "PyDetCond.h"
/// @brief DetCond を取り出すファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyDetCondDeconv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief PyObject* から DetCond を取り出す．
  bool
  operator()(
    PyObject* obj,
    DetCond& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyDetCond PyDetCond.h "PyDetCond.h"
/// @brief Python 用の DetCond 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyDetCond
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

  /// @brief DetCond を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const DetCond& val ///< [in] 値
  )
  {
    PyDetCondConv conv;
    return conv(val);
  }

  /// @brief PyObject が DetCond タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief DetCond を表す PyObject から DetCond を取り出す．
  /// @return DetCond を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const DetCond&
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief DetCond のリストを表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyList(
    const vector<DetCond>& val_list ///< [in] 値のリスト
  )
  {
    return PyList::ToPyObject<DetCond, PyDetCondConv>(val_list);
  }

  /// @brief DetCond のリストを表す PyObject から DetCond のリストを取り出す．
  /// @return 正しく変換できたら true を返す．
  static
  bool
  FromPyList(
    PyObject* obj,                  ///< [in] 対象のオブジェクト
    std::vector<DetCond>& cond_list ///< [out] 結果を格納するリスト
  )
  {
    return PyList::FromPyObject<DetCond, PyDetCondDeconv>(obj, cond_list);
  }

  /// @brief DetCond を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYDETCOND_H
