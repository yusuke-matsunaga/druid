
/// @file PyTestVector.cc
/// @brief Python TestVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTestVector.h"
#include "pym/PyBitVector.h"
#include "pym/PyVal3.h"
#include "pym/PyFaultType.h"
#include "pym/PyMt19937.h"
#include "pym/PyString.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TestVectorObject
{
  PyObject_HEAD
  TestVector mVal;
};

// Python 用のタイプ定義
PyTypeObject TestVectorType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
TestVector_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "input_num",
    "dff_num",
    "has_prev_state",
    "bits",
    nullptr
  };
  SizeType input_num = 0;
  SizeType dff_num = 0;
  int tmp = 0;
  PyObject* bits_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|ibO!",
				    const_cast<char**>(kwlist),
				    &input_num, &dff_num, &tmp,
				    PyBitVector::_typeobject(), &bits_obj) ) {
    return nullptr;
  }

  auto self = type->tp_alloc(type, 0);
  auto testvector_obj = reinterpret_cast<TestVectorObject*>(self);
  bool has_prev_state = static_cast<bool>(tmp);
  if ( bits_obj != nullptr ) {
    auto& bits = PyBitVector::_get_ref(bits_obj);
    new (&testvector_obj->mVal) TestVector(input_num, dff_num, has_prev_state, bits);
  }
  else {
    new (&testvector_obj->mVal) TestVector(input_num, dff_num, has_prev_state);
  }
  return self;
}

// 終了関数
void
TestVector_dealloc(
  PyObject* self
)
{
  auto testvector_obj = reinterpret_cast<TestVectorObject*>(self);
  testvector_obj->mVal.~TestVector();
  Py_TYPE(self)->tp_free(self);
}

// repr() 関数
PyObject*
TestVector_str(
  PyObject* self
)
{
  auto& tv = PyTestVector::_get_ref(self);
  return PyString::ToPyObject(tv.bin_str());
}

PyObject*
TestVector_size(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& tv = PyTestVector::_get_ref(self);
  auto val = tv.vector_size();
  return PyLong_FromLong(val);
}

PyObject*
TestVector_input_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& tv = PyTestVector::_get_ref(self);
  SizeType n = tv.input_num();
  return PyLong_FromLong(n);
}

PyObject*
TestVector_dff_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& tv = PyTestVector::_get_ref(self);
  SizeType n = tv.dff_num();
  return PyLong_FromLong(n);
}

PyObject*
TestVector_ppi_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& tv = PyTestVector::_get_ref(self);
  SizeType n = tv.ppi_num();
  return PyLong_FromLong(n);
}

PyObject*
TestVector_has_aux_input(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& tv = PyTestVector::_get_ref(self);
  bool ans = tv.has_aux_input();
  return PyBool_FromLong(ans);
}

PyObject*
TestVector_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto& tv = PyTestVector::_get_ref(self);
  auto val = tv.val(pos);
  return PyVal3::ToPyObject(val);
}

PyObject*
TestVector_ppi_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto& tv = PyTestVector::_get_ref(self);
  auto val = tv.ppi_val(pos);
  return PyVal3::ToPyObject(val);
}

PyObject*
TestVector_input_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto& tv = PyTestVector::_get_ref(self);
  auto val = tv.input_val(pos);
  return PyVal3::ToPyObject(val);
}

PyObject*
TestVector_dff_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto& tv = PyTestVector::_get_ref(self);
  auto val = tv.dff_val(pos);
  return PyVal3::ToPyObject(val);
}

PyObject*
TestVector_aux_input_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto& tv = PyTestVector::_get_ref(self);
  auto val = tv.aux_input_val(pos);
  return PyVal3::ToPyObject(val);
}

PyObject*
TestVector_x_count(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& tv = PyTestVector::_get_ref(self);
  auto val = tv.x_count();
  return PyLong_FromLong(val);
}

PyObject*
TestVector_bin_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& tv = PyTestVector::_get_ref(self);
  auto tmp_str = tv.bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
TestVector_hex_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& tv = PyTestVector::_get_ref(self);
  auto tmp_str = tv.hex_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
TestVector_init_method(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& tv = PyTestVector::_get_ref(self);
  tv.init();
  Py_RETURN_NONE;
}

PyObject*
TestVector_from_bin(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "",
    "input_num",
    "dff_num",
    "has_prev_state",
    nullptr
  };
  SizeType input_num = 0;
  SizeType dff_num = 0;
  int tmp = 0;
  char* bin_str = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "si|ib",
				    const_cast<char**>(kwlist),
				    &bin_str, &input_num, &dff_num, &tmp) ) {
    return nullptr;
  }
  bool has_prev_state = static_cast<bool>(tmp);
  auto tv = TestVector::from_bin(input_num, dff_num, has_prev_state, bin_str);
  return PyTestVector::ToPyObject(tv);
}

PyObject*
TestVector_from_hex(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "",
    "input_num",
    "dff_num",
    "has_prev_state",
    nullptr
  };
  SizeType input_num = 0;
  SizeType dff_num = 0;
  int tmp = 0;
  char* hex_str = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "si|ib",
				    const_cast<char**>(kwlist),
				    &hex_str, &input_num, &dff_num, &tmp) ) {
    return nullptr;
  }
  bool has_prev_state = static_cast<bool>(tmp);
  auto tv = TestVector::from_hex(input_num, dff_num, has_prev_state, hex_str);
  return PyTestVector::ToPyObject(tv);
}

PyObject*
TestVector_set_ppi_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "iO", &pos, &obj) ) {
    return nullptr;
  }
  Val3 val;
  if ( !PyVal3::FromPyObject(obj, val) ) {
    return nullptr;
  }
  auto& tv = PyTestVector::_get_ref(self);
  tv.set_ppi_val(pos, val);
  Py_RETURN_NONE;
}

PyObject*
TestVector_set_input_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "iO", &pos, &obj) ) {
    return nullptr;
  }
  Val3 val;
  if ( !PyVal3::FromPyObject(obj, val) ) {
    return nullptr;
  }
  auto& tv = PyTestVector::_get_ref(self);
  tv.set_input_val(pos, val);
  Py_RETURN_NONE;
}

PyObject*
TestVector_set_dff_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "iO", &pos, &obj) ) {
    return nullptr;
  }
  Val3 val;
  if ( !PyVal3::FromPyObject(obj, val) ) {
    return nullptr;
  }
  auto& tv = PyTestVector::_get_ref(self);
  tv.set_dff_val(pos, val);
  Py_RETURN_NONE;
}

PyObject*
TestVector_set_aux_input_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "iO", &pos, &obj) ) {
    return nullptr;
  }
  Val3 val;
  if ( !PyVal3::FromPyObject(obj, val) ) {
    return nullptr;
  }
  auto& tv = PyTestVector::_get_ref(self);
  tv.set_aux_input_val(pos, val);
  Py_RETURN_NONE;
}

PyObject*
TestVector_set_from_random(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!", PyMt19937::_typeobject(), &obj) ) {
    return nullptr;
  }

  auto& mt19937 = PyMt19937::_get_ref(obj);
  auto& tv = PyTestVector::_get_ref(self);
  tv.set_from_random(mt19937);
  Py_RETURN_NONE;
}

PyObject*
TestVector_fix_x_from_random(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!", PyMt19937::_typeobject(), &obj) ) {
    return nullptr;
  }

  auto& mt19937 = PyMt19937::_get_ref(obj);
  auto& tv = PyTestVector::_get_ref(self);
  tv.fix_x_from_random(mt19937);
  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef TestVector_methods[] = {
  {"size", TestVector_size, METH_NOARGS,
   PyDoc_STR("returns the size")},
  {"input_num", TestVector_input_num, METH_NOARGS,
   PyDoc_STR("returns a number of inputs")},
  {"dff_num", TestVector_dff_num, METH_NOARGS,
   PyDoc_STR("returns a number of D-FFs")},
  {"ppi_num", TestVector_ppi_num, METH_NOARGS,
   PyDoc_STR("returns a number of PPIs")},
  {"has_aux_input", TestVector_has_aux_input, METH_NOARGS,
   PyDoc_STR("returns True if having auxially inputs")},
  {"val", TestVector_val, METH_VARARGS,
   PyDoc_STR("returns a value of the specified bit")},
  {"ppi_val", TestVector_ppi_val, METH_VARARGS,
   PyDoc_STR("returns a value of the specified PPI")},
  {"input_val", TestVector_input_val, METH_VARARGS,
   PyDoc_STR("returns a value of the specified input")},
  {"dff_val", TestVector_dff_val, METH_VARARGS,
   PyDoc_STR("returns a value of the specified DFF")},
  {"aux_input_val", TestVector_aux_input_val, METH_VARARGS,
   PyDoc_STR("returns a value of the specified auxially input")},
  {"x_count", TestVector_x_count, METH_NOARGS,
   PyDoc_STR("returns a number of 'X' bits")},
  {"bin_str", TestVector_bin_str, METH_NOARGS,
   PyDoc_STR("returns a string representation in binary format")},
  {"hex_str", TestVector_hex_str, METH_NOARGS,
   PyDoc_STR("returns a string representation in hexadecimal format")},
  {"init", TestVector_init_method, METH_NOARGS,
   PyDoc_STR("returns a string representation in hexadecimal format")},
  {"from_bin", reinterpret_cast<PyCFunction>(TestVector_from_bin),
   METH_VARARGS | METH_KEYWORDS | METH_STATIC,
   PyDoc_STR("create new object from BIN string")},
  {"from_hex", reinterpret_cast<PyCFunction>(TestVector_from_hex),
   METH_VARARGS | METH_KEYWORDS | METH_STATIC,
   PyDoc_STR("create new object from HEX string")},
  {"set_ppi_val", TestVector_set_ppi_val, METH_VARARGS,
   PyDoc_STR("set value of the specified PPI")},
  {"set_input_val", TestVector_set_input_val, METH_VARARGS,
   PyDoc_STR("set value of the specified input")},
  {"set_dff_val", TestVector_set_dff_val, METH_VARARGS,
   PyDoc_STR("set value of the specified DFF")},
  {"set_aux_input_val", TestVector_set_aux_input_val, METH_VARARGS,
   PyDoc_STR("set value of the specified auxially input")},
  {"set_from_random", TestVector_set_from_random, METH_VARARGS,
   PyDoc_STR("set value randomly")},
  {"fix_x_from_random", TestVector_fix_x_from_random, METH_VARARGS,
   PyDoc_STR("fix 'X' values randomly")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
TestVector_and(
  PyObject* self,
  PyObject* other
)
{
  if ( !Py_IS_TYPE(self, &TestVectorType) ||
       !Py_IS_TYPE(other, &TestVectorType) ) {
    // 型が合わない．
    PyErr_SetString(PyExc_TypeError, "both arguments sould be TestVector type");
    return nullptr;
  }
  auto& tv1 = PyTestVector::_get_ref(self);
  auto& tv2 = PyTestVector::_get_ref(other);
  auto ans = tv1 & tv2;
  return PyTestVector::ToPyObject(ans);
}

// 数値演算メソッド定義
PyNumberMethods TestVectorNumber = {
  .nb_and = TestVector_and,
};

END_NONAMESPACE


// @brief 'TestVector' オブジェクトを使用可能にする．
bool
PyTestVector::init(
  PyObject* m
)
{
  TestVectorType.tp_name = "TestVector";
  TestVectorType.tp_basicsize = sizeof(TestVectorObject);
  TestVectorType.tp_itemsize = 0;
  TestVectorType.tp_dealloc = TestVector_dealloc;
  TestVectorType.tp_flags = Py_TPFLAGS_DEFAULT;
  TestVectorType.tp_doc = PyDoc_STR("TestVector object");
  TestVectorType.tp_methods = TestVector_methods;
  TestVectorType.tp_new = TestVector_new;
  TestVectorType.tp_str = TestVector_str;
  TestVectorType.tp_as_number = &TestVectorNumber;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "TestVector", &TestVectorType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief TestVector を PyObject に変換する．
PyObject*
PyTestVector::Conv::operator()(
  const TestVector& val
)
{
  auto obj = TestVectorType.tp_alloc(&TestVectorType, 0);
  auto testvector_obj = reinterpret_cast<TestVectorObject*>(obj);
  new (&testvector_obj->mVal) TestVector(val);
  return obj;
}

// @brief PyObject* を TestVector に変換する
bool
PyTestVector::Deconv::operator()(
  PyObject* obj,
  TestVector& val
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
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TestVector を表す PyObject から TestVector を取り出す．
TestVector&
PyTestVector::_get_ref(
  PyObject* obj
)
{
  auto testvector_obj = reinterpret_cast<TestVectorObject*>(obj);
  return testvector_obj->mVal;
}

// @brief TestVector を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTestVector::_typeobject()
{
  return &TestVectorType;
}

END_NAMESPACE_DRUID
