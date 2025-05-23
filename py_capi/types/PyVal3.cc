
/// @file PyVal3.cc
/// @brief Python Val3 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyVal3.h"
#include "pym/PyModule.h"


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
  PyObject* kwds
)
{
  if ( type != &Val3Type ) {
    PyErr_SetString(PyExc_TypeError, "Val3 cannot be overloaded");
    return nullptr;
  }
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

  static const char* kwlist[] = {
    "name",
    nullptr
  };
  PyObject* obj1;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
				    const_cast<char**>(kwlist),
				    &obj1) ) {
    return nullptr;
  }
  Val3 val3;
  if ( PyLong_Check(obj1) ) {
    long val = PyLong_AsLong(obj1);
    if ( val == 0 ) {
      val3 = Val3::_0;
    }
    else if ( val == 1 ) {
      val3 = Val3::_1;
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
      val3 = Val3::_0;
    }
    else if ( strcmp(val_str, "1") == 0 ) {
      val3 = Val3::_1;
    }
    else if ( strcmp(val_str, "x") == 0 ||
	      strcmp(val_str, "X") == 0 ||
	      strcmp(val_str, "?") == 0 ) {
      val3 = Val3::_X;
    }
    else {
      // エラー
      PyErr_SetString(PyExc_TypeError,
		      "argument 1 must be \"0\", \"1\", \"x\", \"X\" or \"?\"");
      return nullptr;
    }
  }
  return PyVal3::ToPyObject(val3);
}

// 終了関数
void
Val3_dealloc(
  PyObject* self
)
{
  // auto val3_obj = reinterpret_cast<Val3Object*>(self);
  // 必要なら val3_obj->mVal の終了処理を行う．
  Py_TYPE(self)->tp_free(self);
}

// repr() 関数
PyObject*
Val3_repr(
  PyObject* self
)
{
  auto val = PyVal3::_get_ref(self);
  // val から 文字列を作る．
  const char* tmp_str = nullptr;
  switch ( val ) {
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

// 比較関数
PyObject*
Val3_richcmpfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PyVal3::Check(self) &&
       PyVal3::Check(other) ) {
    auto val1 = PyVal3::_get_ref(self);
    auto val2 = PyVal3::_get_ref(other);
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

// 否定演算(単項演算の例)
PyObject*
Val3_invert(
  PyObject* self
)
{
  if ( PyVal3::Check(self) ) {
    auto val = PyVal3::_get_ref(self);
    return PyVal3::ToPyObject(~val);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// AND
PyObject*
Val3_and(
  PyObject* self,
  PyObject* other
)
{
  if ( PyVal3::Check(self) &&
       PyVal3::Check(other) ) {
    auto val1 = PyVal3::_get_ref(self);
    auto val2 = PyVal3::_get_ref(other);
    return PyVal3::ToPyObject(val1 & val2);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// OR
PyObject*
Val3_or(
  PyObject* self,
  PyObject* other
)
{
  if ( PyVal3::Check(self) &&
       PyVal3::Check(other) ) {
    auto val1 = PyVal3::_get_ref(self);
    auto val2 = PyVal3::_get_ref(other);
    return PyVal3::ToPyObject(val1 | val2);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// XOR
PyObject*
Val3_xor(
  PyObject* self,
  PyObject* other
)
{
  if ( PyVal3::Check(self) &&
       PyVal3::Check(other) ) {
    auto val1 = PyVal3::_get_ref(self);
    auto val2 = PyVal3::_get_ref(other);
    return PyVal3::ToPyObject(val1 ^ val2);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 数値演算メソッド定義
PyNumberMethods Val3_number = {
  .nb_invert = Val3_invert,
  .nb_and = Val3_and,
  .nb_xor = Val3_xor,
  .nb_or = Val3_or,
  .nb_inplace_and = Val3_and,
  .nb_inplace_xor = Val3_xor,
  .nb_inplace_or = Val3_or
};

// 定数オブジェクトの生成
PyObject*
new_const(
  Val3 val
)
{
  auto obj = Val3Type.tp_alloc(&Val3Type, 0);
  auto val3_obj = reinterpret_cast<Val3Object*>(obj);
  val3_obj->mVal = val;
  return obj;
}

// 定数オブジェクトの登録
bool
reg_obj(
  const char* name,
  PyObject*& obj
)
{
  if ( PyDict_SetItemString(Val3Type.tp_dict, name, obj) < 0 ) {
    return false;
  }
  return true;
}

END_NONAMESPACE


// @brief 'Val3' オブジェクトを使用可能にする．
bool
PyVal3::init(
  PyObject* m
)
{
  Val3Type.tp_name = "Val3";
  Val3Type.tp_basicsize = sizeof(Val3Object);
  Val3Type.tp_itemsize = 0;
  Val3Type.tp_dealloc = Val3_dealloc;
  Val3Type.tp_flags = Py_TPFLAGS_DEFAULT;
  Val3Type.tp_doc = PyDoc_STR("Val3 object");
  Val3Type.tp_richcompare = Val3_richcmpfunc;
  Val3Type.tp_methods = Val3_methods;
  Val3Type.tp_new = Val3_new;
  Val3Type.tp_repr = Val3_repr;
  Val3Type.tp_as_number = &Val3_number;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "Val3", &Val3Type) ) {
    goto error;
  }

  // 定数オブジェクトの生成
  Val3_0 = new_const(Val3::_0);
  Val3_1 = new_const(Val3::_1);
  Val3_X = new_const(Val3::_X);

  // 定数オブジェクトの登録
  if ( !reg_obj("_0", Val3_0) ) {
    goto error;
  }
  if ( !reg_obj("_1", Val3_1) ) {
    goto error;
  }
  if ( !reg_obj("_X", Val3_X) ) {
    goto error;
  }

  return true;

 error:

  Py_XDECREF(Val3_0);
  Py_XDECREF(Val3_1);
  Py_XDECREF(Val3_X);

  return false;
}

// @brief Val3 を PyObject に変換する．
PyObject*
PyVal3::Conv::operator()(
  const Val3& val
)
{
  PyObject* obj = nullptr;
  switch ( val ) {
  case Val3::_0: obj = Val3_0; break;
  case Val3::_1: obj = Val3_1; break;
  case Val3::_X: obj = Val3_X; break;
  default: ASSERT_NOT_REACHED; break;
  }
  Py_INCREF(obj);
  return obj;
}

// @brief PyObject から Val3 を取り出す．
bool
PyVal3::Deconv::operator()(
  PyObject* obj,
  Val3& val
)
{
  if ( PyVal3::Check(obj) ) {
    val = PyVal3::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が Val3 タイプか調べる．
bool
PyVal3::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief Val3 を表す PyObject から Val3 を取り出す．
Val3&
PyVal3::_get_ref(
  PyObject* obj
)
{
  auto val3_obj = reinterpret_cast<Val3Object*>(obj);
  return val3_obj->mVal;
}

// @brief Val3 を表すオブジェクトの型定義を返す．
PyTypeObject*
PyVal3::_typeobject()
{
  return &Val3Type;
}

END_NAMESPACE_DRUID
