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
/// @class PyTpgFFR PyTpgFFR.h "PyTpgFFR.h"
/// @brief Python 用の TpgFFR 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTpgFFR
{
public:

  /// @brief const TpgFFR* を PyObject* に変換するファンクタクラス
  struct Conv {
    /// @brief const TpgFFR* を PyObject* に変換する．
    PyObject*
    operator()(
      const TpgFFR* val
    );
  };

  /// @brief TpgFFR を取り出すファンクタクラス
  struct Deconv {
    /// @brief PyObject* から const TpgFFR* を取り出す．
    bool
    operator()(
      PyObject* obj,
      const TpgFFR*& val
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

  /// @brief TpgFFR を PyObject に変換する．
  static
  PyObject*
  ToPyObject(
    const TpgFFR* val ///< [in] 値
  )
  {
    Conv conv;
    return conv(val);
  }

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
    return PyList<const TpgFFR*, PyTpgFFR>::ToPyObject(val_list);
  }

  /// @brief PyObject から const TpgFFR* を取り出す．
  /// @return 正しく変換できた時に true を返す．
  static
  bool
  FromPyObject(
    PyObject* obj,     ///< [in] Python のオブジェクト
    const TpgFFR*& val ///< [out] 結果を格納する変数
  )
  {
    Deconv deconv;
    return deconv(obj, val);
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

  /// @brief TpgFFR を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTPGFFR_H
