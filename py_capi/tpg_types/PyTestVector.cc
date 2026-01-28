
/// @file PyTestVector.cc
/// @brief PyTestVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTestVector.h"
#include "pym/PyAssignList.h"
#include "pym/PyMt19937.h"
#include "pym/PyVal3.h"
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
struct TestVector_Object
{
  PyObject_HEAD
  TestVector mVal;
};

// Python 用のタイプ定義
PyTypeObject TestVector_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTestVector::init() 中で初期化する．
};

// richcompare 関数
PyObject*
richcompare_func(
  PyObject* self,
  PyObject* other,
  int op
)
{
  auto& val = PyTestVector::_get_ref(self);
  try {
    if ( PyTestVector::Check(other) ) {
      auto& val2 = PyTestVector::_get_ref(other);
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
  auto& val = PyTestVector::_get_ref(self);
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
has_aux_input(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTestVector::_get_ref(self);
  try {
    return PyBool_FromLong(val.has_aux_input());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ppi_val(
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
  auto& val = PyTestVector::_get_ref(self);
  try {
    return PyVal3::ToPyObject(val.ppi_val(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
input_val(
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
  auto& val = PyTestVector::_get_ref(self);
  try {
    return PyVal3::ToPyObject(val.input_val(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
dff_val(
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
  auto& val = PyTestVector::_get_ref(self);
  try {
    return PyVal3::ToPyObject(val.dff_val(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
aux_input_val(
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
  auto& val = PyTestVector::_get_ref(self);
  try {
    return PyVal3::ToPyObject(val.aux_input_val(pos));
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
  auto& val = PyTestVector::_get_ref(self);
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
  auto& val = PyTestVector::_get_ref(self);
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
  auto& val = PyTestVector::_get_ref(self);
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
  auto& val = PyTestVector::_get_ref(self);
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
set_from_assign_list(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "assign_list",
    nullptr
  };
  PyObject* assign_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyAssignList::_typeobject(), &assign_list_obj) ) {
    return nullptr;
  }
  AssignList assign_list;
  if ( assign_list_obj != nullptr ) {
    if ( !PyAssignList::FromPyObject(assign_list_obj, assign_list) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to AssignList");
      return nullptr;
    }
  }
  auto& val = PyTestVector::_get_ref(self);
  try {
    val.set_from_assign_list(assign_list);
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
set_ppi_val(
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
  auto& val = PyTestVector::_get_ref(self);
  try {
    val.set_ppi_val(pos, v);
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
set_input_val(
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
  auto& val = PyTestVector::_get_ref(self);
  try {
    val.set_input_val(pos, v);
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
set_dff_val(
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
  auto& val = PyTestVector::_get_ref(self);
  try {
    val.set_dff_val(pos, v);
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
set_aux_input_val(
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
  auto& val = PyTestVector::_get_ref(self);
  try {
    val.set_aux_input_val(pos, v);
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
  auto& val = PyTestVector::_get_ref(self);
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
  auto& val = PyTestVector::_get_ref(self);
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
                                    PyTestVector::_typeobject(), &right_obj) ) {
    return nullptr;
  }
  TestVector right;
  if ( right_obj != nullptr ) {
    if ( !PyTestVector::FromPyObject(right_obj, right) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TestVector");
      return nullptr;
    }
  }
  auto& val = PyTestVector::_get_ref(self);
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
                                    PyTestVector::_typeobject(), &right_obj) ) {
    return nullptr;
  }
  TestVector right;
  if ( right_obj != nullptr ) {
    if ( !PyTestVector::FromPyObject(right_obj, right) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TestVector");
      return nullptr;
    }
  }
  auto& val = PyTestVector::_get_ref(self);
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
                                    PyTestVector::_typeobject(), &right_obj) ) {
    return nullptr;
  }
  TestVector right;
  if ( right_obj != nullptr ) {
    if ( !PyTestVector::FromPyObject(right_obj, right) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TestVector");
      return nullptr;
    }
  }
  auto& val = PyTestVector::_get_ref(self);
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
                                    PyTestVector::_typeobject(), &right_obj) ) {
    return nullptr;
  }
  TestVector right;
  if ( right_obj != nullptr ) {
    if ( !PyTestVector::FromPyObject(right_obj, right) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TestVector");
      return nullptr;
    }
  }
  auto& val = PyTestVector::_get_ref(self);
  try {
    return PyTestVector::ToPyObject(val & right);
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
  {"has_aux_input",
   has_aux_input,
   METH_NOARGS,
   PyDoc_STR("True if having aux inputs")},
  {"ppi_val",
   reinterpret_cast<PyCFunction>(ppi_val),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return bit value of PPI")},
  {"input_val",
   reinterpret_cast<PyCFunction>(input_val),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return bit value of input")},
  {"dff_val",
   reinterpret_cast<PyCFunction>(dff_val),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return bit value of DFF")},
  {"aux_input_val",
   reinterpret_cast<PyCFunction>(aux_input_val),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return bit value of aux input")},
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
  {"set_from_assign_list",
   reinterpret_cast<PyCFunction>(set_from_assign_list),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set from AssignList")},
  {"set_ppi_val",
   reinterpret_cast<PyCFunction>(set_ppi_val),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set bit value of PPI")},
  {"set_input_val",
   reinterpret_cast<PyCFunction>(set_input_val),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set bit value of input")},
  {"set_dff_val",
   reinterpret_cast<PyCFunction>(set_dff_val),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set bit value of DFF")},
  {"set_aux_input_val",
   reinterpret_cast<PyCFunction>(set_aux_input_val),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set bit value of aux input")},
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
  auto& val = PyTestVector::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.vector_size());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_input_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTestVector::_get_ref(self);
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
get_dff_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTestVector::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.dff_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_ppi_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTestVector::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.ppi_num());
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
  {"input_num", get_input_num, nullptr, PyDoc_STR("input size(bits)"), nullptr},
  {"dff_num", get_dff_num, nullptr, PyDoc_STR("DFF size(bits)"), nullptr},
  {"ppi_num", get_ppi_num, nullptr, PyDoc_STR("PPI size(bits)"), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief TestVector オブジェクトを使用可能にする．
bool
PyTestVector::init(
  PyObject* m
)
{
  TestVector_Type.tp_name = "TestVector";
  TestVector_Type.tp_basicsize = sizeof(TestVector_Object);
  TestVector_Type.tp_itemsize = 0;
  TestVector_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TestVector_Type.tp_doc = PyDoc_STR("Python extended object for TestVector");
  TestVector_Type.tp_richcompare = richcompare_func;
  TestVector_Type.tp_methods = methods;
  TestVector_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "TestVector", &TestVector_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TestVector を PyObject に変換する．
PyObject*
PyTestVector::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTestVector::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TestVector_Object*>(obj);
  new (&my_obj->mVal) TestVector(val);
  return obj;
}

// PyObject を TestVector に変換する．
bool
PyTestVector::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyTestVector::Check(obj) ) {
    val = PyTestVector::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が TestVector タイプか調べる．
bool
PyTestVector::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TestVector_Type);
}

// @brief PyObject から TestVector を取り出す．
TestVector&
PyTestVector::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TestVector_Object*>(obj);
  return my_obj->mVal;
}

// @brief TestVector を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTestVector::_typeobject()
{
  return &TestVector_Type;
}

END_NAMESPACE_DRUID
