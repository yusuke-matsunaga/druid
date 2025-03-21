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
/// @class PyDetCond PyDetCond.h "PyDetCond.h"
/// @brief Python 用の DetCond 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyDetCond
{
  using ElemType = DetCond;

public:

  /// @brief DetCond を PyObject* に変換するファンクタクラス
  struct Conv {
    PyObject*
    operator()(
      const ElemType& val
    );
  };

  /// @brief PyObject* から DetCond を取り出すファンクタクラス
  struct Deconv {
    bool
    operator()(
      PyObject* obj,
      ElemType& val
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

  /// @brief DetCond を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const ElemType& val ///< [in] 値
  )
  {
    Conv conv;
    return conv(val);
  }

  /// @brief DetCond のリストを表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyList(
    const vector<ElemType>& val_list ///< [in] 値のリスト
  )
  {
    return PyList<ElemType, PyDetCond>::ToPyObject(val_list);
  }

  /// @brief PyObject から DetCond を取り出す．
  /// @return 正しく変換できた時に true を返す．
  static
  bool
  FromPyObject(
    PyObject* obj, ///< [in] Python のオブジェクト
    ElemType& val  ///< [out] 結果を格納する変数
  )
  {
    Deconv deconv;
    return deconv(obj, val);
  }

  /// @brief DetCond のリストを表す PyObject から DetCond のリストを取り出す．
  /// @return 正しく変換できたら true を返す．
  static
  bool
  FromPyList(
    PyObject* obj,                  ///< [in] 対象のオブジェクト
    std::vector<ElemType>& val_list ///< [out] 結果を格納するリスト
  )
  {
    return PyList<ElemType, PyDetCond>::FromPyObject(obj, val_list);
  }

  /// @brief PyObject が DetCond タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief DetCond を表す PyObject から DetCond を取り出す．
  /// @return DetCond を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  DetCond&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief DetCond を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYDETCOND_H
