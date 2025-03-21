#ifndef PYTPGNETWORK_H
#define PYTPGNETWORK_H

/// @file PyTpgNetwork.h
/// @brief PyTpgNetwork のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyTpgNetwork PyTpgNetwork.h "PyTpgNetwork.h"
/// @brief Python 用の TpgNetwork 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTpgNetwork
{
  // このクラスは Conv/Deconv を持たない．

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

  /// @brief PyObject が TpgNetwork タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief TpgNetwork を表す PyObject から TpgNetwork を取り出す．
  /// @return TpgNetwork を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  TpgNetwork&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TpgNetwork を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTPGNETWORK_H
