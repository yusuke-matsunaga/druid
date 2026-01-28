
/// @file PyTpgMFFC.cc
/// @brief PyTpgMFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgMFFC.h"
#include "pym/PyTpgFFR.h"
#include "pym/PyTpgFFRList.h"
#include "pym/PyTpgNode.h"
#include "pym/PyUlong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgMFFC_Object
{
  PyObject_HEAD
  TpgMFFC mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgMFFC_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgMFFC::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgMFFC_Object*>(self);
  obj->mVal.~TpgMFFC();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
is_valid(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgMFFC::_get_ref(self);
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
root(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgMFFC::_get_ref(self);
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

PyObject*
ffr_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgMFFC::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.ffr_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ffr(
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
  auto& val = PyTpgMFFC::_get_ref(self);
  try {
    return PyTpgFFR::ToPyObject(val.ffr(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ffr_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgMFFC::_get_ref(self);
  try {
    return PyTpgFFRList::ToPyObject(val.ffr_list());
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
  {"ffr_num",
   ffr_num,
   METH_NOARGS,
   PyDoc_STR("return the number of FFRs")},
  {"ffr",
   reinterpret_cast<PyCFunction>(ffr),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return FFR")},
  {"ffr_list",
   ffr_list,
   METH_NOARGS,
   PyDoc_STR("return list of FFRs")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
get_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgMFFC::_get_ref(self);
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

// getter/setter定義
PyGetSetDef getsets[] = {
  {"id", get_id, nullptr, PyDoc_STR(""), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief TpgMFFC オブジェクトを使用可能にする．
bool
PyTpgMFFC::init(
  PyObject* m
)
{
  TpgMFFC_Type.tp_name = "TpgMFFC";
  TpgMFFC_Type.tp_basicsize = sizeof(TpgMFFC_Object);
  TpgMFFC_Type.tp_itemsize = 0;
  TpgMFFC_Type.tp_dealloc = dealloc_func;
  TpgMFFC_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgMFFC_Type.tp_doc = PyDoc_STR("Python extended object for TpgMFFC");
  TpgMFFC_Type.tp_methods = methods;
  TpgMFFC_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "TpgMFFC", &TpgMFFC_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgMFFC を PyObject に変換する．
PyObject*
PyTpgMFFC::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgMFFC::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgMFFC_Object*>(obj);
  new (&my_obj->mVal) TpgMFFC(val);
  return obj;
}

// @brief PyObject が TpgMFFC タイプか調べる．
bool
PyTpgMFFC::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgMFFC_Type);
}

// @brief PyObject から TpgMFFC を取り出す．
TpgMFFC&
PyTpgMFFC::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgMFFC_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgMFFC を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgMFFC::_typeobject()
{
  return &TpgMFFC_Type;
}

END_NAMESPACE_DRUID
