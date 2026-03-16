
/// @file PySuffCond.cc
/// @brief PySuffCond の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PySuffCond.h"
#include "pym/PyAssignList.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct StructEngine_Object
{
  PyObject_HEAD
  SuffCond mVal;
};

// Python 用のタイプ定義
PyTypeObject StructEngine_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PySuffCond::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<StructEngine_Object*>(self);
  obj->mVal.~SuffCond();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
get_main_cond(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PySuffCond::_get_ref(self);
  try {
    return PyAssignList::ToPyObject(val.main_cond());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_aux_cond(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PySuffCond::_get_ref(self);
  try {
    return PyAssignList::ToPyObject(val.aux_cond());
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
  {"main_cond", get_main_cond, nullptr, PyDoc_STR("main condition"), nullptr},
  {"aux_cond", get_aux_cond, nullptr, PyDoc_STR("auxially condition"), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief StructEngine オブジェクトを使用可能にする．
bool
PySuffCond::init(
  PyObject* m
)
{
  StructEngine_Type.tp_name = "StructEngine";
  StructEngine_Type.tp_basicsize = sizeof(StructEngine_Object);
  StructEngine_Type.tp_itemsize = 0;
  StructEngine_Type.tp_dealloc = dealloc_func;
  StructEngine_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  StructEngine_Type.tp_doc = PyDoc_STR("Python extended object for SuffCond");
  StructEngine_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "StructEngine", &StructEngine_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// SuffCond を PyObject に変換する．
PyObject*
PySuffCond::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PySuffCond::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<StructEngine_Object*>(obj);
  new (&my_obj->mVal) SuffCond(val);
  return obj;
}

// PyObject を SuffCond に変換する．
bool
PySuffCond::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PySuffCond::Check(obj) ) {
    val = PySuffCond::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が SuffCond タイプか調べる．
bool
PySuffCond::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &StructEngine_Type);
}

// @brief PyObject から SuffCond を取り出す．
SuffCond&
PySuffCond::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<StructEngine_Object*>(obj);
  return my_obj->mVal;
}

// @brief SuffCond を表すオブジェクトの型定義を返す．
PyTypeObject*
PySuffCond::_typeobject()
{
  return &StructEngine_Type;
}

END_NAMESPACE_DRUID
