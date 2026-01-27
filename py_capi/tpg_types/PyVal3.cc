
/// @file PyVal3.cc
/// @brief PyVal3 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyVal3.h"
#include "pym/PyString.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct Val3_Object
{
  PyObject_HEAD
  Val3 mVal;
};

// Python 用のタイプ定義
PyTypeObject Val3_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyVal3::init() 中で初期化する．
};

// 定数を表すオブジェクト
PyObject* Const__X = nullptr;
PyObject* Const__0 = nullptr;
PyObject* Const__1 = nullptr;

// 定数の登録を行う関数
bool
reg_const_obj(
  const char* name,
  Val3 val,
  PyObject*& const_obj
)
{
  auto type = PyVal3::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<Val3_Object*>(obj);
  my_obj->mVal = val;
  if ( PyDict_SetItemString(type->tp_dict, name, obj) < 0 ) {
    return false;
  }
  Py_INCREF(obj);
  const_obj = obj;
  return true;
}

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  Py_TYPE(self)->tp_free(self);
}

// repr 関数
PyObject*
repr_func(
  PyObject* self
)
{
  auto& val = PyVal3::_get_ref(self);
  try {
    std::string str_val;
    switch ( val ) {
      case Val3::_X: str_val = "X"; break;
      case Val3::_0: str_val = "0"; break;
      case Val3::_1: str_val = "1"; break;
    }
    return PyString::ToPyObject(str_val);
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// richcompare 関数
PyObject*
richcompare_func(
  PyObject* self,
  PyObject* other,
  int op
)
{
  auto& val = PyVal3::_get_ref(self);
  try {
    if ( PyVal3::Check(self) && PyVal3::Check(other) ) {
      auto& val1 = PyVal3::_get_ref(self);
      auto& val2 = PyVal3::_get_ref(other);
      if ( op == Py_EQ ) {
        return PyBool_FromLong(val1 == val2);
      }
      if ( op == Py_NE ) {
        return PyBool_FromLong(val1 != val2);
      }
    }
    Py_RETURN_NOTIMPLEMENTED;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// new 関数
PyObject*
new_func(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "",
    nullptr
  };
  PyObject* val_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
                                    const_cast<char**>(kwlist),
                                    &val_obj) ) {
    return nullptr;
  }
  Val3 val;
  if ( val_obj != nullptr ) {
    if ( !PyVal3::FromPyObject(val_obj, val) ) {
      PyErr_SetString(PyExc_ValueError, "could not convert to Val3");
      return nullptr;
    }
  }
  try {
    return PyVal3::ToPyObject(val);
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
  catch ( std::out_of_range err ) {
    std::ostringstream buf;
    buf << "out of range" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

END_NONAMESPACE


// @brief Val3 オブジェクトを使用可能にする．
bool
PyVal3::init(
  PyObject* m
)
{
  Val3_Type.tp_name = "Val3";
  Val3_Type.tp_basicsize = sizeof(Val3_Object);
  Val3_Type.tp_itemsize = 0;
  Val3_Type.tp_dealloc = dealloc_func;
  Val3_Type.tp_repr = repr_func;
  Val3_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  Val3_Type.tp_doc = PyDoc_STR("Python extended object for Val3");
  Val3_Type.tp_richcompare = richcompare_func;
  Val3_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "Val3", &Val3_Type) ) {
    goto error;
  }
  // 定数オブジェクトの生成・登録
  if ( !reg_const_obj("_X", Val3::_X, Const__X) ) {
    goto error;
  }
  if ( !reg_const_obj("_0", Val3::_0, Const__0) ) {
    goto error;
  }
  if ( !reg_const_obj("_1", Val3::_1, Const__1) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// Val3 を PyObject に変換する．
PyObject*
PyVal3::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  PyObject* obj = nullptr;
  switch ( val ) {
    case Val3::_X: obj = Const__X; break;
    case Val3::_0: obj = Const__0; break;
    case Val3::_1: obj = Const__1; break;
  }
  if ( obj == nullptr ) {
    PyErr_SetString(PyExc_ValueError, "invalid value for Val3");
    return nullptr;
  }
  Py_INCREF(obj);
  return obj;
}

// PyObject を Val3 に変換する．
bool
PyVal3::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyString::Check(obj) ) {
    auto str_val = PyString::Get(obj);
    if ( strcasecmp(str_val.c_str(), "X") == 0 ) {
      val = Val3::_X;
      return true;
    }
    else if ( strcasecmp(str_val.c_str(), "0") == 0 ) {
      val = Val3::_0;
      return true;
    }
    else if ( strcasecmp(str_val.c_str(), "1") == 0 ) {
      val = Val3::_1;
      return true;
    }
    return false;
  }
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
  return Py_IS_TYPE(obj, &Val3_Type);
}

// @brief PyObject から Val3 を取り出す．
Val3&
PyVal3::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<Val3_Object*>(obj);
  return my_obj->mVal;
}

// @brief Val3 を表すオブジェクトの型定義を返す．
PyTypeObject*
PyVal3::_typeobject()
{
  return &Val3_Type;
}

END_NAMESPACE_DRUID
