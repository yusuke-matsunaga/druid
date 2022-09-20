
/// @file PyFaultType.cc
/// @brief PyFaultType の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "FaultType.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct FaultTypeObject
{
  PyObject_HEAD
  FaultType mVal;
};

// 定数 "stuck-at"
PyObject* FaultType_StuckAt = nullptr;

// 定数 "transition-delay"
PyObject* FaultType_TransitionDelay = nullptr;

// 生成関数
PyObject*
FaultType_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  auto self = reinterpret_cast<FaultTypeObject*>(type->tp_alloc(type, 0));
  // 必要なら self の初期化を行う．
  return reinterpret_cast<PyObject*>(self);
}

// 終了関数
void
FaultType_dealloc(
  FaultTypeObject* self
)
{
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

// 初期化関数(__init__()相当)
int
FaultType_init(
  FaultTypeObject* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  // 変換ルール
  // - 文字列型
  //   * "stuck-at" -> FaultType::StuckAt
  //   * "transition-delay" -> FaultType::TransitionDelay
  // それ以外は TypeError

  const char* val_str = nullptr;
  if ( !PyArg_ParseTuple(args, "s", &val_str) ) {
    return -1;
  }
  if ( strcmp(val_str, "stuck-at") == 0 ||
       strcmp(val_str, "s-a") == 0 ) {
    self->mVal = FaultType::StuckAt;
  }
  else if ( strcmp(val_str, "transition-delay") == 0 ||
	    strcmp(val_str, "t-d") == 0 ) {
    self->mVal = FaultType::TransitionDelay;
  }
  else {
    PyErr_SetString(PyExc_ValueError,
		    "1st argument should be either 'stuck-at' or 'transition-delay'");
    return -1;
  }
  return 0;
}

// str() 関数
PyObject*
FaultType_str(
  PyObject* self
)
{
  auto ft_obj = reinterpret_cast<FaultTypeObject*>(self);
  const char* tmp_str = nullptr;
  switch ( ft_obj->mVal ) {
  case FaultType::None: tmp_str = "---"; break;
  case FaultType::StuckAt: tmp_str = "stuck-at"; break;
  case FaultType::TransitionDelay: tmp_str = "transition-delay"; break;
  }
  return Py_BuildValue("s", tmp_str);
}

// メソッド定義
PyMethodDef FaultType_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// Python 用のタイプ定義
PyTypeObject FaultTypeType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

END_NONAMESPACE


// @brief 'FaultType' オブジェクトを使用可能にする．
bool
PyInit_FaultType(
  PyObject* m
)
{
  FaultTypeType.tp_name = "druid.FaultType";
  FaultTypeType.tp_basicsize = sizeof(FaultTypeObject);
  FaultTypeType.tp_itemsize = 0;
  FaultTypeType.tp_dealloc = reinterpret_cast<destructor>(FaultType_dealloc);
  FaultTypeType.tp_flags = Py_TPFLAGS_DEFAULT;
  FaultTypeType.tp_doc = PyDoc_STR("FaultType objects");
  FaultTypeType.tp_methods = FaultType_methods;
  FaultTypeType.tp_init = reinterpret_cast<initproc>(FaultType_init);
  FaultTypeType.tp_new = FaultType_new;
  FaultTypeType.tp_str = FaultType_str;
  if ( PyType_Ready(&FaultTypeType) < 0 ) {
    return false;
  }
  Py_INCREF(&FaultTypeType);
  if ( PyModule_AddObject(m, "FaultType", reinterpret_cast<PyObject*>(&FaultTypeType)) < 0 ) {
    Py_DECREF(&FaultTypeType);
    return false;
  }

  FaultType_StuckAt = FaultType_new(&FaultTypeType, nullptr, nullptr);
  reinterpret_cast<FaultTypeObject*>(FaultType_StuckAt)->mVal = FaultType::StuckAt;
  Py_INCREF(FaultType_StuckAt);
  if ( PyModule_AddObject(m, "FaultType_StuckAt", FaultType_StuckAt) < 0 ) {
    Py_DECREF(FaultType_StuckAt);
    return false;
  }

  FaultType_TransitionDelay = FaultType_new(&FaultTypeType, nullptr, nullptr);
  reinterpret_cast<FaultTypeObject*>(FaultType_TransitionDelay)->mVal = FaultType::TransitionDelay;
  Py_INCREF(FaultType_TransitionDelay);
  if ( PyModule_AddObject(m, "FaultType_TransitionDelay", FaultType_TransitionDelay) < 0 ) {
    Py_DECREF(FaultType_TransitionDelay);
    return false;
  }

  return true;
}

bool
FaultType_from_PyObj(
  PyObject* obj,
  FaultType& fault_type
)
{
  if ( !Py_IS_TYPE(obj, &FaultTypeType) ) {
    PyErr_SetString(PyExc_ValueError, "object is not a FaultType type");
    return false;
  }
  auto ft_obj = reinterpret_cast<FaultTypeObject*>(obj);
  fault_type = ft_obj->mVal;
  return true;
}

PyObject*
PyObj_from_FaultType(
  FaultType val
)
{
  auto ft_obj = FaultType_new(&FaultTypeType, nullptr, nullptr);
  reinterpret_cast<FaultTypeObject*>(ft_obj)->mVal = val;
  return ft_obj;
}

END_NAMESPACE_DRUID
