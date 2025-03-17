
/// @file PyDtpgResult.cc
/// @brief Python DtpgResult の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDtpgResult.h"
#include "pym/PyFaultStatus.h"
#include "pym/PyTestVector.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DtpgResultObject
{
  PyObject_HEAD
  DtpgResult* mVal;
};

// Python 用のタイプ定義
PyTypeObject DtpgResultType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DtpgResult_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'DtpgResult' is disabled.");
  return nullptr;
}

// 終了関数
void
DtpgResult_dealloc(
  PyObject* self
)
{
  auto result_obj = reinterpret_cast<DtpgResultObject*>(self);
  delete result_obj->mVal;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
DtpgResult_detected(
  PyObject* Py_UNUSED(self),
  PyObject* args
)
{
  PyObject* tv_obj;
  if ( !PyArg_ParseTuple(args, "O!",
			 PyTestVector::_typeobject(), &tv_obj) ) {
    return nullptr;
  }
  auto tv = PyTestVector::Get(tv_obj);
  auto type = PyDtpgResult::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto result_obj = reinterpret_cast<DtpgResultObject*>(obj);
  auto result = DtpgResult::detected(tv);
  return PyDtpgResult::ToPyObject(result);
}

PyObject*
DtpgResult_untestable(
  PyObject* Py_UNUSED(self),
  PyObject* Py_UNUSED(args)
)
{
  auto type = PyDtpgResult::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto result_obj = reinterpret_cast<DtpgResultObject*>(obj);
  auto result = DtpgResult::untestable();
  return PyDtpgResult::ToPyObject(result);
}

PyObject*
DtpgResult_undetected(
  PyObject* Py_UNUSED(self),
  PyObject* Py_UNUSED(args)
)
{
  auto type = PyDtpgResult::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto result_obj = reinterpret_cast<DtpgResultObject*>(obj);
  auto result = DtpgResult::undetected();
  return PyDtpgResult::ToPyObject(result);
}

// メソッド定義
PyMethodDef DtpgResult_methods[] = {
  {"detected", DtpgResult_detected,
   METH_VARARGS | METH_STATIC,
   PyDoc_STR("new 'detected' result")},
  {"untestable", DtpgResult_untestable,
   METH_NOARGS | METH_STATIC,
   PyDoc_STR("new 'untestable' result")},
  {"undetected", DtpgResult_undetected,
   METH_NOARGS | METH_STATIC,
   PyDoc_STR("new 'undetected' result")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
DtpgResult_status(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto result_obj = reinterpret_cast<DtpgResultObject*>(self);
  auto& result = *result_obj->mVal;
  return PyFaultStatus::ToPyObject(result.status());
}

PyObject*
DtpgResult_testvector(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto result_obj = reinterpret_cast<DtpgResultObject*>(self);
  auto& result = *result_obj->mVal;
  return PyTestVector::ToPyObject(result.testvector());
}

PyGetSetDef DtpgResult_getset[] = {
  {"status", DtpgResult_status, nullptr,
   PyDoc_STR("status"), nullptr},
  {"testvector", DtpgResult_testvector, nullptr,
   PyDoc_STR("testvector"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief 'DtpgResult' オブジェクトを使用可能にする．
bool
PyDtpgResult::init(
  PyObject* m
)
{
  DtpgResultType.tp_name = "DtpgResult";
  DtpgResultType.tp_basicsize = sizeof(DtpgResultObject);
  DtpgResultType.tp_itemsize = 0;
  DtpgResultType.tp_dealloc = DtpgResult_dealloc;
  DtpgResultType.tp_flags = Py_TPFLAGS_DEFAULT;
  DtpgResultType.tp_doc = PyDoc_STR("DtpgResult object");
  DtpgResultType.tp_methods = DtpgResult_methods;
  DtpgResultType.tp_getset = DtpgResult_getset;
  DtpgResultType.tp_new = DtpgResult_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "DtpgResult", &DtpgResultType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief DtpgResult を表す PyObject を作る．
PyObject*
PyDtpgResult::ToPyObject(
  const DtpgResult& val
)
{
  auto obj = DtpgResultType.tp_alloc(&DtpgResultType, 0);
  auto result_obj = reinterpret_cast<DtpgResultObject*>(obj);
  result_obj->mVal = new DtpgResult{val};
  return obj;
}

// @brief PyObject が DtpgResult タイプか調べる．
bool
PyDtpgResult::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DtpgResult を表す PyObject から DtpgResult を取り出す．
const DtpgResult&
PyDtpgResult::Get(
  PyObject* obj
)
{
  auto result_obj = reinterpret_cast<DtpgResultObject*>(obj);
  return *result_obj->mVal;
}

// @brief DtpgResult を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgResult::_typeobject()
{
  return &DtpgResultType;
}

END_NAMESPACE_DRUID
