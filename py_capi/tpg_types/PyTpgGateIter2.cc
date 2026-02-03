
/// @file PyTpgGateIter2.cc
/// @brief PyTpgGateIter2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgGateIter2.h"
#include "pym/PyTpgGate.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgGateIter2_Object
{
  PyObject_HEAD
  TpgGateIter2 mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgGateIter2_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgGateIter2::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgGateIter2_Object*>(self);
  obj->mVal.~TpgGateIter2();
  Py_TYPE(self)->tp_free(self);
}

// iter 関数
PyObject*
iter_func(
  PyObject* self
)
{
  auto& val = PyTpgGateIter2::_get_ref(self);
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
  auto& val = PyTpgGateIter2::_get_ref(self);
  try {
    if ( val.has_next() ) {
      return PyTpgGate::ToPyObject(val.next());
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


// @brief TpgGateIter2 オブジェクトを使用可能にする．
bool
PyTpgGateIter2::init(
  PyObject* m
)
{
  TpgGateIter2_Type.tp_name = "TpgGateIter2";
  TpgGateIter2_Type.tp_basicsize = sizeof(TpgGateIter2_Object);
  TpgGateIter2_Type.tp_itemsize = 0;
  TpgGateIter2_Type.tp_dealloc = dealloc_func;
  TpgGateIter2_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgGateIter2_Type.tp_doc = PyDoc_STR("Python extended object for TpgGateIter2");
  TpgGateIter2_Type.tp_iter = iter_func;
  TpgGateIter2_Type.tp_iternext = iternext_func;
  if ( !PyModule::reg_type(m, "TpgGateIter2", &TpgGateIter2_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgGateIter2 を PyObject に変換する．
PyObject*
PyTpgGateIter2::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgGateIter2::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgGateIter2_Object*>(obj);
  new (&my_obj->mVal) TpgGateIter2(val);
  return obj;
}

// @brief PyObject が TpgGateIter2 タイプか調べる．
bool
PyTpgGateIter2::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgGateIter2_Type);
}

// @brief PyObject から TpgGateIter2 を取り出す．
TpgGateIter2&
PyTpgGateIter2::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgGateIter2_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgGateIter2 を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgGateIter2::_typeobject()
{
  return &TpgGateIter2_Type;
}

END_NAMESPACE_DRUID
