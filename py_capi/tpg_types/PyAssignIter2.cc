
/// @file PyAssignIter2.cc
/// @brief PyAssignIter2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyAssignIter2.h"
#include "pym/PyAssign.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct AssignIter2_Object
{
  PyObject_HEAD
  AssignIter2 mVal;
};

// Python 用のタイプ定義
PyTypeObject AssignIter2_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyAssignIter2::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<AssignIter2_Object*>(self);
  obj->mVal.~AssignIter2();
  Py_TYPE(self)->tp_free(self);
}

// iter 関数
PyObject*
iter_func(
  PyObject* self
)
{
  auto& val = PyAssignIter2::_get_ref(self);
  try {
    return self;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// iternext 関数
PyObject*
iternext_func(
  PyObject* self
)
{
  auto& val = PyAssignIter2::_get_ref(self);
  try {
    if ( val.has_next() ) {
      return PyAssign::ToPyObject(val.next());
    }
    return nullptr;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

END_NONAMESPACE


// @brief AssignIter2 オブジェクトを使用可能にする．
bool
PyAssignIter2::init(
  PyObject* m
)
{
  AssignIter2_Type.tp_name = "AssignIter2";
  AssignIter2_Type.tp_basicsize = sizeof(AssignIter2_Object);
  AssignIter2_Type.tp_itemsize = 0;
  AssignIter2_Type.tp_dealloc = dealloc_func;
  AssignIter2_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  AssignIter2_Type.tp_doc = PyDoc_STR("Python extended object for AssignIter2");
  AssignIter2_Type.tp_iter = iter_func;
  AssignIter2_Type.tp_iternext = iternext_func;
  if ( !PyModule::reg_type(m, "AssignIter2", &AssignIter2_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// AssignIter2 を PyObject に変換する．
PyObject*
PyAssignIter2::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyAssignIter2::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<AssignIter2_Object*>(obj);
  new (&my_obj->mVal) AssignIter2(val);
  return obj;
}

// @brief PyObject が AssignIter2 タイプか調べる．
bool
PyAssignIter2::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &AssignIter2_Type);
}

// @brief PyObject から AssignIter2 を取り出す．
AssignIter2&
PyAssignIter2::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<AssignIter2_Object*>(obj);
  return my_obj->mVal;
}

// @brief AssignIter2 を表すオブジェクトの型定義を返す．
PyTypeObject*
PyAssignIter2::_typeobject()
{
  return &AssignIter2_Type;
}

END_NAMESPACE_DRUID
