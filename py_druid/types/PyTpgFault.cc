
/// @file PyTpgFault.cc
/// @brief Python TpgFault の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "PyTpgFault.h"
#include "TpgNode.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TpgFaultObject
{
  PyObject_HEAD
  const TpgFault* mFault;
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
  // mFault は borrowed reference なので開放しない．
  Py_TYPE(self)->tp_free(self);
}

// str() 関数
PyObject*
TpgFault_str(
  PyObject* self
)
{
  auto tmp_str = reinterpret_cast<TpgFaultObject*>(self)->mFault->str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
TpgFault_is_stem_fault(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fault = PyTpgFault::_get(self);
  return PyBool_FromLong(fault->is_stem_fault());
}

PyObject*
TpgFault_is_branch_fault(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fault = PyTpgFault::_get(self);
  return PyBool_FromLong(fault->is_branch_fault());
}

PyObject*
TpgFault_is_rep(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fault = PyTpgFault::_get(self);
  return PyBool_FromLong(fault->is_rep());
}

// メソッド定義
PyMethodDef TpgFault_methods[] = {
  {"is_stem_fault", TpgFault_is_stem_fault, METH_NOARGS,
   PyDoc_STR("True if STEM fault")},
  {"is_branch_fault", TpgFault_is_branch_fault, METH_NOARGS,
   PyDoc_STR("True if BRANCH fault")},
  {"is_rep", TpgFault_is_rep, METH_NOARGS,
   PyDoc_STR("True if representative fault")},
  {nullptr, nullptr, 0, nullptr}
};

// ID を返す．
PyObject*
TpgFault_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto fault = PyTpgFault::_get(self);
  auto id = fault->id();
  return PyLong_FromLong(id);
}

// tpg_inode を返す．
PyObject*
TpgFault_tpg_inode(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto fault = PyTpgFault::_get(self);
  auto node = fault->tpg_inode();
  return PyLong_FromLong(node->id());
}

// tpg_onode を返す．
PyObject*
TpgFault_tpg_onode(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto fault = PyTpgFault::_get(self);
  auto node = fault->tpg_onode();
  return PyLong_FromLong(node->id());
}

// fault_pos を返す．
PyObject*
TpgFault_fault_pos(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto fault = PyTpgFault::_get(self);
  auto pos = fault->fault_pos();
  return PyLong_FromLong(pos);
}

// tpg_pos を返す．
PyObject*
TpgFault_tpg_pos(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto fault = PyTpgFault::_get(self);
  auto pos = fault->tpg_pos();
  return PyLong_FromLong(pos);
}

// val を返す．
PyObject*
TpgFault_val(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto fault = PyTpgFault::_get(self);
  auto val = fault->val();
  auto ival = val == Fval2::zero ? 0 : 1;
  return PyLong_FromLong(ival);
}

// rep_fault を返す．
PyObject*
TpgFault_rep_fault(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto fault = PyTpgFault::_get(self);
  auto node = fault->rep_fault();
  return PyLong_FromLong(node->id());
}

// getset メソッド定義
PyGetSetDef TpgFault_getsetters[] = {
  {"id", TpgFault_id, nullptr, PyDoc_STR("ID")},
  {"tpg_inode", TpgFault_tpg_inode, nullptr, PyDoc_STR("input node")},
  {"tpg_onode", TpgFault_tpg_onode, nullptr, PyDoc_STR("output node")},
  {"fault_pos", TpgFault_fault_pos, nullptr, PyDoc_STR("fault's position")},
  {"tpg_pos", TpgFault_tpg_pos, nullptr, PyDoc_STR("position in TpgNode's fanin")},
  {"val", TpgFault_val, nullptr, PyDoc_STR("fault value")},
  {"rep_fault", TpgFault_rep_fault, nullptr, PyDoc_STR("representative fault")},
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
  const TpgFault* val
)
{
  auto obj = TpgFaultType.tp_alloc(&TpgFaultType, 0);
  auto tpgfault_obj = reinterpret_cast<TpgFaultObject*>(obj);
  tpgfault_obj->mFault = val;
  return obj;
}

// @brief PyObject が TpgFault タイプか調べる．
bool
PyTpgFault::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TpgFault を表す PyObject から TpgFault を取り出す．
const TpgFault*
PyTpgFault::_get(
  PyObject* obj
)
{
  auto tpgfault_obj = reinterpret_cast<TpgFaultObject*>(obj);
  return tpgfault_obj->mFault;
}

// @brief TpgFault を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgFault::_typeobject()
{
  return &TpgFaultType;
}

END_NAMESPACE_DRUID
