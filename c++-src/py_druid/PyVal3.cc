
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
  PyObject* kwds
)
{
  auto self = reinterpret_cast<Val3Object*>(type->tp_alloc(type, 0));
  // 必要なら self の初期化を行う．
  return reinterpret_cast<PyObject*>(self);
}

// 終了関数
void
Val3_dealloc(
  Val3Object* self
)
{
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

// 初期化関数(__init__()相当)
int
Val3_init(
  Val3Object* self,
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

  PyObject* obj;
  if ( !PyArg_ParseTuple(args, "O", &obj) ) {
    return -1;
  }
  if ( PyLong_Check(obj) ) {
    long val = PyLong_AsLong(obj);
    if ( val == 0 ) {
      self->mVal = Val3::_0;
    }
    else if ( val == 1 ) {
      self->mVal = Val3::_1;
    }
    else {
      // エラー
      PyErr_SetString(PyExc_TypeError, "argument 1 must be 0 or 1");
      return -1;
    }
  }
  else {
    const char* val_str = nullptr;
    if ( !PyArg_ParseTuple(args, "s", &val_str) ) {
      return -1;
    }
    if ( strcmp(val_str, "0") == 0 ) {
      self->mVal = Val3::_0;
    }
    else if ( strcmp(val_str, "1") == 0 ) {
      self->mVal = Val3::_1;
    }
    else if ( strcmp(val_str, "x") == 0 ||
	      strcmp(val_str, "X") == 0 ||
	      strcmp(val_str, "?") == 0 ) {
      self->mVal = Val3::_X;
    }
    else {
      // エラー
      PyErr_SetString(PyExc_TypeError,
		      "argument 1 must be \"0\", \"1\", \"x\", \"X\" or \"?\"");
      return -1;
    }
  }
  return 0;
}

// str() 関数
PyObject*
Val3_str(
  PyObject* self
)
{
  const char* tmp_str = nullptr;
  switch ( reinterpret_cast<Val3Object*>(self)->mVal ) {
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

// Python 用のタイプ定義
PyTypeObject Val3Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

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
  Val3Type.tp_init = reinterpret_cast<initproc>(Val3_init);
  Val3Type.tp_new = Val3_new;
  Val3Type.tp_str = Val3_str;
  if ( PyType_Ready(&Val3Type) < 0 ) {
    return false;
  }

  Py_INCREF(&Val3Type);
  if ( PyModule_AddObject(m, "Val3", reinterpret_cast<PyObject*>(&Val3Type)) < 0 ) {
    Py_DECREF(&Val3Type);
    return false;
  }

  Val3_0 = Val3_new(&Val3Type, nullptr, nullptr);
  reinterpret_cast<Val3Object*>(Val3_0)->mVal = Val3::_0;
  Py_INCREF(Val3_0);
  if ( PyModule_AddObject(m, "Val3_0", Val3_0) < 0 ) {
    Py_DECREF(Val3_0);
    return false;
  }

  Val3_1 = Val3_new(&Val3Type, nullptr, nullptr);
  reinterpret_cast<Val3Object*>(Val3_1)->mVal = Val3::_1;
  Py_INCREF(Val3_1);
  if ( PyModule_AddObject(m, "Val3_1", Val3_1) < 0 ) {
    Py_DECREF(Val3_1);
    return false;
  }

  Val3_X = Val3_new(&Val3Type, nullptr, nullptr);
  reinterpret_cast<Val3Object*>(Val3_X)->mVal = Val3::_X;
  Py_INCREF(Val3_X);
  if ( PyModule_AddObject(m, "Val3_X", Val3_X) < 0 ) {
    Py_DECREF(Val3_X);
    return false;
  }

  return true;
}

bool
Val3_from_PyObj(
  PyObject* obj,
  Val3& val
)
{
  if ( !Py_IS_TYPE(obj, &Val3Type) ) {
    PyErr_SetString(PyExc_ValueError, "object is not a Val3 type");
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
  auto val3_obj = Val3_new(&Val3Type, nullptr, nullptr);
  reinterpret_cast<Val3Object*>(val3_obj)->mVal = val;
  return val3_obj;
}

END_NAMESPACE_DRUID
