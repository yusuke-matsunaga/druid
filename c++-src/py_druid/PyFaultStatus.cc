
/// @file PyFaultStatus.cc
/// @brief Python FaultStatus の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PyFaultStatus.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct FaultStatusObject
{
  PyObject_HEAD
  FaultStatus mVal;
};

// Python 用のタイプ定義
PyTypeObject FaultStatusType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 定数 "Undetected"
PyObject* FaultStatus_Undetected = nullptr;

// 定数 "Detected"
PyObject* FaultStatus_Detected = nullptr;

// 定数 "Untestable"
PyObject* FaultStatus_Untestable = nullptr;


// 生成関数
PyObject*
FaultStatus_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  if ( type != &FaultStatusType ) {
    PyErr_SetString(PyExc_TypeError, "FaultStatus cannot be overloaded");
    return nullptr;
  }

  // 変換ルール
  // - 文字列型(大文字小文字は区別しない)
  //   * "undetected"|"u" -> FaultStatus::Undetected
  //   * "detected"|"d" -> FaultStatus::Detected
  //   * "untestable" -> FaultStatus::Untestable
  // それ以外は TypeError

  const char* val_str = nullptr;
  if ( !PyArg_ParseTuple(args, "s", &val_str) ) {
    return nullptr;
  }

  PyObject* obj = nullptr;
  if ( strcasecmp(val_str, "undetected") == 0 ||
       strcasecmp(val_str, "u") == 0 ) {
    obj = FaultStatus_Undetected;
  }
  else if ( strcasecmp(val_str, "detected") == 0 ||
	    strcasecmp(val_str, "d") == 0 ) {
    obj = FaultStatus_Detected;
  }
  else if ( strcasecmp(val_str, "untestable") == 0 ) {
    obj = FaultStatus_Untestable;
  }
  else {
    PyErr_SetString(PyExc_ValueError,
		    "1st argument should be either 'undetected', 'detected' or 'untestable'");
    return nullptr;
  }
  Py_INCREF(obj);
  return obj;
}

// 終了関数
void
FaultStatus_dealloc(
  PyObject* self
)
{
  // auto faultstatus_obj = reinterpret_cast<FaultStatusObject*>(self);
  // 必要なら faultstatus_obj->mVal の終了処理を行う．
  Py_TYPE(self)->tp_free(self);
}

// repr() 関数
PyObject*
FaultStatus_repr(
  PyObject* self
)
{
  auto val = PyFaultStatus::_get(self);
  // val から 文字列を作る．
  const char* tmp_str = str(val);
  return Py_BuildValue("s", tmp_str);
}

// メソッド定義
PyMethodDef FaultStatus_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// 比較関数
PyObject*
FaultStatus_richcmpfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PyFaultStatus::_check(self) &&
       PyFaultStatus::_check(other) ) {
    auto val1 = PyFaultStatus::_get(self);
    auto val2 = PyFaultStatus::_get(other);
    if ( op == Py_EQ ) {
      return PyBool_FromLong(val1 == val2);
    }
    if ( op == Py_NE ) {
      return PyBool_FromLong(val1 != val2);
    }
  }
  Py_INCREF(Py_NotImplemented);
  return Py_NotImplemented;
}

// 定数オブジェクトの登録
bool
reg_const(
  PyTypeObject& type,
  PyObject*& obj,
  const char* name,
  FaultStatus val
)
{
  obj = type.tp_alloc(&type, 0);
  PyFaultStatus::_put(obj, val);
  Py_INCREF(obj);
  if ( PyDict_SetItemString(type.tp_dict, name, obj) < 0 ) {
    return false;
  }
  return true;
}

END_NONAMESPACE


// @brief 'FaultStatus' オブジェクトを使用可能にする．
bool
PyFaultStatus::init(
  PyObject* m
)
{
  FaultStatusType.tp_name = "FaultStatus";
  FaultStatusType.tp_basicsize = sizeof(FaultStatusObject);
  FaultStatusType.tp_itemsize = 0;
  FaultStatusType.tp_dealloc = FaultStatus_dealloc;
  FaultStatusType.tp_flags = Py_TPFLAGS_DEFAULT;
  FaultStatusType.tp_doc = PyDoc_STR("FaultStatus object");
  FaultStatusType.tp_richcompare = FaultStatus_richcmpfunc;
  FaultStatusType.tp_methods = FaultStatus_methods;
  FaultStatusType.tp_new = FaultStatus_new;
  FaultStatusType.tp_repr = FaultStatus_repr;

  // 型オブジェクトの登録
  if ( !reg_type(m, "Faultstatus", &FaultStatusType) ) {
    goto error;
  }

  // 定数オブジェクトの生成
  if ( !reg_const(FaultStatusType, FaultStatus_Undetected,
		  "Undetected", FaultStatus::Undetected) ) {
    goto error;
  }
  if ( !reg_const(FaultStatusType, FaultStatus_Detected,
		  "Detected", FaultStatus::Detected) ) {
    goto error;
  }
  if ( !reg_const(FaultStatusType, FaultStatus_Untestable,
		  "Untestable", FaultStatus::Untestable) ) {
    goto error;
  }

  return true;

 error:

  Py_XDECREF(FaultStatus_Undetected);
  Py_XDECREF(FaultStatus_Detected);
  Py_XDECREF(FaultStatus_Untestable);

  return false;
}

// @brief PyObject から FaultStatus を取り出す．
bool
PyFaultStatus::FromPyObject(
  PyObject* obj,
  FaultStatus& val
)
{
  if ( !_check(obj) ) {
    PyErr_SetString(PyExc_TypeError, "object is not a FaultStatus type");
    return false;
  }
  val = _get(obj);
  return true;
}

// @brief FaultStatus を PyObject に変換する．
PyObject*
PyFaultStatus::ToPyObject(
  FaultStatus val
)
{
  auto obj = FaultStatus_new(_typeobject(), nullptr, nullptr);
  _put(obj, val);
  return obj;
}

// @brief PyObject が FaultStatus タイプか調べる．
bool
PyFaultStatus::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief FaultStatus を表す PyObject から FaultStatus を取り出す．
FaultStatus
PyFaultStatus::_get(
  PyObject* obj
)
{
  auto faultstatus_obj = reinterpret_cast<FaultStatusObject*>(obj);
  return faultstatus_obj->mVal;
}

// @brief FaultStatus を表す PyObject に値を設定する．
void
PyFaultStatus::_put(
  PyObject* obj,
  FaultStatus val
)
{
  auto faultstatus_obj = reinterpret_cast<FaultStatusObject*>(obj);
  faultstatus_obj->mVal = val;
}

// @brief FaultStatus を表すオブジェクトの型定義を返す．
PyTypeObject*
PyFaultStatus::_typeobject()
{
  return &FaultStatusType;
}

END_NAMESPACE_DRUID
