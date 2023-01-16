
/// @file PyFaultType.cc
/// @brief Python FaultType の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "PyFaultType.h"
#include "ym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct FaultTypeObject
{
  PyObject_HEAD
  FaultType mVal;
};

// Python 用のタイプ定義
PyTypeObject FaultTypeType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 定数 "stuck-at"
PyObject* FaultType_StuckAt = nullptr;

// 定数 "transition-delay"
PyObject* FaultType_TransitionDelay = nullptr;

// 生成関数
PyObject*
FaultType_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  if ( type != &FaultTypeType ) {
    PyErr_SetString(PyExc_TypeError, "FaultType cannot be overloaded");
    return nullptr;
  }

  // 変換ルール
  // - 文字列型
  //   * "stuck-at"|"s-a" -> FaultType::StuckAt
  //   * "transition-delay"|"t-d" -> FaultType::TransitionDelay
  // それ以外は TypeError
  static const char* kwlist[] = {
    "name",
    nullptr
  };
  const char* val_str = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s",
				    const_cast<char**>(kwlist),
				    &val_str) ) {
    return nullptr;
  }

  FaultType fault_type;
  if ( strcmp(val_str, "stuck-at") == 0 ||
       strcmp(val_str, "s-a") == 0 ) {
    fault_type = FaultType::StuckAt;
  }
  else if ( strcmp(val_str, "transition-delay") == 0 ||
	    strcmp(val_str, "t-d") == 0 ) {
    fault_type = FaultType::TransitionDelay;
  }
  else {
    PyErr_SetString(PyExc_ValueError,
		    "1st argument should be either 'stuck-at' or 'transition-delay'");
    return nullptr;
  }
  return PyFaultType::ToPyObject(fault_type);
}

// 終了関数
void
FaultType_dealloc(
  PyObject* self
)
{
  // auto faulttype_obj = reinterpret_cast<FaultTypeObject*>(self);
  // 必要なら faulttype_obj->mVal の終了処理を行う．
  Py_TYPE(self)->tp_free(self);
}

// repr() 関数
PyObject*
FaultType_repr(
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

// 定数オブジェクトの生成
PyObject*
new_const(
  FaultType val
)
{
  auto obj = FaultTypeType.tp_alloc(&FaultTypeType, 0);
  auto ft_obj = reinterpret_cast<FaultTypeObject*>(obj);
  ft_obj->mVal = val;
  Py_INCREF(obj);
  return obj;
}

// 定数オブジェクトの登録
bool
reg_const(
  const char* name,
  PyObject*& obj
)
{
  if ( PyDict_SetItemString(FaultTypeType.tp_dict, name, obj) < 0 ) {
    return false;
  }
  return true;
}

END_NONAMESPACE


// @brief 'FaultType' オブジェクトを使用可能にする．
bool
PyFaultType::init(
  PyObject* m
)
{
  FaultTypeType.tp_name = "FaultType";
  FaultTypeType.tp_basicsize = sizeof(FaultTypeObject);
  FaultTypeType.tp_itemsize = 0;
  FaultTypeType.tp_dealloc = FaultType_dealloc;
  FaultTypeType.tp_flags = Py_TPFLAGS_DEFAULT;
  FaultTypeType.tp_doc = PyDoc_STR("FaultType object");
  FaultTypeType.tp_methods = FaultType_methods;
  FaultTypeType.tp_new = FaultType_new;
  FaultTypeType.tp_repr = FaultType_repr;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "FaultType", &FaultTypeType) ) {
    goto error;
  }

  // 定数オブジェクトの生成
  FaultType_StuckAt = new_const(FaultType::StuckAt);
  FaultType_TransitionDelay = new_const(FaultType::TransitionDelay);

  // 定数オブジェクトの登録
  if ( !reg_const("StuckAt", FaultType_StuckAt) ) {
    goto error;
  }
  if ( !reg_const("TransitionDelay", FaultType_TransitionDelay) ) {
    goto error;
  }

  return true;

 error:

  Py_XDECREF(FaultType_StuckAt);
  Py_XDECREF(FaultType_TransitionDelay);

  return false;
}

// @brief PyObject から FaultType を取り出す．
bool
PyFaultType::FromPyObject(
  PyObject* obj,
  FaultType& val
)
{
  if ( !_check(obj) ) {
    PyErr_SetString(PyExc_TypeError, "object is not a FaultType type");
    return false;
  }
  val = _get(obj);
  return true;
}

// @brief FaultType を PyObject に変換する．
PyObject*
PyFaultType::ToPyObject(
  FaultType val
)
{
  PyObject* obj = nullptr;
  switch ( val ) {
  case FaultType::StuckAt: obj = FaultType_StuckAt; break;
  case FaultType::TransitionDelay: obj = FaultType_TransitionDelay; break;
  default: ASSERT_NOT_REACHED; break;
  }
  Py_INCREF(obj);
  return obj;
}

// @brief PyObject が FaultType タイプか調べる．
bool
PyFaultType::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief FaultType を表す PyObject から FaultType を取り出す．
FaultType
PyFaultType::_get(
  PyObject* obj
)
{
  auto faulttype_obj = reinterpret_cast<FaultTypeObject*>(obj);
  return faulttype_obj->mVal;
}

// @brief FaultType を表すオブジェクトの型定義を返す．
PyTypeObject*
PyFaultType::_typeobject()
{
  return &FaultTypeType;
}

END_NAMESPACE_DRUID
