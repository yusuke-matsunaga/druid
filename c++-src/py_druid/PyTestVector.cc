
/// @file PyTestVector.cc
/// @brief Python TestVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PyTestVector.h"
#include "ym/PyMt19937.h"
#include "PyVal3.h"
#include "PyFaultType.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TestVectorObject
{
  PyObject_HEAD
  TestVector* mPtr;
};

// Python 用のタイプ定義
PyTypeObject TestVectorType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
TestVector_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  auto self = type->tp_alloc(type, 0);
  auto testvector_obj = reinterpret_cast<TestVectorObject*>(self);
  testvector_obj->mPtr = nullptr;
  return self;
}

// 終了関数
void
TestVector_dealloc(
  PyObject* self
)
{
  auto testvector_obj = reinterpret_cast<TestVectorObject*>(self);
  delete testvector_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

// 初期化関数(__init__()相当)
int
TestVector_init(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  SizeType input_num = 0;
  SizeType dff_num = 0;
  PyObject* fault_type_obj = nullptr;
  static const char* kwlist[] = {
    "",
    "",
    "fault_type",
    nullptr
  };
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|i$O",
				    const_cast<char**>(kwlist),
				    &input_num, &dff_num, &fault_type_obj) ) {
    return -1;
  }

  FaultType fault_type;
  if ( fault_type_obj == nullptr ) {
    fault_type = FaultType::StuckAt;
  }
  else if ( !PyFaultType::FromPyObject(fault_type_obj, fault_type) ) {
    return -1;
  }
  auto testvector_obj = reinterpret_cast<TestVectorObject*>(self);
  testvector_obj->mPtr = new TestVector{input_num, dff_num, fault_type};
  return 0;
}

// repr() 関数
PyObject*
TestVector_str(
  PyObject* self
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto tmp_str = tv_obj->mPtr->bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
TestVector_size(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  SizeType n = tv_obj->mPtr->vector_size();
  return PyLong_FromLong(n);
}

PyObject*
TestVector_input_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  SizeType n = tv_obj->mPtr->input_num();
  return PyLong_FromLong(n);
}

PyObject*
TestVector_dff_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  SizeType n = tv_obj->mPtr->dff_num();
  return PyLong_FromLong(n);
}

PyObject*
TestVector_ppi_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  SizeType n = tv_obj->mPtr->ppi_num();
  return PyLong_FromLong(n);
}

PyObject*
TestVector_has_aux_input(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  bool ans = tv_obj->mPtr->has_aux_input();
  return PyBool_FromLong(ans);
}

PyObject*
TestVector_fault_type(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto fault_type = tv_obj->mPtr->fault_type();
  return PyFaultType::ToPyObject(fault_type);
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
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto val = tv_obj->mPtr->val(pos);
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
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto val = tv_obj->mPtr->ppi_val(pos);
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
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto val = tv_obj->mPtr->input_val(pos);
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
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto val = tv_obj->mPtr->dff_val(pos);
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
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto val = tv_obj->mPtr->aux_input_val(pos);
  return PyVal3::ToPyObject(val);
}

PyObject*
TestVector_x_count(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto val = tv_obj->mPtr->x_count();
  return PyLong_FromLong(val);
}

PyObject*
TestVector_bin_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto tmp_str = tv_obj->mPtr->bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
TestVector_hex_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto tmp_str = tv_obj->mPtr->hex_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
TestVector_init_method(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mPtr->init();
  Py_RETURN_NONE;
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
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mPtr->set_ppi_val(pos, val);
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
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mPtr->set_input_val(pos, val);
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
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mPtr->set_dff_val(pos, val);
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
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mPtr->set_aux_input_val(pos, val);
  Py_RETURN_NONE;
}

PyObject*
TestVector_set_from_random(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &obj) ) {
    return nullptr;
  }

  std::mt19937* mt19937;
  if ( !PyMt19937::FromPyObject(obj, mt19937) ) {
    return nullptr;
  }
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mPtr->set_from_random(*mt19937);
  Py_RETURN_NONE;
}

PyObject*
TestVector_fix_x_from_random(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &obj) ) {
    return nullptr;
  }

  std::mt19937* mt19937;
  if ( !PyMt19937::FromPyObject(obj, mt19937) ) {
    return nullptr;
  }
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mPtr->fix_x_from_random(*mt19937);
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
  {"fault_type", TestVector_fault_type, METH_NOARGS,
   PyDoc_STR("return 'FaultType' of this vector")},
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
  auto tv1_obj = reinterpret_cast<TestVectorObject*>(self);
  auto tv2_obj = reinterpret_cast<TestVectorObject*>(other);
  auto ans = tv1_obj->mPtr->operator&(*tv2_obj->mPtr);
  auto ans_obj = TestVector_new(&TestVectorType, nullptr, nullptr);
  reinterpret_cast<TestVectorObject*>(ans_obj)->mPtr = new TestVector{ans};
  return ans_obj;
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
  TestVectorType.tp_doc = PyDoc_STR("TestVector objects");
  TestVectorType.tp_methods = TestVector_methods;
  TestVectorType.tp_init = TestVector_init;
  TestVectorType.tp_new = TestVector_new;
  TestVectorType.tp_str = TestVector_str;
  TestVectorType.tp_as_number = &TestVectorNumber;
  if ( PyType_Ready(&TestVectorType) < 0 ) {
    return false;
  }

  // 型オブジェクトの登録
  auto type_obj = reinterpret_cast<PyObject*>(&TestVectorType);
  Py_INCREF(type_obj);
  if ( PyModule_AddObject(m, "TestVector", type_obj) < 0 ) {
    Py_DECREF(type_obj);
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject から TestVector を取り出す．
bool
PyTestVector::FromPyObject(
  PyObject* obj,
  TestVector& val
)
{
  if ( !_check(obj) ) {
    PyErr_SetString(PyExc_TypeError, "object is not a TestVector type");
    return false;
  }
  val = _get(obj);
  return true;
}

// @brief TestVector を PyObject に変換する．
PyObject*
PyTestVector::ToPyObject(
  TestVector val
)
{
  auto obj = TestVector_new(_typeobject(), nullptr, nullptr);
  _put(obj, val);
  return obj;
}

// @brief PyObject が TestVector タイプか調べる．
bool
PyTestVector::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TestVector を表す PyObject から TestVector を取り出す．
const TestVector&
PyTestVector::_get(
  PyObject* obj
)
{
  auto testvector_obj = reinterpret_cast<TestVectorObject*>(obj);
  return *testvector_obj->mPtr;
}

// @brief TestVector を表す PyObject に値を設定する．
void
PyTestVector::_put(
  PyObject* obj,
  const TestVector& val
)
{
  auto testvector_obj = reinterpret_cast<TestVectorObject*>(obj);
  (*testvector_obj->mPtr) = val;
}

// @brief TestVector を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTestVector::_typeobject()
{
  return &TestVectorType;
}

END_NAMESPACE_DRUID
