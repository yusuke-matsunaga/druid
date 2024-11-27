
/// @file PyTestCond.cc
/// @brief Python TestCond の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTestCond.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TestCondObject
{
  PyObject_HEAD
  TestCond* mVal;
};

// Python 用のタイプ定義
PyTypeObject TestCondType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
TestCond_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'TestCond' is disabled.");
  return nullptr;
}

// 終了関数
void
TestCond_dealloc(
  PyObject* self
)
{
  auto testcond_obj = reinterpret_cast<TestCondObject*>(self);
  delete testcond_obj->mVal;
  Py_TYPE(self)->tp_free(self);
}

// メソッド定義
PyMethodDef TestCond_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief 'TestCond' オブジェクトを使用可能にする．
bool
PyTestCond::init(
  PyObject* m
)
{
  TestCondType.tp_name = "TestCond";
  TestCondType.tp_basicsize = sizeof(TestCondObject);
  TestCondType.tp_itemsize = 0;
  TestCondType.tp_dealloc = TestCond_dealloc;
  TestCondType.tp_flags = Py_TPFLAGS_DEFAULT;
  TestCondType.tp_doc = PyDoc_STR("TestCond object");
  TestCondType.tp_methods = TestCond_methods;
  TestCondType.tp_new = TestCond_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "TestCond", &TestCondType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief TestCond を PyObject に変換する．
PyObject*
PyTestCond::ToPyObject(
  const TestCond& val
)
{
  auto obj = TestCondType.tp_alloc(&TestCondType, 0);
  auto testcond_obj = reinterpret_cast<TestCondObject*>(obj);
  testcond_obj->mVal = new TestCond{val};
  return obj;
}

// @brief PyObject が TestCond タイプか調べる．
bool
PyTestCond::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TestCond を表す PyObject から TestCond を取り出す．
TestCond
PyTestCond::Get(
  PyObject* obj
)
{
  auto testcond_obj = reinterpret_cast<TestCondObject*>(obj);
  return *testcond_obj->mVal;
}

// @brief TestCond を表す PyObject から TestCond を取り出す．
const TestCond&
PyTestCond::_Get(
  PyObject* obj
)
{
  auto testcond_obj = reinterpret_cast<TestCondObject*>(obj);
  return *testcond_obj->mVal;
}

// @brief TestCond を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTestCond::_typeobject()
{
  return &TestCondType;
}

END_NAMESPACE_DRUID
