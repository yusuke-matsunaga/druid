
/// @file PyDiffBits.cc
/// @brief PyDiffBits の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDiffBits.h"
#include "pym/pyfstream.h"
#include "pym/PyUlong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct DiffBits_Object
{
  PyObject_HEAD
  DiffBits mVal;
};

// Python 用のタイプ定義
PyTypeObject DiffBits_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyDiffBits::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<DiffBits_Object*>(self);
  obj->mVal.~DiffBits();
  Py_TYPE(self)->tp_free(self);
}

// hash 関数
Py_hash_t
hash_func(
  PyObject* self
)
{
  auto& val = PyDiffBits::_get_ref(self);
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
  auto& val = PyDiffBits::_get_ref(self);
  try {
    if ( PyDiffBits::Check(other) ) {
      auto& val2 = PyDiffBits::_get_ref(other);
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

// return output ID
PyObject*
output(
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
  auto& val = PyDiffBits::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.output(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// clear
PyObject*
clear(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyDiffBits::_get_ref(self);
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

// add output ID
PyObject*
add_output(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "output",
    nullptr
  };
  unsigned long output;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &output) ) {
    return nullptr;
  }
  auto& val = PyDiffBits::_get_ref(self);
  try {
    val.add_output(output);
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// do sorting
PyObject*
sort(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyDiffBits::_get_ref(self);
  try {
    val.sort();
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// return sorted DiffBits
PyObject*
sorted(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyDiffBits::_get_ref(self);
  try {
    return PyDiffBits::ToPyObject(val.sorted());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// print
PyObject*
print(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "fout",
    nullptr
  };
  PyObject* fout_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
                                    const_cast<char**>(kwlist),
                                    &fout_obj) ) {
    return nullptr;
  }
  auto& val = PyDiffBits::_get_ref(self);
  try {
    auto s = opyfstream(fout_obj);
    val.print(s);
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
  {"output",
   reinterpret_cast<PyCFunction>(output),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return output ID")},
  {"clear",
   clear,
   METH_NOARGS,
   PyDoc_STR("clear")},
  {"add_output",
   reinterpret_cast<PyCFunction>(add_output),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add output ID")},
  {"sort",
   sort,
   METH_NOARGS,
   PyDoc_STR("do sorting")},
  {"sorted",
   sorted,
   METH_NOARGS,
   PyDoc_STR("return sorted DiffBits")},
  {"print",
   reinterpret_cast<PyCFunction>(print),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("print")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
get_elem_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDiffBits::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.elem_num());
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
  {"elem_num", get_elem_num, nullptr, PyDoc_STR(""), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief DiffBits オブジェクトを使用可能にする．
bool
PyDiffBits::init(
  PyObject* m
)
{
  DiffBits_Type.tp_name = "DiffBits";
  DiffBits_Type.tp_basicsize = sizeof(DiffBits_Object);
  DiffBits_Type.tp_itemsize = 0;
  DiffBits_Type.tp_dealloc = dealloc_func;
  DiffBits_Type.tp_hash = hash_func;
  DiffBits_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  DiffBits_Type.tp_doc = PyDoc_STR("Python extended object for DiffBits");
  DiffBits_Type.tp_richcompare = richcompare_func;
  DiffBits_Type.tp_methods = methods;
  DiffBits_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "DiffBits", &DiffBits_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// DiffBits を PyObject に変換する．
PyObject*
PyDiffBits::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyDiffBits::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<DiffBits_Object*>(obj);
  new (&my_obj->mVal) DiffBits(val);
  return obj;
}

// PyObject を DiffBits に変換する．
bool
PyDiffBits::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyDiffBits::Check(obj) ) {
    val = PyDiffBits::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が DiffBits タイプか調べる．
bool
PyDiffBits::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &DiffBits_Type);
}

// @brief PyObject から DiffBits を取り出す．
DiffBits&
PyDiffBits::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<DiffBits_Object*>(obj);
  return my_obj->mVal;
}

// @brief DiffBits を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDiffBits::_typeobject()
{
  return &DiffBits_Type;
}

END_NAMESPACE_DRUID
