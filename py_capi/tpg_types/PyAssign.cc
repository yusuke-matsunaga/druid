
/// @file PyAssign.cc
/// @brief PyAssign の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyAssign.h"
#include "pym/PyTpgNode.h"
#include "pym/PyInt.h"
#include "pym/PyUlong.h"
#include "pym/PyBool.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct Assign_Object
{
  PyObject_HEAD
  Assign mVal;
};

// Python 用のタイプ定義
PyTypeObject Assign_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyAssign::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<Assign_Object*>(self);
  obj->mVal.~Assign();
  Py_TYPE(self)->tp_free(self);
}

// hash 関数
Py_hash_t
hash_func(
  PyObject* self
)
{
  auto& val = PyAssign::_get_ref(self);
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

// richcompare 関数
PyObject*
richcompare_func(
  PyObject* self,
  PyObject* other,
  int op
)
{
  auto& val = PyAssign::_get_ref(self);
  try {
    if ( PyAssign::Check(other) ) {
      auto& val2 = PyAssign::_get_ref(other);
      Py_RETURN_RICHCOMPARE(val, val2, op);
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
node_id(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyAssign::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.node_id());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
node(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyAssign::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.node());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
time(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyAssign::_get_ref(self);
  try {
    return PyInt::ToPyObject(val.time());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
node_time(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyAssign::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.node_time());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
val(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyAssign::_get_ref(self);
  try {
    return PyBool_FromLong(val.val());
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
  {"node_id",
   node_id,
   METH_NOARGS,
   PyDoc_STR("return node ID")},
  {"node",
   node,
   METH_NOARGS,
   PyDoc_STR("return node")},
  {"time",
   time,
   METH_NOARGS,
   PyDoc_STR("return time")},
  {"node_time",
   node_time,
   METH_NOARGS,
   PyDoc_STR("return the packed value of node_id and time")},
  {"val",
   val,
   METH_NOARGS,
   PyDoc_STR("return value")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief Assign オブジェクトを使用可能にする．
bool
PyAssign::init(
  PyObject* m
)
{
  Assign_Type.tp_name = "Assign";
  Assign_Type.tp_basicsize = sizeof(Assign_Object);
  Assign_Type.tp_itemsize = 0;
  Assign_Type.tp_dealloc = dealloc_func;
  Assign_Type.tp_hash = hash_func;
  Assign_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  Assign_Type.tp_doc = PyDoc_STR("Python extended object for Assign");
  Assign_Type.tp_richcompare = richcompare_func;
  Assign_Type.tp_methods = methods;
  if ( !PyModule::reg_type(m, "Assign", &Assign_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// Assign を PyObject に変換する．
PyObject*
PyAssign::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyAssign::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<Assign_Object*>(obj);
  new (&my_obj->mVal) Assign(val);
  return obj;
}

// PyObject を Assign に変換する．
bool
PyAssign::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyAssign::Check(obj) ) {
    val = PyAssign::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が Assign タイプか調べる．
bool
PyAssign::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &Assign_Type);
}

// @brief PyObject から Assign を取り出す．
Assign&
PyAssign::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<Assign_Object*>(obj);
  return my_obj->mVal;
}

// @brief Assign を表すオブジェクトの型定義を返す．
PyTypeObject*
PyAssign::_typeobject()
{
  return &Assign_Type;
}

END_NAMESPACE_DRUID
