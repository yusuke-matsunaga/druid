
/// @file PyTpgMFFC.cc
/// @brief Python TpgMFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgMFFC.h"
#include "pym/PyTpgFault.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TpgMFFCObject
{
  PyObject_HEAD
  const TpgMFFC* mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgMFFCType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
TpgMFFC_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'TpgMFFC' is disabled.");
  return nullptr;
}

// 終了関数
void
TpgMFFC_dealloc(
  PyObject* self
)
{
  // auto tpgmffc_obj = reinterpret_cast<TpgMFFCObject*>(self);
  // 必要なら tpgmffc_obj->mVal の終了処理を行う．
  Py_TYPE(self)->tp_free(self);
}

// メソッド定義
PyMethodDef TpgMFFC_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// ID番号
PyObject*
TpgMFFC_mffc_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto val = PyTpgMFFC::_get(self);
  return PyLong_FromLong(val->id());
}

// get/set 関数定義
PyGetSetDef TpgMFFC_getset[] = {
  {"ffr_id", TpgMFFC_mffc_id, nullptr, PyDoc_STR("MFFC ID"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr},
};

// 比較関数
PyObject*
TpgMFFC_richcmpfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PyTpgMFFC::_check(self) &&
       PyTpgMFFC::_check(other) ) {
    auto val1 = PyTpgMFFC::_get(self);
    auto val2 = PyTpgMFFC::_get(other);
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


// @brief 'TpgMFFC' オブジェクトを使用可能にする．
bool
PyTpgMFFC::init(
  PyObject* m
)
{
  TpgMFFCType.tp_name = "TpgMFFC";
  TpgMFFCType.tp_basicsize = sizeof(TpgMFFCObject);
  TpgMFFCType.tp_itemsize = 0;
  TpgMFFCType.tp_dealloc = TpgMFFC_dealloc;
  TpgMFFCType.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgMFFCType.tp_doc = PyDoc_STR("TpgMFFC object");
  TpgMFFCType.tp_richcompare = TpgMFFC_richcmpfunc;
  TpgMFFCType.tp_methods = TpgMFFC_methods;
  TpgMFFCType.tp_getset = TpgMFFC_getset;
  TpgMFFCType.tp_new = TpgMFFC_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "TpgMFFC", &TpgMFFCType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief TpgMFFC を PyObject に変換する．
PyObject*
PyTpgMFFC::Conv::operator()(
  const TpgMFFC* val
)
{
  auto obj = TpgMFFCType.tp_alloc(&TpgMFFCType, 0);
  auto tpgmffc_obj = reinterpret_cast<TpgMFFCObject*>(obj);
  tpgmffc_obj->mVal = val;
  return obj;
}

// @brief PyObject* から const TpgFFR* を取り出す．
bool
PyTpgMFFC::Deconv::operator()(
  PyObject* obj,
  const TpgMFFC*& val
)
{
  if ( PyTpgMFFC::_check(obj) ) {
    val = PyTpgMFFC::_get(obj);
    return true;
  }
  return false;
}

// @brief PyObject が TpgMFFC タイプか調べる．
bool
PyTpgMFFC::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TpgMFFC を表す PyObject から TpgMFFC を取り出す．
const TpgMFFC*
PyTpgMFFC::_get(
  PyObject* obj
)
{
  auto tpgmffc_obj = reinterpret_cast<TpgMFFCObject*>(obj);
  return tpgmffc_obj->mVal;
}

// @brief TpgMFFC を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgMFFC::_typeobject()
{
  return &TpgMFFCType;
}

END_NAMESPACE_DRUID
