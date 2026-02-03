
/// @file PyTpgFFRIter2.cc
/// @brief PyTpgFFRIter2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgFFRIter2.h"
#include "pym/PyTpgFFR.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgFFRIter2_Object
{
  PyObject_HEAD
  TpgFFRIter2 mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgFFRIter2_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgFFRIter2::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgFFRIter2_Object*>(self);
  obj->mVal.~TpgFFRIter2();
  Py_TYPE(self)->tp_free(self);
}

// iter 関数
PyObject*
iter_func(
  PyObject* self
)
{
  auto& val = PyTpgFFRIter2::_get_ref(self);
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
  auto& val = PyTpgFFRIter2::_get_ref(self);
  try {
    if ( val.has_next() ) {
      return PyTpgFFR::ToPyObject(val.next());
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


// @brief TpgFFRIter2 オブジェクトを使用可能にする．
bool
PyTpgFFRIter2::init(
  PyObject* m
)
{
  TpgFFRIter2_Type.tp_name = "TpgFFRIter2";
  TpgFFRIter2_Type.tp_basicsize = sizeof(TpgFFRIter2_Object);
  TpgFFRIter2_Type.tp_itemsize = 0;
  TpgFFRIter2_Type.tp_dealloc = dealloc_func;
  TpgFFRIter2_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgFFRIter2_Type.tp_doc = PyDoc_STR("Python extended object for TpgFFRIter2");
  TpgFFRIter2_Type.tp_iter = iter_func;
  TpgFFRIter2_Type.tp_iternext = iternext_func;
  if ( !PyModule::reg_type(m, "TpgFFRIter2", &TpgFFRIter2_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgFFRIter2 を PyObject に変換する．
PyObject*
PyTpgFFRIter2::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgFFRIter2::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgFFRIter2_Object*>(obj);
  new (&my_obj->mVal) TpgFFRIter2(val);
  return obj;
}

// @brief PyObject が TpgFFRIter2 タイプか調べる．
bool
PyTpgFFRIter2::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgFFRIter2_Type);
}

// @brief PyObject から TpgFFRIter2 を取り出す．
TpgFFRIter2&
PyTpgFFRIter2::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgFFRIter2_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgFFRIter2 を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgFFRIter2::_typeobject()
{
  return &TpgFFRIter2_Type;
}

END_NAMESPACE_DRUID
