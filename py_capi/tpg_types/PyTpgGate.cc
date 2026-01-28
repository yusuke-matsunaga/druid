
/// @file PyTpgGate.cc
/// @brief PyTpgGate の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgGate.h"
#include "pym/PyTpgNode.h"
#include "pym/PyTpgFault.h"
#include "pym/PyVal3.h"
#include "pym/PyFval2.h"
#include "pym/PyPrimType.h"
#include "pym/PyExpr.h"
#include "pym/PyString.h"
#include "pym/PyUlong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgGate_Object
{
  PyObject_HEAD
  TpgGate mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgGate_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgGate::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgGate_Object*>(self);
  obj->mVal.~TpgGate();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
is_valid(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgGate::_get_ref(self);
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

PyObject*
output_node(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.output_node());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
input_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgGate::_get_ref(self);
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
input_node(
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
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.input_node(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
branch_info(
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
  auto& val = PyTpgGate::_get_ref(self);
  try {
    auto val1 = PyTpgNode::ToPyObject(val.branch_info(pos).node);
    auto val2 = PyUlong::ToPyObject(val.branch_info(pos).ipos);
    return Py_BuildValue("(Ok)", val1, val2);
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_ppi(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_ppi());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_ppo(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_ppo());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_simple(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_simple());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_complex(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_complex());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
cval(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    "val",
    nullptr
  };
  unsigned long ipos;
  PyObject* ival_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "kO!",
                                    const_cast<char**>(kwlist),
                                    &ipos,
                                    PyVal3::_typeobject(), &ival_obj) ) {
    return nullptr;
  }
  Val3 ival = Val3::_X;
  if ( ival_obj != nullptr ) {
    if ( !PyVal3::FromPyObject(ival_obj, ival) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to Val3");
      return nullptr;
    }
  }
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyVal3::ToPyObject(val.cval(ipos, ival));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
stem_fault(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "fval",
    nullptr
  };
  PyObject* fval_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyFval2::_typeobject(), &fval_obj) ) {
    return nullptr;
  }
  Fval2 fval = Fval2::zero;
  if ( fval_obj != nullptr ) {
    if ( !PyFval2::FromPyObject(fval_obj, fval) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to Fval2");
      return nullptr;
    }
  }
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyTpgFault::ToPyObject(val.stem_fault(fval));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
branch_fault(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "ipos",
    "fval",
    nullptr
  };
  unsigned long ipos;
  PyObject* fval_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "kO!",
                                    const_cast<char**>(kwlist),
                                    &ipos,
                                    PyFval2::_typeobject(), &fval_obj) ) {
    return nullptr;
  }
  Fval2 fval = Fval2::zero;
  if ( fval_obj != nullptr ) {
    if ( !PyFval2::FromPyObject(fval_obj, fval) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to Fval2");
      return nullptr;
    }
  }
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyTpgFault::ToPyObject(val.branch_fault(ipos, fval));
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
  {"output_node",
   output_node,
   METH_NOARGS,
   PyDoc_STR("return Output Node")},
  {"input_num",
   input_num,
   METH_NOARGS,
   PyDoc_STR("return the number of inputs")},
  {"input_node",
   reinterpret_cast<PyCFunction>(input_node),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return Input Node")},
  {"branch_info",
   reinterpret_cast<PyCFunction>(branch_info),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return BranchInfo")},
  {"is_ppi",
   is_ppi,
   METH_NOARGS,
   PyDoc_STR("True if PPI")},
  {"is_ppo",
   is_ppo,
   METH_NOARGS,
   PyDoc_STR("True if PPO")},
  {"is_simple",
   is_simple,
   METH_NOARGS,
   PyDoc_STR("True if simple type")},
  {"is_complex",
   is_complex,
   METH_NOARGS,
   PyDoc_STR("True if complex type")},
  {"cval",
   reinterpret_cast<PyCFunction>(cval),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return the controlling value")},
  {"stem_fault",
   reinterpret_cast<PyCFunction>(stem_fault),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return Stem Fault")},
  {"branch_fault",
   reinterpret_cast<PyCFunction>(branch_fault),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return Stem Fault")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
get_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgGate::_get_ref(self);
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
get_name(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyString::ToPyObject(val.name());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_primitive_type(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyPrimType::ToPyObject(val.primitive_type());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_expr(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyExpr::ToPyObject(val.expr());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_extra_node_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgGate::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.extra_node_num());
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
  {"name", get_name, nullptr, PyDoc_STR(""), nullptr},
  {"primitive_type", get_primitive_type, nullptr, PyDoc_STR(""), nullptr},
  {"expr", get_expr, nullptr, PyDoc_STR(""), nullptr},
  {"extra_node_num", get_extra_node_num, nullptr, PyDoc_STR(""), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief TpgGate オブジェクトを使用可能にする．
bool
PyTpgGate::init(
  PyObject* m
)
{
  TpgGate_Type.tp_name = "TpgGate";
  TpgGate_Type.tp_basicsize = sizeof(TpgGate_Object);
  TpgGate_Type.tp_itemsize = 0;
  TpgGate_Type.tp_dealloc = dealloc_func;
  TpgGate_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgGate_Type.tp_doc = PyDoc_STR("Python extended object for TpgGate");
  TpgGate_Type.tp_methods = methods;
  TpgGate_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "TpgGate", &TpgGate_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgGate を PyObject に変換する．
PyObject*
PyTpgGate::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgGate::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgGate_Object*>(obj);
  new (&my_obj->mVal) TpgGate(val);
  return obj;
}

// @brief PyObject が TpgGate タイプか調べる．
bool
PyTpgGate::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgGate_Type);
}

// @brief PyObject から TpgGate を取り出す．
TpgGate&
PyTpgGate::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgGate_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgGate を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgGate::_typeobject()
{
  return &TpgGate_Type;
}

END_NAMESPACE_DRUID
