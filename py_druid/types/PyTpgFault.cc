
/// @file PyTpgFault.cc
/// @brief Python TpgFault の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "PyTpgFault.h"
#include "PyTpgNode.h"
#include "TpgNode.h"
#include "NodeValList.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TpgFaultObject
{
  PyObject_HEAD
  TpgFault mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgFaultType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
TpgFault_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'TpgFault' is disabled");
  return nullptr;
}

// 終了関数
void
TpgFault_dealloc(
  PyObject* self
)
{
  // mVal は borrowed reference なので開放しない．
  Py_TYPE(self)->tp_free(self);
}

// str() 関数
PyObject*
TpgFault_str(
  PyObject* self
)
{
  auto tmp_str = reinterpret_cast<TpgFaultObject*>(self)->mVal.str();
  return Py_BuildValue("s", tmp_str.c_str());
}

// メソッド定義
PyMethodDef TpgFault_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// ID を返す．
PyObject*
TpgFault_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto fault = PyTpgFault::Get(self);
  auto id = fault.id();
  return PyLong_FromLong(id);
}

PyObject*
TpgFault_origin_node(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto fault = PyTpgFault::Get(self);
  auto node = fault.origin_node();
  return PyTpgNode::ToPyObject(node);
}

PyObject*
TpgFault_ffr_root(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto fault = PyTpgFault::Get(self);
  auto node = fault.ffr_root();
  return PyTpgNode::ToPyObject(node);
}

BEGIN_NONAMESPACE

PyObject*
assign_list_to_pyobj(
  const NodeValList& assign_list
)
{
  SizeType n = assign_list.size();
  auto ans_obj = PyList_New(n);
  if ( !ans_obj ) {
    return nullptr;
  }
  SizeType index = 0;
  for ( auto nodeval: assign_list ) {
    auto node = nodeval.node();
    int time = nodeval.time();
    bool val = nodeval.val();
    auto nodeval_obj = Py_BuildValue("(Oib)", node, time, val);
    PyList_SET_ITEM(ans_obj, index, nodeval_obj);
    ++ index;
  }
  return ans_obj;
}

END_NONAMESPACE

PyObject*
TpgFault_excitation_condition(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto fault = PyTpgFault::Get(self);
  auto assign_list = fault.excitation_condition();
  return assign_list_to_pyobj(assign_list);
}

PyObject*
TpgFault_ffr_propagate_condition(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto fault = PyTpgFault::Get(self);
  auto assign_list = fault.ffr_propagate_condition();
  return assign_list_to_pyobj(assign_list);
}

// getset メソッド定義
PyGetSetDef TpgFault_getsetters[] = {
  {"id", TpgFault_id, nullptr, PyDoc_STR("ID")},
  {"origin_node", TpgFault_origin_node, nullptr,
   PyDoc_STR("origin node for fault propagation")},
  {"ffr_root", TpgFault_ffr_root, nullptr,
   PyDoc_STR("FFR's root node")},
  {"excitation_condition", TpgFault_excitation_condition, nullptr,
   PyDoc_STR("fault excitation condition")},
  {"ffr_propagation_condition", TpgFault_ffr_propagate_condition, nullptr,
   PyDoc_STR("fault propagation condition for FFR")},
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief 'TpgFault' オブジェクトを使用可能にする．
bool
PyTpgFault::init(
  PyObject* m
)
{
  TpgFaultType.tp_name = "TpgFault";
  TpgFaultType.tp_basicsize = sizeof(TpgFaultObject);
  TpgFaultType.tp_itemsize = 0;
  TpgFaultType.tp_dealloc = TpgFault_dealloc;
  TpgFaultType.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgFaultType.tp_doc = PyDoc_STR("TpgFault object");
  TpgFaultType.tp_methods = TpgFault_methods;
  TpgFaultType.tp_getset = TpgFault_getsetters;
  TpgFaultType.tp_new = TpgFault_new;
  TpgFaultType.tp_str = TpgFault_str;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "TpgFault", &TpgFaultType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief TpgFault を PyObject に変換する．
PyObject*
PyTpgFault::ToPyObject(
  const TpgFault& val
)
{
  auto obj = TpgFaultType.tp_alloc(&TpgFaultType, 0);
  auto tpgfault_obj = reinterpret_cast<TpgFaultObject*>(obj);
  tpgfault_obj->mVal = val;
  return obj;
}

// @brief PyObject が TpgFault タイプか調べる．
bool
PyTpgFault::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TpgFault を表す PyObject から TpgFault を取り出す．
TpgFault
PyTpgFault::Get(
  PyObject* obj
)
{
  auto tpgfault_obj = reinterpret_cast<TpgFaultObject*>(obj);
  return tpgfault_obj->mVal;
}

// @brief TpgFault を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgFault::_typeobject()
{
  return &TpgFaultType;
}

END_NAMESPACE_DRUID
