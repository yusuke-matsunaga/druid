
/// @file PyDetCond.cc
/// @brief Python DetCond の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDetCond.h"
#include "pym/PyDetCondType.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DetCondObject
{
  PyObject_HEAD
  DetCond mVal;
};

// Python 用のタイプ定義
PyTypeObject DetCondType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DetCond_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'DetCond' is disabled.");
  return nullptr;
}

// 終了関数
void
DetCond_dealloc(
  PyObject* self
)
{
  auto testcond_obj = reinterpret_cast<DetCondObject*>(self);
  testcond_obj->mVal.~DetCond();
  Py_TYPE(self)->tp_free(self);
}

// メソッド定義
PyMethodDef DetCond_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
DetCond_type(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDetCond::_get_ref(self);
  return PyDetCondType::ToPyObject(val.type());
}

// getsetter 定義
PyGetSetDef DetCond_getsetters[] = {
  {"type", DetCond_type, nullptr, PyDoc_STR("type"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief 'DetCond' オブジェクトを使用可能にする．
bool
PyDetCond::init(
  PyObject* m
)
{
  DetCondType.tp_name = "DetCond";
  DetCondType.tp_basicsize = sizeof(DetCondObject);
  DetCondType.tp_itemsize = 0;
  DetCondType.tp_dealloc = DetCond_dealloc;
  DetCondType.tp_flags = Py_TPFLAGS_DEFAULT;
  DetCondType.tp_doc = PyDoc_STR("DetCond object");
  DetCondType.tp_methods = DetCond_methods;
  DetCondType.tp_getset = DetCond_getsetters;
  DetCondType.tp_new = DetCond_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "DetCond", &DetCondType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief DetCond を PyObject に変換する．
PyObject*
PyDetCond::Conv::operator()(
  const DetCond& val
)
{
  auto obj = DetCondType.tp_alloc(&DetCondType, 0);
  auto testcond_obj = reinterpret_cast<DetCondObject*>(obj);
  new (&testcond_obj->mVal) DetCond{val};
  return obj;
}

// @brief PyObject* から DetCond を取り出す．
bool
PyDetCond::Deconv::operator()(
  PyObject* obj,
  DetCond& val
)
{
  if ( PyDetCond::Check(obj) ) {
    val = PyDetCond::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が DetCond タイプか調べる．
bool
PyDetCond::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DetCond を表す PyObject から DetCond を取り出す．
DetCond&
PyDetCond::_get_ref(
  PyObject* obj
)
{
  auto testcond_obj = reinterpret_cast<DetCondObject*>(obj);
  return testcond_obj->mVal;
}

// @brief DetCond を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDetCond::_typeobject()
{
  return &DetCondType;
}

END_NAMESPACE_DRUID
