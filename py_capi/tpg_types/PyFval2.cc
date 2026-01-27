
/// @file PyFval2.cc
/// @brief PyFval2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyFval2.h"
#include "pym/PyString.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct Fval2_Object
{
  PyObject_HEAD
  Fval2 mVal;
};

// Python 用のタイプ定義
PyTypeObject Fval2_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyFval2::init() 中で初期化する．
};

// 定数を表すオブジェクト
PyObject* Const_zero = nullptr;
PyObject* Const_one = nullptr;

// 定数の登録を行う関数
bool
reg_const_obj(
  const char* name,
  Fval2 val,
  PyObject*& const_obj
)
{
  auto type = PyFval2::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<Fval2_Object*>(obj);
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
  auto& val = PyFval2::_get_ref(self);
  try {
    std::string str_val;
    switch ( val ) {
      case Fval2::zero: str_val = "zero"; break;
      case Fval2::one: str_val = "one"; break;
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
  auto& val = PyFval2::_get_ref(self);
  try {
    if ( PyFval2::Check(self) && PyFval2::Check(other) ) {
      auto& val1 = PyFval2::_get_ref(self);
      auto& val2 = PyFval2::_get_ref(other);
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
  Fval2 val;
  if ( val_obj != nullptr ) {
    if ( !PyFval2::FromPyObject(val_obj, val) ) {
      PyErr_SetString(PyExc_ValueError, "could not convert to Fval2");
      return nullptr;
    }
  }
  try {
    return PyFval2::ToPyObject(val);
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


// @brief Fval2 オブジェクトを使用可能にする．
bool
PyFval2::init(
  PyObject* m
)
{
  Fval2_Type.tp_name = "Fval2";
  Fval2_Type.tp_basicsize = sizeof(Fval2_Object);
  Fval2_Type.tp_itemsize = 0;
  Fval2_Type.tp_dealloc = dealloc_func;
  Fval2_Type.tp_repr = repr_func;
  Fval2_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  Fval2_Type.tp_doc = PyDoc_STR("Python extended object for Fval2");
  Fval2_Type.tp_richcompare = richcompare_func;
  Fval2_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "Fval2", &Fval2_Type) ) {
    goto error;
  }
  // 定数オブジェクトの生成・登録
  if ( !reg_const_obj("zero", Fval2::zero, Const_zero) ) {
    goto error;
  }
  if ( !reg_const_obj("one", Fval2::one, Const_one) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// Fval2 を PyObject に変換する．
PyObject*
PyFval2::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  PyObject* obj = nullptr;
  switch ( val ) {
    case Fval2::zero: obj = Const_zero; break;
    case Fval2::one: obj = Const_one; break;
  }
  if ( obj == nullptr ) {
    PyErr_SetString(PyExc_ValueError, "invalid value for Fval2");
    return nullptr;
  }
  Py_INCREF(obj);
  return obj;
}

// PyObject を Fval2 に変換する．
bool
PyFval2::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyString::Check(obj) ) {
    auto str_val = PyString::Get(obj);
    if ( strcasecmp(str_val.c_str(), "zero") == 0 ) {
      val = Fval2::zero;
      return true;
    }
    else if ( strcasecmp(str_val.c_str(), "one") == 0 ) {
      val = Fval2::one;
      return true;
    }
    return false;
  }
  if ( PyFval2::Check(obj) ) {
    val = PyFval2::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が Fval2 タイプか調べる．
bool
PyFval2::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &Fval2_Type);
}

// @brief PyObject から Fval2 を取り出す．
Fval2&
PyFval2::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<Fval2_Object*>(obj);
  return my_obj->mVal;
}

// @brief Fval2 を表すオブジェクトの型定義を返す．
PyTypeObject*
PyFval2::_typeobject()
{
  return &Fval2_Type;
}

END_NAMESPACE_DRUID
