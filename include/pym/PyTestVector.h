#ifndef PYTESTVECTOR_H
#define PYTESTVECTOR_H

/// @file PyTestVector.h
/// @brief PyTestVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "pym/PyList.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyTestVectorConv PyTestVectorConv.h "PyTestVectorConv.h"
/// @brief TestVector を PyObject* に変換するファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyTestVectorConv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief TestVector を PyObject* に変換する．
  PyObject*
  operator()(
    TestVector val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyTestVectorDeconv PyTestVectorDeconv.h "PyTestVectorDeconv.h"
/// @brief TestVector に変換するファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyTestVectorDeconv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief PyObject* を TestVector に変換する
  /// @return 正しく変換できた時に true を返す．
  bool
  operator()(
    PyObject* obj,
    TestVector& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyTestVector PyTestVector.h "PyTestVector.h"
/// @brief Python 用の TestVector 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTestVector
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

  /// @brief TestVector を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    TestVector val ///< [in] 値
  )
  {
    PyTestVectorConv conv;
    return conv(val);
  }

  /// @brief PyObject が TestVector タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief TestVector を表す PyObject から TestVector を取り出す．
  /// @return TestVector を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const TestVector&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TestVector のリストを表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyList(
    const vector<TestVector>& val_list ///< [in] 値のリスト
  )
  {
    return PyList::ToPyObject<TestVector, PyTestVectorConv>(val_list);
  }

  /// @brief TestVector のリストを表す PyObject から TestVector のリストを取り出す．
  /// @return 成功したら true を返す．
  ///
  /// obj は単一の TestVector か TestVector のシーケンスタイプである必要がある．
  static
  bool
  FromPyList(
    PyObject* obj,              ///< [in] 変換元の PyObject
    vector<TestVector>& tv_list ///< [out] 変換結果を格納する変数
  )
  {
    return PyList::FromPyObject<TestVector, PyTestVectorDeconv>(obj, tv_list);
  }

  /// @brief TestVector を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTESTVECTOR_H
