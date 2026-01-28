
/// @file PyInputVector.cc
/// @brief PyInputVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyInputVector.h"
#include "pym/PyAssignList.h"
#include "pym/PyVal3.h"
#include "pym/PyMt19937.h"
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
struct InputVector_Object
{
  PyObject_HEAD
  InputVector mVal;
};

// Python 用のタイプ定義
PyTypeObject InputVector_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyInputVector::init() 中で初期化する．
};

// hash 関数
Py_hash_t
hash_func(
  PyObject* self
)
{
  auto& val = PyInputVector::_get_ref(self);
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
  auto& val = PyInputVector::_get_ref(self);
  try {
    if ( PyInputVector::Check(other) ) {
      auto& val2 = PyInputVector::_get_ref(other);
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
val(
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
  auto& val = PyInputVector::_get_ref(self);
  try {
    return PyVal3::ToPyObject(val.val(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
x_count(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyInputVector::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.x_count());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
bin_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyInputVector::_get_ref(self);
  try {
    return PyString::ToPyObject(val.bin_str());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
hex_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyInputVector::_get_ref(self);
  try {
    return PyString::ToPyObject(val.hex_str());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
init(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyInputVector::_get_ref(self);
  try {
    val.init();
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
lshift(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "new_val",
    nullptr
  };
  PyObject* new_val_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyVal3::_typeobject(), &new_val_obj) ) {
    return nullptr;
  }
  Val3 new_val = Val3::_X;
  if ( new_val_obj != nullptr ) {
    if ( !PyVal3::FromPyObject(new_val_obj, new_val) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to Val3");
      return nullptr;
    }
  }
  auto& val = PyInputVector::_get_ref(self);
  try {
    val.lshift(new_val);
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
rshift(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "new_val",
    nullptr
  };
  PyObject* new_val_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyVal3::_typeobject(), &new_val_obj) ) {
    return nullptr;
  }
  Val3 new_val = Val3::_X;
  if ( new_val_obj != nullptr ) {
    if ( !PyVal3::FromPyObject(new_val_obj, new_val) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to Val3");
      return nullptr;
    }
  }
  auto& val = PyInputVector::_get_ref(self);
  try {
    val.rshift(new_val);
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
set_val(
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
  unsigned long pos;
  PyObject* v_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "kO!",
                                    const_cast<char**>(kwlist),
                                    &pos,
                                    PyVal3::_typeobject(), &v_obj) ) {
    return nullptr;
  }
  Val3 v = Val3::_X;
  if ( v_obj != nullptr ) {
    if ( !PyVal3::FromPyObject(v_obj, v) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to Val3");
      return nullptr;
    }
  }
  auto& val = PyInputVector::_get_ref(self);
  try {
    val.set_val(pos, v);
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
set_from_bin(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "bin_str",
    nullptr
  };
  const char* bin_str_tmp = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s",
                                    const_cast<char**>(kwlist),
                                    &bin_str_tmp) ) {
    return nullptr;
  }
  std::string bin_str;
  if ( bin_str_tmp != nullptr ) {
    bin_str = std::string(bin_str_tmp);
  }
  auto& val = PyInputVector::_get_ref(self);
  try {
    val.set_from_bin(bin_str);
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
set_from_hex(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "hex_str",
    nullptr
  };
  const char* hex_str_tmp = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s",
                                    const_cast<char**>(kwlist),
                                    &hex_str_tmp) ) {
    return nullptr;
  }
  std::string hex_str;
  if ( hex_str_tmp != nullptr ) {
    hex_str = std::string(hex_str_tmp);
  }
  auto& val = PyInputVector::_get_ref(self);
  try {
    val.set_from_hex(hex_str);
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
set_from_random(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "randgen",
    nullptr
  };
  PyObject* randgen_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyMt19937::_typeobject(), &randgen_obj) ) {
    return nullptr;
  }
  std::mt19937 randgen;
  if ( randgen_obj != nullptr ) {
    if ( !PyMt19937::FromPyObject(randgen_obj, randgen) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to std::mt19937");
      return nullptr;
    }
  }
  auto& val = PyInputVector::_get_ref(self);
  try {
    val.set_from_random(randgen);
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
fix_x_from_random(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "randgen",
    nullptr
  };
  PyObject* randgen_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyMt19937::_typeobject(), &randgen_obj) ) {
    return nullptr;
  }
  std::mt19937 randgen;
  if ( randgen_obj != nullptr ) {
    if ( !PyMt19937::FromPyObject(randgen_obj, randgen) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to std::mt19937");
      return nullptr;
    }
  }
  auto& val = PyInputVector::_get_ref(self);
  try {
    val.fix_x_from_random(randgen);
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
check_compatible(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "right",
    nullptr
  };
  PyObject* right_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyInputVector::_typeobject(), &right_obj) ) {
    return nullptr;
  }
  InputVector right;
  if ( right_obj != nullptr ) {
    if ( !PyInputVector::FromPyObject(right_obj, right) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to InputVector");
      return nullptr;
    }
  }
  auto& val = PyInputVector::_get_ref(self);
  try {
    return PyBool_FromLong(val && right);
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
check_containment(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "right",
    nullptr
  };
  PyObject* right_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyInputVector::_typeobject(), &right_obj) ) {
    return nullptr;
  }
  InputVector right;
  if ( right_obj != nullptr ) {
    if ( !PyInputVector::FromPyObject(right_obj, right) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to InputVector");
      return nullptr;
    }
  }
  auto& val = PyInputVector::_get_ref(self);
  try {
    return PyBool_FromLong(val < right);
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
check_containment_or_equal(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "right",
    nullptr
  };
  PyObject* right_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyInputVector::_typeobject(), &right_obj) ) {
    return nullptr;
  }
  InputVector right;
  if ( right_obj != nullptr ) {
    if ( !PyInputVector::FromPyObject(right_obj, right) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to InputVector");
      return nullptr;
    }
  }
  auto& val = PyInputVector::_get_ref(self);
  try {
    return PyBool_FromLong(val < right);
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
    "right",
    nullptr
  };
  PyObject* right_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyInputVector::_typeobject(), &right_obj) ) {
    return nullptr;
  }
  InputVector right;
  if ( right_obj != nullptr ) {
    if ( !PyInputVector::FromPyObject(right_obj, right) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to InputVector");
      return nullptr;
    }
  }
  auto& val = PyInputVector::_get_ref(self);
  try {
    return PyInputVector::ToPyObject(val & right);
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
  {"val",
   reinterpret_cast<PyCFunction>(val),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return bit value")},
  {"x_count",
   x_count,
   METH_NOARGS,
   PyDoc_STR("return X count")},
  {"bin_str",
   bin_str,
   METH_NOARGS,
   PyDoc_STR("return BIN string representation")},
  {"hex_str",
   hex_str,
   METH_NOARGS,
   PyDoc_STR("return HEX string representation")},
  {"init",
   init,
   METH_NOARGS,
   PyDoc_STR("initialize")},
  {"lshift",
   reinterpret_cast<PyCFunction>(lshift),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("do left-shift")},
  {"rshift",
   reinterpret_cast<PyCFunction>(rshift),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("do right-shift")},
  {"set_val",
   reinterpret_cast<PyCFunction>(set_val),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set bit value")},
  {"set_from_bin",
   reinterpret_cast<PyCFunction>(set_from_bin),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set from BIN string")},
  {"set_from_hex",
   reinterpret_cast<PyCFunction>(set_from_hex),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set from HEX string")},
  {"set_from_random",
   reinterpret_cast<PyCFunction>(set_from_random),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set value from random generator")},
  {"fix_x_from_random",
   reinterpret_cast<PyCFunction>(fix_x_from_random),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("fix X value from random generator")},
  {"check_compatible",
   reinterpret_cast<PyCFunction>(check_compatible),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return True if both vectors are compatible")},
  {"check_containment",
   reinterpret_cast<PyCFunction>(check_containment),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return True if self is contained in right")},
  {"check_containment_or_equal",
   reinterpret_cast<PyCFunction>(check_containment_or_equal),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return True if self is contained in right or equal")},
  {"merge",
   reinterpret_cast<PyCFunction>(merge),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("merge")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
get_vector_size(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyInputVector::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.len());
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
  {"vector_size", get_vector_size, nullptr, PyDoc_STR("vector size(bits)"), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief InputVector オブジェクトを使用可能にする．
bool
PyInputVector::init(
  PyObject* m
)
{
  InputVector_Type.tp_name = "InputVector";
  InputVector_Type.tp_basicsize = sizeof(InputVector_Object);
  InputVector_Type.tp_itemsize = 0;
  InputVector_Type.tp_hash = hash_func;
  InputVector_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  InputVector_Type.tp_doc = PyDoc_STR("Python extended object for InputVector");
  InputVector_Type.tp_richcompare = richcompare_func;
  InputVector_Type.tp_methods = methods;
  InputVector_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "InputVector", &InputVector_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// InputVector を PyObject に変換する．
PyObject*
PyInputVector::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyInputVector::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<InputVector_Object*>(obj);
  new (&my_obj->mVal) InputVector(val);
  return obj;
}

// PyObject を InputVector に変換する．
bool
PyInputVector::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyInputVector::Check(obj) ) {
    val = PyInputVector::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が InputVector タイプか調べる．
bool
PyInputVector::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &InputVector_Type);
}

// @brief PyObject から InputVector を取り出す．
InputVector&
PyInputVector::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<InputVector_Object*>(obj);
  return my_obj->mVal;
}

// @brief InputVector を表すオブジェクトの型定義を返す．
PyTypeObject*
PyInputVector::_typeobject()
{
  return &InputVector_Type;
}

END_NAMESPACE_DRUID
