
/// @file PyDtpgResult.cc
/// @brief Python DtpgResult の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PyDtpgResult.h"
#include "PyFaultStatus.h"
#include "PyTestVector.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DtpgResultObject
{
  PyObject_HEAD
  DtpgResult mVal;
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
  auto self = type->tp_alloc(type, 0);
  // auto dtpgresult_obj = reinterpret_cast<DtpgResultObject*>(self);
  // 必要なら dtpgresult_obj->mVal の初期化を行う．
  return self;
}

// 終了関数
void
DtpgResult_dealloc(
  PyObject* self
)
{
  auto dtpgresult_obj = reinterpret_cast<DtpgResultObject*>(self);
  auto& result = dtpgresult_obj->mVal;
  // デストラクタを明示的に呼び出す．
  result.~DtpgResult();
  Py_TYPE(self)->tp_free(self);
}

// 初期化関数(__init__()相当)
int
DtpgResult_init(
  PyObject* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  PyObject* arg_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &arg_obj) ) {
    return -1;
  }
  if ( PyFaultStatus::_check(arg_obj) ) {
    auto fault_status = PyFaultStatus::_get(arg_obj);
    PyDtpgResult::_put(self, DtpgResult{fault_status});
  }
  else if ( PyTestVector::_check(arg_obj) ) {
    auto& tv = PyTestVector::_get(arg_obj);
    PyDtpgResult::_put(self, DtpgResult{tv});
  }
  else {
    PyErr_SetString(PyExc_TypeError,
		    "1st argument should be either 'FaultStatus' or 'TestVector'");
    return -1;
  }

  return 0;
}

// メソッド定義
PyMethodDef DtpgResult_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// status を返す．
PyObject*
DtpgResult_status(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& result = PyDtpgResult::_get(self);
  auto status = result.status();
  return PyFaultStatus::ToPyObject(status);
}

// testvector を返す．
PyObject*
DtpgResult_testvector(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& result = PyDtpgResult::_get(self);
  auto& tv = result.testvector();
  return PyTestVector::ToPyObject(tv);
}

// getset メソッド定義
PyGetSetDef DtpgResult_getsetters[] = {
  {"status", DtpgResult_status, nullptr, PyDoc_STR("Fault Status")},
  {"testvector", DtpgResult_testvector, nullptr, PyDoc_STR("Test Vector")},
  {nullptr, nullptr, nullptr, nullptr}
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
  DtpgResultType.tp_doc = PyDoc_STR("DtpgResult objects");
  DtpgResultType.tp_methods = DtpgResult_methods;
  DtpgResultType.tp_getset = DtpgResult_getsetters;
  DtpgResultType.tp_init = DtpgResult_init;
  DtpgResultType.tp_new = DtpgResult_new;
  if ( PyType_Ready(&DtpgResultType) < 0 ) {
    return false;
  }

  // 型オブジェクトの登録
  auto type_obj = reinterpret_cast<PyObject*>(&DtpgResultType);
  Py_INCREF(type_obj);
  if ( PyModule_AddObject(m, "DtpgResult", type_obj) < 0 ) {
    Py_DECREF(type_obj);
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject から DtpgResult を取り出す．
bool
PyDtpgResult::FromPyObject(
  PyObject* obj,
  DtpgResult& val
)
{
  if ( !_check(obj) ) {
    PyErr_SetString(PyExc_TypeError, "object is not a DtpgResult type");
    return false;
  }
  val = _get(obj);
  return true;
}

// @brief DtpgResult を PyObject に変換する．
PyObject*
PyDtpgResult::ToPyObject(
  const DtpgResult& val
)
{
  auto obj = DtpgResult_new(_typeobject(), nullptr, nullptr);
  _put(obj, val);
  return obj;
}

// @brief PyObject が DtpgResult タイプか調べる．
bool
PyDtpgResult::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DtpgResult を表す PyObject から DtpgResult を取り出す．
const DtpgResult&
PyDtpgResult::_get(
  PyObject* obj
)
{
  auto dtpgresult_obj = reinterpret_cast<DtpgResultObject*>(obj);
  return dtpgresult_obj->mVal;
}

// @brief DtpgResult を表す PyObject に値を設定する．
void
PyDtpgResult::_put(
  PyObject* obj,
  const DtpgResult& val
)
{
  auto dtpgresult_obj = reinterpret_cast<DtpgResultObject*>(obj);
  dtpgresult_obj->mVal = val;
}

// @brief DtpgResult を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgResult::_typeobject()
{
  return &DtpgResultType;
}

END_NAMESPACE_DRUID
