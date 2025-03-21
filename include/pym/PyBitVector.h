#ifndef PYBITVECTOR_H
#define PYBITVECTOR_H

/// @file PyBitVector.h
/// @brief PyBitVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "BitVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyBitVector PyBitVector.h "PyBitVector.h"
/// @brief Python 用の BitVector 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyBitVector
{
  using ElemType = BitVector;

public:

  /// @brief BitVector を PyObject* に変換するファンクタクラス
  struct Conv {
    PyObject*
    operator()(
      const BitVector& val
    );
  };

  /// @brief PyObject* から BitVector を取り出すファンクタクラス
  struct Deconv {
    bool
    operator()(
      PyObject* obj,
      BitVector& val
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

  /// @brief BitVector を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const BitVector& val ///< [in] 値
  )
  {
    Conv conv;
    return conv(val);
  }

  /// @brief PyObject から BitVector を取り出す．
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

  /// @brief PyObject が BitVector タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief BitVector を表す PyObject から BitVector を取り出す．
  /// @return BitVector を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  BitVector&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief BitVector を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYBITVECTOR_H
