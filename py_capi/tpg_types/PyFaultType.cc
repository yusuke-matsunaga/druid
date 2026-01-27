
/// @file PyFaultType.cc
/// @brief PyFaultType の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyFaultType.h"
#include "pym/PyString.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct FaultType_Object
{
  PyObject_HEAD
  FaultType mVal;
};

// Python 用のタイプ定義
PyTypeObject FaultType_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyFaultType::init() 中で初期化する．
};

// 定数を表すオブジェクト
PyObject* Const_StuckAt = nullptr;
PyObject* Const_TransitionDelay = nullptr;
PyObject* Const_GateExhaustive = nullptr;

// 定数の登録を行う関数
bool
reg_const_obj(
  const char* name,
  FaultType val,
  PyObject*& const_obj
)
{
  auto type = PyFaultType::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<FaultType_Object*>(obj);
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
  auto& val = PyFaultType::_get_ref(self);
  try {
    std::string str_val;
    switch ( val ) {
      case FaultType::StuckAt: str_val = "StuckAt"; break;
      case FaultType::TransitionDelay: str_val = "TransitionDelay"; break;
      case FaultType::GateExhaustive: str_val = "GateExhaustive"; break;
      case FaultType::None: str_val = "None"; break;
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
  auto& val = PyFaultType::_get_ref(self);
  try {
    if ( PyFaultType::Check(self) && PyFaultType::Check(other) ) {
      auto& val1 = PyFaultType::_get_ref(self);
      auto& val2 = PyFaultType::_get_ref(other);
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
  FaultType val;
  if ( val_obj != nullptr ) {
    if ( !PyFaultType::FromPyObject(val_obj, val) ) {
      PyErr_SetString(PyExc_ValueError, "could not convert to FaultType");
      return nullptr;
    }
  }
  try {
    return PyFaultType::ToPyObject(val);
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


// @brief FaultType オブジェクトを使用可能にする．
bool
PyFaultType::init(
  PyObject* m
)
{
  FaultType_Type.tp_name = "FaultType";
  FaultType_Type.tp_basicsize = sizeof(FaultType_Object);
  FaultType_Type.tp_itemsize = 0;
  FaultType_Type.tp_dealloc = dealloc_func;
  FaultType_Type.tp_repr = repr_func;
  FaultType_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  FaultType_Type.tp_doc = PyDoc_STR("Python extended object for FaultType");
  FaultType_Type.tp_richcompare = richcompare_func;
  FaultType_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "FaultType", &FaultType_Type) ) {
    goto error;
  }
  // 定数オブジェクトの生成・登録
  if ( !reg_const_obj("StuckAt", FaultType::StuckAt, Const_StuckAt) ) {
    goto error;
  }
  if ( !reg_const_obj("TransitionDelay", FaultType::TransitionDelay, Const_TransitionDelay) ) {
    goto error;
  }
  if ( !reg_const_obj("GateExhaustive", FaultType::GateExhaustive, Const_GateExhaustive) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// FaultType を PyObject に変換する．
PyObject*
PyFaultType::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  PyObject* obj = nullptr;
  switch ( val ) {
    case FaultType::StuckAt: obj = Const_StuckAt; break;
    case FaultType::TransitionDelay: obj = Const_TransitionDelay; break;
    case FaultType::GateExhaustive: obj = Const_GateExhaustive; break;
    case FaultType::None: Py_RETURN_NONE;
  }
  if ( obj == nullptr ) {
    PyErr_SetString(PyExc_ValueError, "invalid value for FaultType");
    return nullptr;
  }
  Py_INCREF(obj);
  return obj;
}

// PyObject を FaultType に変換する．
bool
PyFaultType::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( obj == Py_None ) {
    val = FaultType::None;
    return true;
  }
  if ( PyString::Check(obj) ) {
    auto str_val = PyString::Get(obj);
    if ( str_val == "StuckAt" ) {
      val = FaultType::StuckAt;
      return true;
    }
    else if ( str_val == "TransitionDelay" ) {
      val = FaultType::TransitionDelay;
      return true;
    }
    else if ( str_val == "GateExhaustive" ) {
      val = FaultType::GateExhaustive;
      return true;
    }
    return false;
  }
  if ( PyFaultType::Check(obj) ) {
    val = PyFaultType::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が FaultType タイプか調べる．
bool
PyFaultType::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &FaultType_Type);
}

// @brief PyObject から FaultType を取り出す．
FaultType&
PyFaultType::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<FaultType_Object*>(obj);
  return my_obj->mVal;
}

// @brief FaultType を表すオブジェクトの型定義を返す．
PyTypeObject*
PyFaultType::_typeobject()
{
  return &FaultType_Type;
}

END_NAMESPACE_DRUID
