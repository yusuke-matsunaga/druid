
/// @file PyDtpgEngine.cc
/// @brief Python DtpgEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PyDtpgEngine.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DtpgEngineObject
{
  PyObject_HEAD
  DtpgEngine* mPtr;
};

// Python 用のタイプ定義
PyTypeObject DtpgEngineType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DtpgEngine_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  auto self = type->tp_alloc(type, 0);
  auto dtpgengine_obj = reinterpret_cast<DtpgEngineObject*>(self);
  dtpgengine_obj->mPtr = nullptr;
  return self;
}

// 終了関数
void
DtpgEngine_dealloc(
  PyObject* self
)
{
  auto dtpgengine_obj = reinterpret_cast<DtpgEngineObject*>(self);
  delete dtpgengine_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
DtpgEngine_new_ffr(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "network",
    "fault_type",
    "ffr",
    "just_type",
    "solver_type",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_type_obj = nullptr;
  int ffr_id = -1;
  const char* just_type_str = nullptr;
  const char* solver_type_str = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|$!O!Oiss",
				    const_cast<char**>(kwlist),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    PyFaultType::_typeobject(), &fault_type_obj,
				    &ffr_id,
				    &just_type_str,
				    &solver_type_str) ) {
    return nullptr;
  }
  if ( network_obj == nullptr ) {
    PyErr_SetString(PyExc_TypeError, "'network' keyword should be specified");
    return nullptr;
  }
  if ( fault_type_obj == nullptr ) {
    PyErr_SetString(PyExc_TypeError, "'fault_type' keyword should be specified");
    return nullptr;
  }
  if ( ffr_id == -1 ) {
    PyErr_SetString(PyExc_TypeError, "'ffr' keyword should be specified");
    return nullptr;
  }
  if ( just_type_str == nullptr ) {
    PyErr_SetString(PyExc_TypeError, "'just_type' keyword should be specified");
    return nullptr;
  }
  SatSolverType solver_type;
  if ( solver_type_str != nullptr ) {
    solver_type = SatSolverType{solver_type_str};
  }
  auto obj = DtpgEngine_new(&DtpgEngineTypf, nullptr, nullptr);
  auto dtpg_obj = reinterpret_cast<DtpgEngineObject*>(obj);
  auto network_p = PyTpgNetwork::_get(network_obj);
  auto fault_type = PyFaultType::_get(fault_type_obj);
  auto& ffr = network_p->ffr(ffr_id);
  dtpg_obj->mPtr = new DtpgFFR(*network_p, fault_type,
			       ffr, just_type_str,
			       solver_type);
  return obj;
}

PyObject*
DtpgEngine_gen_pattern(
  PyObject* self,
  PyObject* args
)
{
  PyObject* fault_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "!O", PyTpgFault::_typeobject(), &fault_obj) ) {
    return nullptr;
  }
  auto fault = PyTpgFault::_get(fault_obj);
  auto dtpg_obj = reinterpret_cast<DtpgEngineObject*>(self);
  auto result = dtpg_obj->get_pattern(fault);
  return PyDtpgResult::ToPyObject(result);
}

// メソッド定義
PyMethodDef DtpgEngine_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief 'DtpgEngine' オブジェクトを使用可能にする．
bool
PyDtpgEngine::init(
  PyObject* m
)
{
  DtpgEngineType.tp_name = "DtpgEngine";
  DtpgEngineType.tp_basicsize = sizeof(DtpgEngineObject);
  DtpgEngineType.tp_itemsize = 0;
  DtpgEngineType.tp_dealloc = DtpgEngine_dealloc;
  DtpgEngineType.tp_flags = Py_TPFLAGS_DEFAULT;
  DtpgEngineType.tp_doc = PyDoc_STR("DtpgEngine objects");
  DtpgEngineType.tp_methods = DtpgEngine_methods;
  DtpgEngineType.tp_new = DtpgEngine_new;
  if ( PyType_Ready(&DtpgEngineType) < 0 ) {
    return false;
  }

  // 型オブジェクトの登録
  auto type_obj = reinterpret_cast<PyObject*>(&DtpgEngineType);
  Py_INCREF(type_obj);
  if ( PyModule_AddObject(m, "DtpgEngine", type_obj) < 0 ) {
    Py_DECREF(type_obj);
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject から DtpgEngine を取り出す．
bool
PyDtpgEngine::FromPyObject(
  PyObject* obj,
  DtpgEngine*& val
)
{
  if ( !_check(obj) ) {
    PyErr_SetString(PyExc_TypeError, "object is not a DtpgEngine type");
    return false;
  }
  val = _get(obj);
  return true;
}

// @brief PyObject が DtpgEngine タイプか調べる．
bool
PyDtpgEngine::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DtpgEngine を表す PyObject から DtpgEngine を取り出す．
DtpgEngine*
PyDtpgEngine::_get(
  PyObject* obj
)
{
  auto dtpgengine_obj = reinterpret_cast<DtpgEngineObject*>(obj);
  return dtpgengine_obj->mPtr;
}

// @brief DtpgEngine を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgEngine::_typeobject()
{
  return &DtpgEngineType;
}

END_NAMESPACE_DRUID
