
/// @file PyTpgFaultList.cc
/// @brief PyTpgFaultList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgFaultList.h"
#include "pym/PyTpgFault.h"
#include "pym/PyList.h"
#include "pym/PyUlong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgFaultList_Object
{
  PyObject_HEAD
  TpgFaultList mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgFaultList_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgFaultList::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgFaultList_Object*>(self);
  obj->mVal.~TpgFaultList();
  Py_TYPE(self)->tp_free(self);
}

Py_ssize_t
sq_length(
  PyObject* self
)
{
  auto& val = PyTpgFaultList::_get_ref(self);
  try {
    auto len_val = val.size();
    return len_val;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return -1;
  }
}

PyObject*
sq_item(
  PyObject* self,
  Py_ssize_t index
)
{
  auto& val = PyTpgFaultList::_get_ref(self);
  try {
    return PyTpgFault::ToPyObject(val[index]);
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
  catch ( std::out_of_range err ) {
    std::ostringstream buf;
    buf << "out of range" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// Sequence オブジェクト構造体
PySequenceMethods sequence = {
  .sq_length = sq_length,
  .sq_item = sq_item
};

// True if valid
PyObject*
is_valid(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgFaultList::_get_ref(self);
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

// メソッド定義
PyMethodDef methods[] = {
  {"is_valid",
   is_valid,
   METH_NOARGS,
   PyDoc_STR("True if valid")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief TpgFaultList オブジェクトを使用可能にする．
bool
PyTpgFaultList::init(
  PyObject* m
)
{
  TpgFaultList_Type.tp_name = "TpgFaultList";
  TpgFaultList_Type.tp_basicsize = sizeof(TpgFaultList_Object);
  TpgFaultList_Type.tp_itemsize = 0;
  TpgFaultList_Type.tp_dealloc = dealloc_func;
  TpgFaultList_Type.tp_as_sequence = &sequence;
  TpgFaultList_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgFaultList_Type.tp_doc = PyDoc_STR("Python extended object for TpgFaultList");
  TpgFaultList_Type.tp_methods = methods;
  if ( !PyModule::reg_type(m, "TpgFaultList", &TpgFaultList_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgFaultList を PyObject に変換する．
PyObject*
PyTpgFaultList::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgFaultList::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgFaultList_Object*>(obj);
  new (&my_obj->mVal) TpgFaultList(val);
  return obj;
}

// PyObject を TpgFaultList に変換する．
bool
PyTpgFaultList::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyTpgFaultList::Check(obj) ) {
    val = PyTpgFaultList::_get_ref(obj);
    return true;
  }

  {
    std::vector<TpgFault> fault_list;
    if ( PyList<TpgFault, PyTpgFault>::FromPyObject(obj, fault_list) ) {
      val = TpgFaultList(fault_list);
      return true;
    }
  }
  return false;
}

// @brief PyObject が TpgFaultList タイプか調べる．
bool
PyTpgFaultList::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgFaultList_Type);
}

// @brief PyObject から TpgFaultList を取り出す．
TpgFaultList&
PyTpgFaultList::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgFaultList_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgFaultList を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgFaultList::_typeobject()
{
  return &TpgFaultList_Type;
}

END_NAMESPACE_DRUID
