
/// @file PyTpgFaultIter2.cc
/// @brief PyTpgFaultIter2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgFaultIter2.h"
#include "pym/PyTpgFault.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgFaultIter2_Object
{
  PyObject_HEAD
  TpgFaultIter2 mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgFaultIter2_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgFaultIter2::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgFaultIter2_Object*>(self);
  obj->mVal.~TpgFaultIter2();
  Py_TYPE(self)->tp_free(self);
}

// iter 関数
PyObject*
iter_func(
  PyObject* self
)
{
  auto& val = PyTpgFaultIter2::_get_ref(self);
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
  auto& val = PyTpgFaultIter2::_get_ref(self);
  try {
    if ( val.has_next() ) {
      return PyTpgFault::ToPyObject(val.next());
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


// @brief TpgFaultIter2 オブジェクトを使用可能にする．
bool
PyTpgFaultIter2::init(
  PyObject* m
)
{
  TpgFaultIter2_Type.tp_name = "TpgFaultIter2";
  TpgFaultIter2_Type.tp_basicsize = sizeof(TpgFaultIter2_Object);
  TpgFaultIter2_Type.tp_itemsize = 0;
  TpgFaultIter2_Type.tp_dealloc = dealloc_func;
  TpgFaultIter2_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgFaultIter2_Type.tp_doc = PyDoc_STR("Python extended object for TpgFaultIter2");
  TpgFaultIter2_Type.tp_iter = iter_func;
  TpgFaultIter2_Type.tp_iternext = iternext_func;
  if ( !PyModule::reg_type(m, "TpgFaultIter2", &TpgFaultIter2_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgFaultIter2 を PyObject に変換する．
PyObject*
PyTpgFaultIter2::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgFaultIter2::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgFaultIter2_Object*>(obj);
  new (&my_obj->mVal) TpgFaultIter2(val);
  return obj;
}

// @brief PyObject が TpgFaultIter2 タイプか調べる．
bool
PyTpgFaultIter2::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgFaultIter2_Type);
}

// @brief PyObject から TpgFaultIter2 を取り出す．
TpgFaultIter2&
PyTpgFaultIter2::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgFaultIter2_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgFaultIter2 を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgFaultIter2::_typeobject()
{
  return &TpgFaultIter2_Type;
}

END_NAMESPACE_DRUID
