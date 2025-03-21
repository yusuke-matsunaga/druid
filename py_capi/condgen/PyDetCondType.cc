
/// @file PyDetCondType.cc
/// @brief Python DetCond::Type の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDetCondType.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DetCondTypeObject
{
  PyObject_HEAD
  DetCond::Type mVal;
};

// Python 用のタイプ定義
PyTypeObject DetCondTypeType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 各定数を表す文字列
const char* U_STR = "Undetected";
const char* D_STR = "Detected";
const char* P_STR = "PartialDetected";
const char* O_STR = "Overflow";

// 各定数を表す PyObject
PyObject* DetCondType_U = nullptr;
PyObject* DetCondType_D = nullptr;
PyObject* DetCondType_P = nullptr;
PyObject* DetCondType_O = nullptr;

// 生成関数
PyObject*
DetCondType_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  // キーワード引数
  static const char* kwlist[] = {
    "name",
    nullptr
  };

  if ( type != &DetCondTypeType ) {
    PyErr_SetString(PyExc_TypeError, "DetCondType cannot be overloaded");
    return nullptr;
  }

  const char* name_str = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s",
				    const_cast<char**>(kwlist),
				    &name_str) ) {
    return nullptr;
  }

  // 実はこの関数では実際の new は行わない．
  // 既に作られたオブジェクトを返すだけ
  PyObject* obj = nullptr;
  if ( strcasecmp(name_str, U_STR) == 0 ) {
    obj = DetCondType_U;
  }
  else if ( strcasecmp(name_str, D_STR) == 0 ) {
    obj = DetCondType_D;
  }
  else if ( strcasecmp(name_str, P_STR) == 0 ) {
    obj = DetCondType_P;
  }
  else if ( strcasecmp(name_str, O_STR) == 0 ) {
    obj = DetCondType_O;
  }
  else {
    // エラー
    ostringstream buf;
    buf << "argument 1 must be one of \""
	<< U_STR
	<< "\", \""
	<< D_STR
	<< "\", \""
	<< P_STR
	<< "\", \""
	<< O_STR
	<< "\"";
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
  Py_INCREF(obj);
  return obj;
}

// 終了関数
void
DetCondType_dealloc(
  PyObject* self
)
{
  // auto detcondtype_obj = reinterpret_cast<DetCondTypeObject*>(self);
  // 必要なら detcondtype_obj->mVal の終了処理を行う．
  // delete detcondtype_obj->mVal
  Py_TYPE(self)->tp_free(self);
}

// repr() 関数
PyObject*
DetCondType_repr(
  PyObject* self
)
{
  auto val = PyDetCondType::_get(self);
  // val から 文字列を作る．
  const char* tmp_str = nullptr;
  switch ( val ) {
  case DetCond::Undetected:      tmp_str = U_STR; break;
  case DetCond::Detected:        tmp_str = D_STR; break;
  case DetCond::PartialDetected: tmp_str = P_STR; break;
  case DetCond::Overflow:        tmp_str = O_STR; break;
  }
  return Py_BuildValue("s", tmp_str);
}

// 比較関数
PyObject*
DetCondType_richcmpfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PyDetCondType::Check(self) &&
       PyDetCondType::Check(other) ) {
    auto val1 = PyDetCondType::_get(self);
    auto val2 = PyDetCondType::_get(other);
    bool res = false;
    if ( op == Py_EQ ) {
      return PyBool_FromLong(val1 == val2);
    }
    else if ( op == Py_NE ) {
      return PyBool_FromLong(val1 != val2);
    }
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 定数オブジェクトの生成
PyObject*
new_condtype(
  DetCond::Type type
)
{
  auto obj = DetCondTypeType.tp_alloc(&DetCondTypeType, 0);
  auto type_obj = reinterpret_cast<DetCondTypeObject*>(obj);
  type_obj->mVal = type;
  return obj;
}

// 定数オブジェクトの登録
bool
reg_condtype(
  const char* name,
  PyObject* obj
)
{
  if ( PyDict_SetItemString(DetCondTypeType.tp_dict, name, obj) < 0 ) {
    return false;
  }
  return true;
}

END_NONAMESPACE


// @brief 'DetCondType' オブジェクトを使用可能にする．
bool
PyDetCondType::init(
  PyObject* m
)
{
  DetCondTypeType.tp_name = "DetCondType";
  DetCondTypeType.tp_basicsize = sizeof(DetCondTypeObject);
  DetCondTypeType.tp_itemsize = 0;
  DetCondTypeType.tp_dealloc = DetCondType_dealloc;
  DetCondTypeType.tp_flags = Py_TPFLAGS_DEFAULT;
  DetCondTypeType.tp_doc = PyDoc_STR("DetCond::Type object");
  DetCondTypeType.tp_richcompare = DetCondType_richcmpfunc;
  DetCondTypeType.tp_new = DetCondType_new;
  DetCondTypeType.tp_repr = DetCondType_repr;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "DetCondType", &DetCondTypeType) ) {
    goto error;
  }

  // 定数の生成
  DetCondType_U = new_condtype(DetCond::Undetected);
  DetCondType_D = new_condtype(DetCond::Detected);
  DetCondType_P = new_condtype(DetCond::PartialDetected);
  DetCondType_O = new_condtype(DetCond::Overflow);

  // 定数の登録
  if ( !reg_condtype(U_STR, DetCondType_U) ) {
    goto error;
  }
  if ( !reg_condtype(D_STR, DetCondType_D) ) {
    goto error;
  }
  if ( !reg_condtype(P_STR, DetCondType_P) ) {
    goto error;
  }
  if ( !reg_condtype(O_STR, DetCondType_O) ) {
    goto error;
  }

  return true;

 error:

  Py_XDECREF(DetCondType_U);
  Py_XDECREF(DetCondType_D);
  Py_XDECREF(DetCondType_P);
  Py_XDECREF(DetCondType_O);

  return false;
}

// @brief DetCondType を PyObject に変換する．
PyObject*
PyDetCondType::ToPyObject(
  DetCond::Type val
)
{
  PyObject* obj = nullptr;
  switch ( val ) {
  case DetCond::Undetected:      obj = DetCondType_U; break;
  case DetCond::Detected:        obj = DetCondType_D; break;
  case DetCond::PartialDetected: obj = DetCondType_P; break;
  case DetCond::Overflow:        obj = DetCondType_O; break;
  }
  Py_INCREF(obj);
  return obj;
}

// @brief PyObject が DetCond::Type タイプか調べる．
bool
PyDetCondType::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief PyObject から DetCond::Type を取り出す．
DetCond::Type
PyDetCondType::_get(
  PyObject* obj
)
{
  auto detcondtype_obj = reinterpret_cast<DetCondTypeObject*>(obj);
  return detcondtype_obj->mVal;
}

// @brief DetCond::Type を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDetCondType::_typeobject()
{
  return &DetCondTypeType;
}

END_NAMESPACE_DRUID
