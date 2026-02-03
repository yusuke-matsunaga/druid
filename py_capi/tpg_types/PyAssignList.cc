
/// @file PyAssignList.cc
/// @brief PyAssignList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyAssignList.h"
#include "pym/PyAssignIter2.h"
#include "pym/PyAssign.h"
#include "pym/PyUlong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct AssignList_Object
{
  PyObject_HEAD
  AssignList mVal;
};

// Python 用のタイプ定義
PyTypeObject AssignList_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyAssignList::init() 中で初期化する．
};

Py_ssize_t
sq_length(
  PyObject* self
)
{
  auto& val = PyAssignList::_get_ref(self);
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
  auto& val = PyAssignList::_get_ref(self);
  try {
    return PyAssign::ToPyObject(val.elem(index));
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

// iter 関数
PyObject*
iter_func(
  PyObject* self
)
{
  auto& val = PyAssignList::_get_ref(self);
  try {
    return PyAssignIter2::ToPyObject(val.iter());
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
clear(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyAssignList::_get_ref(self);
  try {
    val.clear();
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
add(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "assign",
    nullptr
  };
  PyObject* assign_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyAssign::_typeobject(), &assign_obj) ) {
    return nullptr;
  }
  Assign assign;
  if ( assign_obj != nullptr ) {
    if ( !PyAssign::FromPyObject(assign_obj, assign) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to Assign");
      return nullptr;
    }
  }
  auto& val = PyAssignList::_get_ref(self);
  try {
    val.add(assign);
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
merge(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "src_list",
    nullptr
  };
  PyObject* src_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyAssignList::_typeobject(), &src_list_obj) ) {
    return nullptr;
  }
  AssignList src_list;
  if ( src_list_obj != nullptr ) {
    if ( !PyAssignList::FromPyObject(src_list_obj, src_list) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to AssignList");
      return nullptr;
    }
  }
  auto& val = PyAssignList::_get_ref(self);
  try {
    val.merge(src_list);
    Py_RETURN_NONE;
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
  {"clear",
   clear,
   METH_NOARGS,
   PyDoc_STR("clear")},
  {"add",
   reinterpret_cast<PyCFunction>(add),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add Assign\n"
             ":param Assign assign: ノードの値")},
  {"merge",
   reinterpret_cast<PyCFunction>(merge),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("merge AssignList\n"
             ":param AssignList src_list: マージ対象のリスト")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief AssignList オブジェクトを使用可能にする．
bool
PyAssignList::init(
  PyObject* m
)
{
  AssignList_Type.tp_name = "AssignList";
  AssignList_Type.tp_basicsize = sizeof(AssignList_Object);
  AssignList_Type.tp_itemsize = 0;
  AssignList_Type.tp_as_sequence = &sequence;
  AssignList_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  AssignList_Type.tp_doc = PyDoc_STR("Python extended object for AssignList");
  AssignList_Type.tp_iter = iter_func;
  AssignList_Type.tp_methods = methods;
  if ( !PyModule::reg_type(m, "AssignList", &AssignList_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// AssignList を PyObject に変換する．
PyObject*
PyAssignList::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyAssignList::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<AssignList_Object*>(obj);
  new (&my_obj->mVal) AssignList(val);
  return obj;
}

// PyObject を AssignList に変換する．
bool
PyAssignList::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyAssignList::Check(obj) ) {
    val = PyAssignList::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が AssignList タイプか調べる．
bool
PyAssignList::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &AssignList_Type);
}

// @brief PyObject から AssignList を取り出す．
AssignList&
PyAssignList::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<AssignList_Object*>(obj);
  return my_obj->mVal;
}

// @brief AssignList を表すオブジェクトの型定義を返す．
PyTypeObject*
PyAssignList::_typeobject()
{
  return &AssignList_Type;
}

END_NAMESPACE_DRUID
