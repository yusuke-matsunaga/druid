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
/// @class PyTpgNodeConv PyTpgNode.h "PyTpgNode.h"
/// @brief const TpgNode* を PyObject* に変換するファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyTpgNodeConv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief const TpgNode* を PyObject* に変換する．
  PyObject*
  operator()(
    const TpgNode* val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyTpgNodeDeconv PyTpgNode.h "PyTpgNode.h"
/// @brief TpgNode を取り出すファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyTpgNodeDeconv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief PyObject* から const TpgNode* を取り出す．
  bool
  operator()(
    PyObject* obj,
    const TpgNode*& val
  );

};


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

  // @brief TpgNode を PyObject に変換する．
  static
  PyObject*
  ToPyObject(
    const TpgNode* val
  )
  {
    PyTpgNodeConv conv;
    return conv(val);
  }

  /// @brief PyObject が TpgNode タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief TpgNode を表す PyObject から TpgNode を取り出す．
  /// @return TpgNode を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const TpgNode*
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TpgNode を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTPGNODE_H
