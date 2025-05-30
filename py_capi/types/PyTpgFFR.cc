
/// @file PyTpgFFR.cc
/// @brief Python TpgFFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgFFR.h"
#include "pym/PyTpgFault.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TpgFFRObject
{
  PyObject_HEAD
  const TpgFFR* mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgFFRType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
TpgFFR_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'TpgFFR' is disabled.");
  return nullptr;
}

// 終了関数
void
TpgFFR_dealloc(
  PyObject* self
)
{
  // auto tpgffr_obj = reinterpret_cast<TpgFFRObject*>(self);
  // 必要なら tpgffr_obj->mVal の終了処理を行う．
  Py_TYPE(self)->tp_free(self);
}

// メソッド定義
PyMethodDef TpgFFR_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// ID番号
PyObject*
TpgFFR_ffr_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto val = PyTpgFFR::_get(self);
  return PyLong_FromLong(val->id());
}

// get/set 関数定義
PyGetSetDef TpgFFR_getset[] = {
  {"ffr_id", TpgFFR_ffr_id, nullptr, PyDoc_STR("FFR ID"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr},
};

// 比較関数
PyObject*
TpgFFR_richcmpfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PyTpgFFR::_check(self) &&
       PyTpgFFR::_check(other) ) {
    auto val1 = PyTpgFFR::_get(self);
    auto val2 = PyTpgFFR::_get(other);
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

END_NONAMESPACE


// @brief 'TpgFFR' オブジェクトを使用可能にする．
bool
PyTpgFFR::init(
  PyObject* m
)
{
  TpgFFRType.tp_name = "TpgFFR";
  TpgFFRType.tp_basicsize = sizeof(TpgFFRObject);
  TpgFFRType.tp_itemsize = 0;
  TpgFFRType.tp_dealloc = TpgFFR_dealloc;
  TpgFFRType.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgFFRType.tp_doc = PyDoc_STR("TpgFFR object");
  TpgFFRType.tp_richcompare = TpgFFR_richcmpfunc;
  TpgFFRType.tp_methods = TpgFFR_methods;
  TpgFFRType.tp_getset = TpgFFR_getset;
  TpgFFRType.tp_new = TpgFFR_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "TpgFFR", &TpgFFRType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief TpgFFR を PyObject に変換する．
PyObject*
PyTpgFFR::Conv::operator()(
  const TpgFFR* val
)
{
  auto obj = TpgFFRType.tp_alloc(&TpgFFRType, 0);
  auto tpgffr_obj = reinterpret_cast<TpgFFRObject*>(obj);
  tpgffr_obj->mVal = val;
  return obj;
}

// @brief PyObject* から const TpgFFR* を取り出す．
bool
PyTpgFFR::Deconv::operator()(
  PyObject* obj,
  const TpgFFR*& val
)
{
  if ( PyTpgFFR::_check(obj) ) {
    val = PyTpgFFR::_get(obj);
    return true;
  }
  return false;
}

// @brief PyObject が TpgFFR タイプか調べる．
bool
PyTpgFFR::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TpgFFR を表す PyObject から TpgFFR を取り出す．
const TpgFFR*
PyTpgFFR::_get(
  PyObject* obj
)
{
  auto tpgffr_obj = reinterpret_cast<TpgFFRObject*>(obj);
  return tpgffr_obj->mVal;
}

// @brief TpgFFR を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgFFR::_typeobject()
{
  return &TpgFFRType;
}

END_NAMESPACE_DRUID
