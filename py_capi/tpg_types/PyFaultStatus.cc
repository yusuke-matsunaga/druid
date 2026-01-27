
/// @file PyFaultStatus.cc
/// @brief PyFaultStatus の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyFaultStatus.h"
#include "pym/PyString.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct FaultStatus_Object
{
  PyObject_HEAD
  FaultStatus mVal;
};

// Python 用のタイプ定義
PyTypeObject FaultStatus_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyFaultStatus::init() 中で初期化する．
};

// 定数を表すオブジェクト
PyObject* Const_Undetected = nullptr;
PyObject* Const_Detected = nullptr;
PyObject* Const_Untestable = nullptr;

// 定数の登録を行う関数
bool
reg_const_obj(
  const char* name,
  FaultStatus val,
  PyObject*& const_obj
)
{
  auto type = PyFaultStatus::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<FaultStatus_Object*>(obj);
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
  auto& val = PyFaultStatus::_get_ref(self);
  try {
    std::string str_val;
    switch ( val ) {
      case FaultStatus::Undetected: str_val = "Undetected"; break;
      case FaultStatus::Detected: str_val = "Detected"; break;
      case FaultStatus::Untestable: str_val = "Untestable"; break;
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
  auto& val = PyFaultStatus::_get_ref(self);
  try {
    if ( PyFaultStatus::Check(self) && PyFaultStatus::Check(other) ) {
      auto& val1 = PyFaultStatus::_get_ref(self);
      auto& val2 = PyFaultStatus::_get_ref(other);
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
  FaultStatus val;
  if ( val_obj != nullptr ) {
    if ( !PyFaultStatus::FromPyObject(val_obj, val) ) {
      PyErr_SetString(PyExc_ValueError, "could not convert to FaultStatus");
      return nullptr;
    }
  }
  try {
    return PyFaultStatus::ToPyObject(val);
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


// @brief FaultStatus オブジェクトを使用可能にする．
bool
PyFaultStatus::init(
  PyObject* m
)
{
  FaultStatus_Type.tp_name = "FaultStatus";
  FaultStatus_Type.tp_basicsize = sizeof(FaultStatus_Object);
  FaultStatus_Type.tp_itemsize = 0;
  FaultStatus_Type.tp_dealloc = dealloc_func;
  FaultStatus_Type.tp_repr = repr_func;
  FaultStatus_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  FaultStatus_Type.tp_doc = PyDoc_STR("Python extended object for FaultStatus");
  FaultStatus_Type.tp_richcompare = richcompare_func;
  FaultStatus_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "FaultStatus", &FaultStatus_Type) ) {
    goto error;
  }
  // 定数オブジェクトの生成・登録
  if ( !reg_const_obj("Undetected", FaultStatus::Undetected, Const_Undetected) ) {
    goto error;
  }
  if ( !reg_const_obj("Detected", FaultStatus::Detected, Const_Detected) ) {
    goto error;
  }
  if ( !reg_const_obj("Untestable", FaultStatus::Untestable, Const_Untestable) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// FaultStatus を PyObject に変換する．
PyObject*
PyFaultStatus::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  PyObject* obj = nullptr;
  switch ( val ) {
    case FaultStatus::Undetected: obj = Const_Undetected; break;
    case FaultStatus::Detected: obj = Const_Detected; break;
    case FaultStatus::Untestable: obj = Const_Untestable; break;
  }
  if ( obj == nullptr ) {
    PyErr_SetString(PyExc_ValueError, "invalid value for FaultStatus");
    return nullptr;
  }
  Py_INCREF(obj);
  return obj;
}

// PyObject を FaultStatus に変換する．
bool
PyFaultStatus::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyString::Check(obj) ) {
    auto str_val = PyString::Get(obj);
    if ( str_val == "Undetected" ) {
      val = FaultStatus::Undetected;
      return true;
    }
    else if ( str_val == "Detected" ) {
      val = FaultStatus::Detected;
      return true;
    }
    else if ( str_val == "Untestable" ) {
      val = FaultStatus::Untestable;
      return true;
    }
    return false;
  }
  if ( PyFaultStatus::Check(obj) ) {
    val = PyFaultStatus::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が FaultStatus タイプか調べる．
bool
PyFaultStatus::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &FaultStatus_Type);
}

// @brief PyObject から FaultStatus を取り出す．
FaultStatus&
PyFaultStatus::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<FaultStatus_Object*>(obj);
  return my_obj->mVal;
}

// @brief FaultStatus を表すオブジェクトの型定義を返す．
PyTypeObject*
PyFaultStatus::_typeobject()
{
  return &FaultStatus_Type;
}

END_NAMESPACE_DRUID
