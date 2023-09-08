
/// @file PyTpgNode.cc
/// @brief Python TpgNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "PyTpgNode.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TpgNodeObject
{
  PyObject_HEAD
  const TpgNode* mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgNodeType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 終了関数
void
TpgNode_dealloc(
  PyObject* self
)
{
  // auto tpgnode_obj = reinterpret_cast<TpgNodeObject*>(self);
  // 必要なら tpgnode_obj->mVal の終了処理を行う．
  // delete tpgnode_obj->mVal
  Py_TYPE(self)->tp_free(self);
}

// 初期化関数(__init__()相当)
int
TpgNode_init(
  PyObject* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  return 0;
}

// メソッド定義
PyMethodDef TpgNode_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// get/set 関数定義
PyGetSetDef TpgNode_getset[] = {
  {nullptr, nullptr, nullptr, nullptr, nullptr},
};

END_NONAMESPACE


// @brief 'TpgNode' オブジェクトを使用可能にする．
bool
PyTpgNode::init(
  PyObject* m
)
{
  TpgNodeType.tp_name = "TpgNode";
  TpgNodeType.tp_basicsize = sizeof(TpgNodeObject);
  TpgNodeType.tp_itemsize = 0;
  TpgNodeType.tp_dealloc = TpgNode_dealloc;
  TpgNodeType.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgNodeType.tp_doc = PyDoc_STR("TpgNode object");
  TpgNodeType.tp_methods = TpgNode_methods;
  TpgNodeType.tp_getset = TpgNode_getset;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "TpgNode", &TpgNodeType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief TpgNode を PyObject に変換する．
PyObject*
PyTpgNode::ToPyObject(
  const TpgNode* val
)
{
  auto obj = TpgNodeType.tp_alloc(&TpgNodeType, 0);
  auto tpgnode_obj = reinterpret_cast<TpgNodeObject*>(obj);
  tpgnode_obj->mVal = val;
  return obj;
}

// @brief PyObject が TpgNode タイプか調べる．
bool
PyTpgNode::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TpgNode を表す PyObject から TpgNode を取り出す．
const TpgNode*
PyTpgNode::Get(
  PyObject* obj
)
{
  auto tpgnode_obj = reinterpret_cast<TpgNodeObject*>(obj);
  return tpgnode_obj->mVal;
}

// @brief TpgNode を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgNode::_typeobject()
{
  return &TpgNodeType;
}

END_NAMESPACE_DRUID
