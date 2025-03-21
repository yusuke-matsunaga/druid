#ifndef PYTPGMFFC_H
#define PYTPGMFFC_H

/// @file PyTpgMFFC.h
/// @brief PyTpgMFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "pym/PyList.h"
#include "TpgMFFC.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyTpgMFFC PyTpgMFFC.h "PyTpgMFFC.h"
/// @brief Python 用の TpgMFFC 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTpgMFFC
{
public:

  /// @brief const TpgMFFC* を PyObject* に変換するファンクタクラス
  struct Conv {
    /// @brief const TpgMFFC* を PyObject* に変換する．
    PyObject*
    operator()(
      const TpgMFFC* val
    );
  };

  /// @brief TpgMFFC を取り出すファンクタクラス
  struct Deconv {
    /// @brief PyObject* から const TpgMFFC* を取り出す．
    bool
    operator()(
      PyObject* obj,
      const TpgMFFC*& val
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

  /// @brief TpgMFFC を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const TpgMFFC* val ///< [in] 値
  )
  {
    Conv conv;
    return conv(val);
  }

  /// @brief TpgMFFC のリストを表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyList(
    const vector<const TpgMFFC*>& val_list ///< [in] 値のリスト
  )
  {
    return PyList<const TpgMFFC*, PyTpgMFFC>::ToPyObject(val_list);
  }

  /// @brief PyObject から const TpgMFFC* を取り出す．
  /// @return 正しく変換できた時に true を返す．
  static
  bool
  FromPyObject(
    PyObject* obj,      ///< [in] Python のオブジェクト
    const TpgMFFC*& val ///< [out] 結果を格納する変数
  )
  {
    Deconv deconv;
    return deconv(obj, val);
  }

  /// @brief PyObject が TpgMFFC タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief TpgMFFC を表す PyObject から TpgMFFC を取り出す．
  /// @return TpgMFFC を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const TpgMFFC*
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TpgMFFC を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTPGMFFC_H
