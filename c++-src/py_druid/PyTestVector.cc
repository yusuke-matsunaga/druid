
/// @file PyTestVector.cc
/// @brief PyTestVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

extern PyObject* PyObj_from_Val3(Val3);
extern PyObject* PyObj_from_FaultType(FaultType);
extern bool Mt19937_from_PyObj(PyObject*, std::mt19937*&);
extern bool FaultType_from_PyObj(PyObject*, FaultType&);
extern bool Val3_from_PyObj(PyObject*, Val3&);

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TestVectorObject
{
  PyObject_HEAD
  TestVector* mTv;
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
  auto self = reinterpret_cast<TestVectorObject*>(type->tp_alloc(type, 0));
  self->mTv = nullptr;
  return reinterpret_cast<PyObject*>(self);
}

// 終了関数
void
TestVector_dealloc(
  TestVectorObject* self
)
{
  delete self->mTv;
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

// 初期化関数(__init__()相当)
int
TestVector_init(
  TestVectorObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  SizeType input_num = 0;
  SizeType dff_num = 0;
  PyObject* fault_type_obj = nullptr;
  static char* kwlist[] = {
    "",
    "",
    "fault_type",
    nullptr
  };
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|i$O", kwlist,
				    &input_num, &dff_num, &fault_type_obj) ) {
    return -1;
  }

  FaultType fault_type;
  if ( fault_type_obj == nullptr ) {
    fault_type = FaultType::StuckAt;
  }
  else if ( !FaultType_from_PyObj(fault_type_obj, fault_type) ) {
    return -1;
  }
  self->mTv = new TestVector{input_num, dff_num, fault_type};
  return 0;
}

// str() 関数
PyObject*
TestVector_str(
  PyObject* self
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto tmp_str = tv_obj->mTv->bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
TestVector_size(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  SizeType n = tv_obj->mTv->vector_size();
  return PyLong_FromLong(n);
}

PyObject*
TestVector_input_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  SizeType n = tv_obj->mTv->input_num();
  return PyLong_FromLong(n);
}

PyObject*
TestVector_dff_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  SizeType n = tv_obj->mTv->dff_num();
  return PyLong_FromLong(n);
}

PyObject*
TestVector_ppi_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  SizeType n = tv_obj->mTv->ppi_num();
  return PyLong_FromLong(n);
}

PyObject*
TestVector_has_aux_input(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  bool ans = tv_obj->mTv->has_aux_input();
  return PyBool_FromLong(ans);
}

PyObject*
TestVector_fault_type(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto fault_type = tv_obj->mTv->fault_type();
  return PyObj_from_FaultType(fault_type);
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
  auto val = tv_obj->mTv->val(pos);
  return PyObj_from_Val3(val);
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
  auto val = tv_obj->mTv->ppi_val(pos);
  return PyObj_from_Val3(val);
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
  auto val = tv_obj->mTv->input_val(pos);
  return PyObj_from_Val3(val);
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
  auto val = tv_obj->mTv->dff_val(pos);
  return PyObj_from_Val3(val);
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
  auto val = tv_obj->mTv->aux_input_val(pos);
  return PyObj_from_Val3(val);
}

PyObject*
TestVector_x_count(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto val = tv_obj->mTv->x_count();
  return PyLong_FromLong(val);
}

PyObject*
TestVector_bin_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto tmp_str = tv_obj->mTv->bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
TestVector_hex_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  auto tmp_str = tv_obj->mTv->hex_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
TestVector_init_method(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mTv->init();
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
  if ( !Val3_from_PyObj(obj, val) ) {
    return nullptr;
  }
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mTv->set_ppi_val(pos, val);
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
  if ( !Val3_from_PyObj(obj, val) ) {
    return nullptr;
  }
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mTv->set_input_val(pos, val);
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
  if ( !Val3_from_PyObj(obj, val) ) {
    return nullptr;
  }
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mTv->set_dff_val(pos, val);
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
  if ( !Val3_from_PyObj(obj, val) ) {
    return nullptr;
  }
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mTv->set_aux_input_val(pos, val);
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
  if ( !Mt19937_from_PyObj(obj, mt19937) ) {
    return nullptr;
  }
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mTv->set_from_random(*mt19937);
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
  if ( !Mt19937_from_PyObj(obj, mt19937) ) {
    return nullptr;
  }
  auto tv_obj = reinterpret_cast<TestVectorObject*>(self);
  tv_obj->mTv->fix_x_from_random(*mt19937);
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
  auto ans = tv1_obj->mTv->operator&(*tv2_obj->mTv);
  auto ans_obj = TestVector_new(&TestVectorType, nullptr, nullptr);
  reinterpret_cast<TestVectorObject*>(ans_obj)->mTv = new TestVector{ans};
  return ans_obj;
}

// 数値演算メソッド定義
PyNumberMethods TestVectorNumber = {
  .nb_and = TestVector_and,
};

END_NONAMESPACE


// @brief 'TestVector' オブジェクトを使用可能にする．
bool
PyInit_TestVector(
  PyObject* m
)
{
  TestVectorType.tp_name = "druid.TestVector";
  TestVectorType.tp_basicsize = sizeof(TestVectorObject);
  TestVectorType.tp_itemsize = 0;
  TestVectorType.tp_dealloc = reinterpret_cast<destructor>(TestVector_dealloc);
  TestVectorType.tp_flags = Py_TPFLAGS_DEFAULT;
  TestVectorType.tp_doc = PyDoc_STR("TestVector objects");
  TestVectorType.tp_methods = TestVector_methods;
  TestVectorType.tp_init = reinterpret_cast<initproc>(TestVector_init);
  TestVectorType.tp_new = TestVector_new;
  TestVectorType.tp_str = TestVector_str;
  TestVectorType.tp_as_number = &TestVectorNumber;

  if ( PyType_Ready(&TestVectorType) < 0 ) {
    return false;
  }
  Py_INCREF(&TestVectorType);
  if ( PyModule_AddObject(m, "TestVector", reinterpret_cast<PyObject*>(&TestVectorType)) < 0 ) {
    Py_DECREF(&TestVectorType);
    return false;
  }
  return true;
}

// @brief PyObject から TestVector を取り出す．
bool
TestVector_from_PyObj(
  PyObject* obj,
  TestVector& tv
)
{
  if ( !Py_IS_TYPE(obj, &TestVectorType) ) {
    PyErr_SetString(PyExc_ValueError, "object is not a TestVector type");
    return false;
  }
  auto tv_obj = reinterpret_cast<TestVectorObject*>(obj);
  tv = *(tv_obj->mTv);
  return true;
}

// @brief TestVector から PyObject を作り出す．
PyObject*
PyObj_from_TestVector(
  const TestVector& tv
)
{
  auto tv_obj = TestVector_new(&TestVectorType, nullptr, nullptr);
  reinterpret_cast<TestVectorObject*>(tv_obj)->mTv = new TestVector{tv};
  return tv_obj;
}

END_NAMESPACE_DRUID
