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
/// @class PyTestVector PyTestVector.h "PyTestVector.h"
/// @brief Python 用の TestVector 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTestVector
{
public:

  /// @brief TestVector を PyObject* に変換するファンクタクラス
  struct Conv {
    PyObject*
    operator()(
      const TestVector& val
    );
  };

  /// @brief PyObject* から TestVector に変換するファンクタクラス
  struct Deconv {
    bool
    operator()(
      PyObject* obj,
      TestVector& val
    );
  };


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
    const TestVector& val ///< [in] 値
  )
  {
    Conv conv;
    return conv(val);
  }

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
    return PyList<TestVector, PyTestVector>::ToPyObject(val_list);
  }

  /// @brief PyObject から TestVector を取り出す．
  /// @return 正しく変換できた時に true を返す．
  static
  bool
  FromPyObject(
    PyObject* obj,  ///< [in] Python のオブジェクト
    TestVector& val ///< [out] 結果を格納する変数
  )
  {
    Deconv deconv;
    return deconv(obj, val);
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
    return PyList<TestVector, PyTestVector>::FromPyObject(obj, tv_list);
  }

  /// @brief PyObject が TestVector タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief TestVector を表す PyObject から TestVector を取り出す．
  /// @return TestVector を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  TestVector&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TestVector を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTESTVECTOR_H
