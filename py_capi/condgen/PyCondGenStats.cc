
/// @file PyCondGenStats.cc
/// @brief PyCondGenStats の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyCondGenStats.h"
#include "pym/PyCnfSize.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct CondGenStats_Object
{
  PyObject_HEAD
  CondGenStats mVal;
};

// Python 用のタイプ定義
PyTypeObject CondGenStats_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyCondGenStats::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<CondGenStats_Object*>(self);
  obj->mVal.~CondGenStats();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
get_base_size(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyCondGenStats::_get_ref(self);
  try {
    return PyCnfSize::ToPyObject(val.base_size());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_bd_size(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyCondGenStats::_get_ref(self);
  try {
    return PyCnfSize::ToPyObject(val.bd_size());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_cond_size(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyCondGenStats::_get_ref(self);
  try {
    return PyCnfSize::ToPyObject(val.cond_size());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// getter/setter定義
PyGetSetDef getsets[] = {
  {"base_size", get_base_size, nullptr, PyDoc_STR(""), nullptr},
  {"bd_size", get_bd_size, nullptr, PyDoc_STR(""), nullptr},
  {"cond_size", get_cond_size, nullptr, PyDoc_STR(""), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief CondGenStats オブジェクトを使用可能にする．
bool
PyCondGenStats::init(
  PyObject* m
)
{
  CondGenStats_Type.tp_name = "CondGenStats";
  CondGenStats_Type.tp_basicsize = sizeof(CondGenStats_Object);
  CondGenStats_Type.tp_itemsize = 0;
  CondGenStats_Type.tp_dealloc = dealloc_func;
  CondGenStats_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  CondGenStats_Type.tp_doc = PyDoc_STR("Python extended object for CondGenStats");
  CondGenStats_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "CondGenStats", &CondGenStats_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// CondGenStats を PyObject に変換する．
PyObject*
PyCondGenStats::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyCondGenStats::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<CondGenStats_Object*>(obj);
  new (&my_obj->mVal) CondGenStats(val);
  return obj;
}

// PyObject を CondGenStats に変換する．
bool
PyCondGenStats::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyCondGenStats::Check(obj) ) {
    val = PyCondGenStats::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が CondGenStats タイプか調べる．
bool
PyCondGenStats::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &CondGenStats_Type);
}

// @brief PyObject から CondGenStats を取り出す．
CondGenStats&
PyCondGenStats::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<CondGenStats_Object*>(obj);
  return my_obj->mVal;
}

// @brief CondGenStats を表すオブジェクトの型定義を返す．
PyTypeObject*
PyCondGenStats::_typeobject()
{
  return &CondGenStats_Type;
}

END_NAMESPACE_DRUID
