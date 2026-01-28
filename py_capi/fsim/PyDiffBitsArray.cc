
/// @file PyDiffBitsArray.cc
/// @brief PyDiffBitsArray の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDiffBitsArray.h"
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
struct DiffBitsArray_Object
{
  PyObject_HEAD
  DiffBitsArray mVal;
};

// Python 用のタイプ定義
PyTypeObject DiffBitsArray_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyDiffBitsArray::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<DiffBitsArray_Object*>(self);
  obj->mVal.~DiffBitsArray();
  Py_TYPE(self)->tp_free(self);
}

// hash 関数
Py_hash_t
hash_func(
  PyObject* self
)
{
  auto& val = PyDiffBitsArray::_get_ref(self);
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
  auto& val = PyDiffBitsArray::_get_ref(self);
  try {
    if ( PyDiffBitsArray::Check(other) ) {
      auto& val2 = PyDiffBitsArray::_get_ref(other);
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
  auto& val = PyDiffBitsArray::_get_ref(self);
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

PyObject*
dbits(
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
  auto& val = PyDiffBitsArray::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.dbits(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
dbits_union(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyDiffBitsArray::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.dbits_union());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_slice(
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
  auto& val = PyDiffBitsArray::_get_ref(self);
  try {
    return PyDiffBits::ToPyObject(val.get_slice(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
masking(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "mask",
    nullptr
  };
  unsigned long mask;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &mask) ) {
    return nullptr;
  }
  auto& val = PyDiffBitsArray::_get_ref(self);
  try {
    return PyDiffBitsArray::ToPyObject(val.masking(mask));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
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
  auto& val = PyDiffBitsArray::_get_ref(self);
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
add_output(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "output",
    "dbits",
    nullptr
  };
  unsigned long output;
  unsigned long dbits;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "kk",
                                    const_cast<char**>(kwlist),
                                    &output,
                                    &dbits) ) {
    return nullptr;
  }
  auto& val = PyDiffBitsArray::_get_ref(self);
  try {
    val.add_output(output, dbits);
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
add_pat(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "dbits",
    "pos",
    nullptr
  };
  PyObject* dbits_obj = nullptr;
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!k",
                                    const_cast<char**>(kwlist),
                                    PyDiffBits::_typeobject(), &dbits_obj,
                                    &pos) ) {
    return nullptr;
  }
  DiffBits dbits = {};
  if ( dbits_obj != nullptr ) {
    if ( !PyDiffBits::FromPyObject(dbits_obj, dbits) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to DiffBits");
      return nullptr;
    }
  }
  auto& val = PyDiffBitsArray::_get_ref(self);
  try {
    val.add_pat(dbits, pos);
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
sort(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyDiffBitsArray::_get_ref(self);
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

PyObject*
sorted(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyDiffBitsArray::_get_ref(self);
  try {
    return PyDiffBitsArray::ToPyObject(val.sorted());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

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
  auto& val = PyDiffBitsArray::_get_ref(self);
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
  {"dbits",
   reinterpret_cast<PyCFunction>(dbits),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return DIFF bits")},
  {"dbits_union",
   dbits_union,
   METH_NOARGS,
   PyDoc_STR("return union of DIFF bits of all outputs")},
  {"get_slice",
   reinterpret_cast<PyCFunction>(get_slice),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return bit slice")},
  {"masking",
   reinterpret_cast<PyCFunction>(masking),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return masked DiffBitsArray")},
  {"clear",
   clear,
   METH_NOARGS,
   PyDoc_STR("clear")},
  {"add_output",
   reinterpret_cast<PyCFunction>(add_output),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add output ID")},
  {"add_pat",
   reinterpret_cast<PyCFunction>(add_pat),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add bit slice")},
  {"sort",
   sort,
   METH_NOARGS,
   PyDoc_STR("do sorting")},
  {"sorted",
   sorted,
   METH_NOARGS,
   PyDoc_STR("return sorted DiffBitsArray")},
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
  auto& val = PyDiffBitsArray::_get_ref(self);
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


// @brief DiffBitsArray オブジェクトを使用可能にする．
bool
PyDiffBitsArray::init(
  PyObject* m
)
{
  DiffBitsArray_Type.tp_name = "DiffBitsArray";
  DiffBitsArray_Type.tp_basicsize = sizeof(DiffBitsArray_Object);
  DiffBitsArray_Type.tp_itemsize = 0;
  DiffBitsArray_Type.tp_dealloc = dealloc_func;
  DiffBitsArray_Type.tp_hash = hash_func;
  DiffBitsArray_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  DiffBitsArray_Type.tp_doc = PyDoc_STR("Python extended object for DiffBitsArray");
  DiffBitsArray_Type.tp_richcompare = richcompare_func;
  DiffBitsArray_Type.tp_methods = methods;
  DiffBitsArray_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "DiffBitsArray", &DiffBitsArray_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// DiffBitsArray を PyObject に変換する．
PyObject*
PyDiffBitsArray::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyDiffBitsArray::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<DiffBitsArray_Object*>(obj);
  new (&my_obj->mVal) DiffBitsArray(val);
  return obj;
}

// @brief PyObject が DiffBitsArray タイプか調べる．
bool
PyDiffBitsArray::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &DiffBitsArray_Type);
}

// @brief PyObject から DiffBitsArray を取り出す．
DiffBitsArray&
PyDiffBitsArray::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<DiffBitsArray_Object*>(obj);
  return my_obj->mVal;
}

// @brief DiffBitsArray を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDiffBitsArray::_typeobject()
{
  return &DiffBitsArray_Type;
}

END_NAMESPACE_DRUID
