#ifndef PYTPGNODE_H
#define PYTPGNODE_H

/// @file PyTpgNode.h
/// @brief PyTpgNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyTpgNode PyTpgNode.h "PyTpgNode.h"
/// @brief Python 用の TpgNode 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTpgNode
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

  /// @brief PyObject が TpgNode タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  // @brief TpgNode を PyObject に変換する．
  static
  PyObject*
  ToPyObject(
    const TpgNode* val
  );

  /// @brief TpgNode を表す PyObject から TpgNode を取り出す．
  /// @return TpgNode を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const TpgNode*
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TpgNode を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTPGNODE_H
