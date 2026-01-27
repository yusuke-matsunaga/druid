
/// @file PyCondGenMgr.cc
/// @brief PyCondGenMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyCondGenMgr.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct CondGenMgr_Object
{
  PyObject_HEAD
  CondGenMgr mVal;
};

// Python 用のタイプ定義
PyTypeObject CondGenMgr_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyCondGenMgr::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<CondGenMgr_Object*>(self);
  obj->mVal.~CondGenMgr();
  Py_TYPE(self)->tp_free(self);
}

END_NONAMESPACE


// @brief CondGenMgr オブジェクトを使用可能にする．
bool
PyCondGenMgr::init(
  PyObject* m
)
{
  CondGenMgr_Type.tp_name = "CondGenMgr";
  CondGenMgr_Type.tp_basicsize = sizeof(CondGenMgr_Object);
  CondGenMgr_Type.tp_itemsize = 0;
  CondGenMgr_Type.tp_dealloc = dealloc_func;
  CondGenMgr_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  CondGenMgr_Type.tp_doc = PyDoc_STR("Python extended object for CondGenMgr");
  if ( !PyModule::reg_type(m, "CondGenMgr", &CondGenMgr_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が CondGenMgr タイプか調べる．
bool
PyCondGenMgr::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &CondGenMgr_Type);
}

// @brief PyObject から CondGenMgr を取り出す．
CondGenMgr&
PyCondGenMgr::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<CondGenMgr_Object*>(obj);
  return my_obj->mVal;
}

// @brief CondGenMgr を表すオブジェクトの型定義を返す．
PyTypeObject*
PyCondGenMgr::_typeobject()
{
  return &CondGenMgr_Type;
}

END_NAMESPACE_DRUID
