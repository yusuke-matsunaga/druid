#ifndef PYDFFVECTOR_H
#define PYDFFVECTOR_H

/// @file PyDffVector.h
/// @brief PyDffVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "DffVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyDffVector PyDffVector.h "PyDffVector.h"
/// @brief Python 用の DffVector 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyDffVector
{
  using ElemType = DffVector;

public:

  /// @brief const DffVector* を PyObject* に変換するファンクタクラス
  struct Conv {
    /// @brief const DffVector& を PyObject* に変換する．
    PyObject*
    operator()(
      const DffVector& val
    );
  };

  /// @brief DffVector を取り出すファンクタクラス
  struct Deconv {
    /// @brief PyObject* から DffVector を取り出す．
    bool
    operator()(
      PyObject* obj,
      DffVector& val
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

  /// @brief DffVector を表す PyObject を作る．
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

  /// @brief PyObject から DffVector を取り出す．
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

  /// @brief PyObject が DffVector タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief DffVector を表す PyObject から DffVector を取り出す．
  /// @return DffVector を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  DffVector&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief DffVector を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYDFFVECTOR_H
