
/// @file PyTpgFault.cc
/// @brief PyTpgFault の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgFault.h"
#include "pym/PyTpgNode.h"
#include "pym/PyTpgGate.h"
#include "pym/PyFaultType.h"
#include "pym/PyFval2.h"
#include "pym/PyAssignList.h"
#include "pym/PyList.h"
#include "pym/PyString.h"
#include "pym/PyLong.h"
#include "pym/PyBoolList.h"
#include "pym/PyBool.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgFault_Object
{
  PyObject_HEAD
  TpgFault mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgFault_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgFault::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgFault_Object*>(self);
  obj->mVal.~TpgFault();
  Py_TYPE(self)->tp_free(self);
}

// hash 関数
Py_hash_t
hash_func(
  PyObject* self
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return val.hash();
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return 0;
  }
}

// str 関数
PyObject*
str_func(
  PyObject* self
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyString::ToPyObject(val.str());
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// richcompare 関数
PyObject*
richcompare_func(
  PyObject* self,
  PyObject* other,
  int op
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    if ( PyTpgFault::Check(other) ) {
      auto& val2 = PyTpgFault::_get_ref(other);
      if ( op == Py_EQ ) {
        return PyBool_FromLong(val == val2);
      }
      if ( op == Py_NE ) {
        return PyBool_FromLong(val != val2);
      }
    }
    Py_RETURN_NOTIMPLEMENTED;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_valid(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgFault::_get_ref(self);
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
is_stem(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_stem());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_branch(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_branch());
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
  {"is_stem",
   is_stem,
   METH_NOARGS,
   PyDoc_STR("True if stem fault")},
  {"is_branch",
   is_branch,
   METH_NOARGS,
   PyDoc_STR("True if branch fault")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
get_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyLong::ToPyObject(val.id());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_gate(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyTpgGate::ToPyObject(val.gate());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_fault_type(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyFaultType::ToPyObject(val.fault_type());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_fval(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyFval2::ToPyObject(val.fval());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_branch_pos(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyLong::ToPyObject(val.branch_pos());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_input_vals(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyBoolList::ToPyObject(val.input_vals());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_rep_fault(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyTpgFault::ToPyObject(val.rep_fault());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_origin_node(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.origin_node());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_excitation_condition(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyAssignList::ToPyObject(val.excitation_condition());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_ffr_root(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.ffr_root());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_ffr_propagate_condition(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgFault::_get_ref(self);
  try {
    return PyAssignList::ToPyObject(val.ffr_propagate_condition());
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
  {"gate", get_gate, nullptr, PyDoc_STR(""), nullptr},
  {"fault_type", get_fault_type, nullptr, PyDoc_STR(""), nullptr},
  {"fval", get_fval, nullptr, PyDoc_STR(""), nullptr},
  {"branch_pos", get_branch_pos, nullptr, PyDoc_STR(""), nullptr},
  {"input_vals", get_input_vals, nullptr, PyDoc_STR(""), nullptr},
  {"rep_fault", get_rep_fault, nullptr, PyDoc_STR(""), nullptr},
  {"origin_node", get_origin_node, nullptr, PyDoc_STR(""), nullptr},
  {"excitation_condition", get_excitation_condition, nullptr, PyDoc_STR(""), nullptr},
  {"ffr_root", get_ffr_root, nullptr, PyDoc_STR(""), nullptr},
  {"ffr_propagate_condition", get_ffr_propagate_condition, nullptr, PyDoc_STR(""), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief TpgFault オブジェクトを使用可能にする．
bool
PyTpgFault::init(
  PyObject* m
)
{
  TpgFault_Type.tp_name = "TpgFault";
  TpgFault_Type.tp_basicsize = sizeof(TpgFault_Object);
  TpgFault_Type.tp_itemsize = 0;
  TpgFault_Type.tp_dealloc = dealloc_func;
  TpgFault_Type.tp_hash = hash_func;
  TpgFault_Type.tp_str = str_func;
  TpgFault_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgFault_Type.tp_doc = PyDoc_STR("Python extended object for TpgFault");
  TpgFault_Type.tp_richcompare = richcompare_func;
  TpgFault_Type.tp_methods = methods;
  TpgFault_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "TpgFault", &TpgFault_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgFault を PyObject に変換する．
PyObject*
PyTpgFault::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgFault::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgFault_Object*>(obj);
  new (&my_obj->mVal) TpgFault(val);
  return obj;
}

// PyObject を TpgFault に変換する．
bool
PyTpgFault::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyTpgFault::Check(obj) ) {
    val = PyTpgFault::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が TpgFault タイプか調べる．
bool
PyTpgFault::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgFault_Type);
}

// @brief PyObject から TpgFault を取り出す．
TpgFault&
PyTpgFault::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgFault_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgFault を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgFault::_typeobject()
{
  return &TpgFault_Type;
}

END_NAMESPACE_DRUID
