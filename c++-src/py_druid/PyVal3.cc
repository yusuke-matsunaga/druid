
/// @file PyVal3.cc
/// @brief PyVal3 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct Val3Object
{
  PyObject_HEAD
  Val3 mVal;
};

// Python 用のタイプ定義
PyTypeObject Val3Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 定数0
PyObject* Val3_0 = nullptr;

// 定数1
PyObject* Val3_1 = nullptr;

// 定数X
PyObject* Val3_X = nullptr;

// 生成関数
PyObject*
Val3_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  // 変換ルール
  // - 数値型
  //   * 0 -> Val3::_0
  //   * 1 -> Val3::_1
  // - 文字列型
  //   * "0" -> Val3::_0
  //   * "1" -> Val3::_1
  //   * "x"
  //     "X"
  //     "?" -> Val3::_X
  // それ以外は TypeError

  PyObject* obj1;
  if ( !PyArg_ParseTuple(args, "O", &obj1) ) {
    return nullptr;
  }
  PyObject* val3_obj = nullptr;
  if ( PyLong_Check(obj1) ) {
    long val = PyLong_AsLong(obj1);
    if ( val == 0 ) {
      val3_obj = Val3_0;
    }
    else if ( val == 1 ) {
      val3_obj = Val3_1;
    }
    else {
      // エラー
      PyErr_SetString(PyExc_TypeError, "argument 1 must be 0 or 1");
      return nullptr;
    }
  }
  else {
    const char* val_str = nullptr;
    if ( !PyArg_ParseTuple(args, "s", &val_str) ) {
      return nullptr;
    }
    if ( strcmp(val_str, "0") == 0 ) {
      val3_obj = Val3_0;
    }
    else if ( strcmp(val_str, "1") == 0 ) {
      val3_obj = Val3_1;
    }
    else if ( strcmp(val_str, "x") == 0 ||
	      strcmp(val_str, "X") == 0 ||
	      strcmp(val_str, "?") == 0 ) {
      val3_obj = Val3_X;
    }
    else {
      // エラー
      PyErr_SetString(PyExc_TypeError,
		      "argument 1 must be \"0\", \"1\", \"x\", \"X\" or \"?\"");
      return nullptr;
    }
  }
  Py_INCREF(val3_obj);
  return val3_obj;
}

// 終了関数
void
Val3_dealloc(
  PyObject* self
)
{
  // auto val3_obj = reinterpret_cast<Val3Object*>(obj);
  // 必要なら val3_obj->mVal の終了処理を行う．
  Py_TYPE(self)->tp_free(self);
}

// repr() 関数
PyObject*
Val3_repr(
  PyObject* self
)
{
  auto val3_obj = reinterpret_cast<Val3Object*>(self);
  const char* tmp_str = nullptr;
  switch ( val3_obj->mVal ) {
  case Val3::_X: tmp_str = "X"; break;
  case Val3::_0: tmp_str = "0"; break;
  case Val3::_1: tmp_str = "1"; break;
  }
  return Py_BuildValue("s", tmp_str);
}

// メソッド定義
PyMethodDef Val3_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// 定数オブジェクトの登録
bool
reg_val3(
  PyObject* m,
  PyObject*& obj,
  const char* name,
  Val3 val
)
{
  obj = Val3Type.tp_alloc(&Val3Type, 0);
  auto val3_obj = reinterpret_cast<Val3Object*>(obj);
  val3_obj->mVal = val;
  Py_INCREF(obj);
  if ( PyModule_AddObject(m, name, obj) < 0 ) {
    return false;
  }
  return true;
}

END_NONAMESPACE


// @brief 'Val3' オブジェクトを使用可能にする．
bool
PyInit_Val3(
  PyObject* m
)
{
  Val3Type.tp_name = "druid.Val3";
  Val3Type.tp_basicsize = sizeof(Val3Object);
  Val3Type.tp_itemsize = 0;
  Val3Type.tp_dealloc = reinterpret_cast<destructor>(Val3_dealloc);
  Val3Type.tp_flags = Py_TPFLAGS_DEFAULT;
  Val3Type.tp_doc = PyDoc_STR("Val3 objects");
  Val3Type.tp_methods = Val3_methods;
  Val3Type.tp_new = Val3_new;
  Val3Type.tp_repr = Val3_repr;
  if ( PyType_Ready(&Val3Type) < 0 ) {
    return false;
  }

  Py_INCREF(&Val3Type);
  if ( PyModule_AddObject(m, "Val3", reinterpret_cast<PyObject*>(&Val3Type)) < 0 ) {
    Py_DECREF(&Val3Type);
    goto error;
  }

  if ( !reg_val3(m, Val3_0, "Val3_0", Val3::_0) ) {
    goto error;
  }

  if ( !reg_val3(m, Val3_1, "Val3_1", Val3::_1) ) {
    goto error;
  }

  if ( !reg_val3(m, Val3_X, "Val3_X", Val3::_X) ) {
    goto error;
  }

  return true;

 error:

  Py_XDECREF(Val3_0);
  Py_XDECREF(Val3_1);
  Py_XDECREF(Val3_X);

  return false;
}

bool
Val3_from_PyObj(
  PyObject* obj,
  Val3& val
)
{
  if ( !Py_IS_TYPE(obj, &Val3Type) ) {
    PyErr_SetString(PyExc_TypeError, "object is not a Val3 type");
    return false;
  }
  auto val3_obj = reinterpret_cast<Val3Object*>(obj);
  val = val3_obj->mVal;
  return true;
}

PyObject*
PyObj_from_Val3(
  Val3 val
)
{
  auto obj = Val3_new(&Val3Type, nullptr, nullptr);
  auto val3_obj = reinterpret_cast<Val3Object*>(obj);
  val3_obj->mVal = val;
  return obj;
}

END_NAMESPACE_DRUID
