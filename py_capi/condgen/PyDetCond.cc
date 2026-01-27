
/// @file PyDetCond.cc
/// @brief PyDetCond の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDetCond.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct DetCond_Object
{
  PyObject_HEAD
  DetCond mVal;
};

// Python 用のタイプ定義
PyTypeObject DetCond_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyDetCond::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<DetCond_Object*>(self);
  obj->mVal.~DetCond();
  Py_TYPE(self)->tp_free(self);
}

END_NONAMESPACE


// @brief DetCond オブジェクトを使用可能にする．
bool
PyDetCond::init(
  PyObject* m
)
{
  DetCond_Type.tp_name = "DetCond";
  DetCond_Type.tp_basicsize = sizeof(DetCond_Object);
  DetCond_Type.tp_itemsize = 0;
  DetCond_Type.tp_dealloc = dealloc_func;
  DetCond_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  DetCond_Type.tp_doc = PyDoc_STR("Python extended object for DetCond");
  if ( !PyModule::reg_type(m, "DetCond", &DetCond_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// DetCond を PyObject に変換する．
PyObject*
PyDetCond::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyDetCond::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<DetCond_Object*>(obj);
  new (&my_obj->mVal) DetCond(val);
  return obj;
}

// PyObject を DetCond に変換する．
bool
PyDetCond::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyDetCond::Check(obj) ) {
    val = PyDetCond::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が DetCond タイプか調べる．
bool
PyDetCond::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &DetCond_Type);
}

// @brief PyObject から DetCond を取り出す．
DetCond&
PyDetCond::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<DetCond_Object*>(obj);
  return my_obj->mVal;
}

// @brief DetCond を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDetCond::_typeobject()
{
  return &DetCond_Type;
}

END_NAMESPACE_DRUID
