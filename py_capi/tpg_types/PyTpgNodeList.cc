
/// @file PyTpgNodeList.cc
/// @brief PyTpgNodeList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgNodeList.h"
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
struct TpgNodeList_Object
{
  PyObject_HEAD
  TpgNodeList mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgNodeList_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgNodeList::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgNodeList_Object*>(self);
  obj->mVal.~TpgNodeList();
  Py_TYPE(self)->tp_free(self);
}

Py_ssize_t
sq_length(
  PyObject* self
)
{
  auto& val = PyTpgNodeList::_get_ref(self);
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
  auto& val = PyTpgNodeList::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val[index]);
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
  auto& val = PyTpgNodeList::_get_ref(self);
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


// @brief TpgNodeList オブジェクトを使用可能にする．
bool
PyTpgNodeList::init(
  PyObject* m
)
{
  TpgNodeList_Type.tp_name = "TpgNodeList";
  TpgNodeList_Type.tp_basicsize = sizeof(TpgNodeList_Object);
  TpgNodeList_Type.tp_itemsize = 0;
  TpgNodeList_Type.tp_dealloc = dealloc_func;
  TpgNodeList_Type.tp_as_sequence = &sequence;
  TpgNodeList_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgNodeList_Type.tp_doc = PyDoc_STR("Python extended object for TpgNodeList");
  TpgNodeList_Type.tp_methods = methods;
  if ( !PyModule::reg_type(m, "TpgNodeList", &TpgNodeList_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgNodeList を PyObject に変換する．
PyObject*
PyTpgNodeList::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgNodeList::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgNodeList_Object*>(obj);
  new (&my_obj->mVal) TpgNodeList(val);
  return obj;
}

// PyObject を TpgNodeList に変換する．
bool
PyTpgNodeList::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyTpgNodeList::Check(obj) ) {
    val = PyTpgNodeList::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が TpgNodeList タイプか調べる．
bool
PyTpgNodeList::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgNodeList_Type);
}

// @brief PyObject から TpgNodeList を取り出す．
TpgNodeList&
PyTpgNodeList::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgNodeList_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgNodeList を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgNodeList::_typeobject()
{
  return &TpgNodeList_Type;
}

END_NAMESPACE_DRUID
