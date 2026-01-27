
/// @file PyTpgFFR.cc
/// @brief PyTpgFFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgFFR.h"
#include "pym/PyTpgNode.h"
#include "pym/PyTpgNodeList.h"
#include "pym/PyUlong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgFFR_Object
{
  PyObject_HEAD
  TpgFFR mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgFFR_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgFFR::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgFFR_Object*>(self);
  obj->mVal.~TpgFFR();
  Py_TYPE(self)->tp_free(self);
}

// True if valid
PyObject*
is_valid(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgFFR::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_valid());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// return root node
PyObject*
root(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgFFR::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.root());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// return input node
PyObject*
input(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgFFR::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.input(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// return list of input nodes
PyObject*
input_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgFFR::_get_ref(self);
  try {
    return PyTpgNodeList::ToPyObject(val.input_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// return node
PyObject*
node(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgFFR::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.node(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// return list of nodes
PyObject*
node_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgFFR::_get_ref(self);
  try {
    return PyTpgNodeList::ToPyObject(val.node_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// メソッド定義
PyMethodDef methods[] = {
  {"is_valid",
   is_valid,
   METH_NOARGS,
   PyDoc_STR("True if valid")},
  {"root",
   root,
   METH_NOARGS,
   PyDoc_STR("return root node")},
  {"input",
   reinterpret_cast<PyCFunction>(input),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return input node")},
  {"input_list",
   input_list,
   METH_NOARGS,
   PyDoc_STR("return list of input nodes")},
  {"node",
   reinterpret_cast<PyCFunction>(node),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return node")},
  {"node_list",
   node_list,
   METH_NOARGS,
   PyDoc_STR("return list of nodes")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
get_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFFR::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.id());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_input_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFFR::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.input_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_node_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFFR::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.node_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// getter/setter定義
PyGetSetDef getsets[] = {
  {"id", get_id, nullptr, PyDoc_STR(""), nullptr},
  {"input_num", get_input_num, nullptr, PyDoc_STR(""), nullptr},
  {"node_num", get_node_num, nullptr, PyDoc_STR(""), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief TpgFFR オブジェクトを使用可能にする．
bool
PyTpgFFR::init(
  PyObject* m
)
{
  TpgFFR_Type.tp_name = "TpgFFR";
  TpgFFR_Type.tp_basicsize = sizeof(TpgFFR_Object);
  TpgFFR_Type.tp_itemsize = 0;
  TpgFFR_Type.tp_dealloc = dealloc_func;
  TpgFFR_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgFFR_Type.tp_doc = PyDoc_STR("Python extended object for TpgFFR");
  TpgFFR_Type.tp_methods = methods;
  TpgFFR_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "TpgFFR", &TpgFFR_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgFFR を PyObject に変換する．
PyObject*
PyTpgFFR::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgFFR::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgFFR_Object*>(obj);
  new (&my_obj->mVal) TpgFFR(val);
  return obj;
}

// @brief PyObject が TpgFFR タイプか調べる．
bool
PyTpgFFR::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgFFR_Type);
}

// @brief PyObject から TpgFFR を取り出す．
TpgFFR&
PyTpgFFR::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgFFR_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgFFR を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgFFR::_typeobject()
{
  return &TpgFFR_Type;
}

END_NAMESPACE_DRUID
