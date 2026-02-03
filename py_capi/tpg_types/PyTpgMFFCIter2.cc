
/// @file PyTpgMFFCIter2.cc
/// @brief PyTpgMFFCIter2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgMFFCIter2.h"
#include "pym/PyTpgMFFC.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgMFFCIter2_Object
{
  PyObject_HEAD
  TpgMFFCIter2 mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgMFFCIter2_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgMFFCIter2::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgMFFCIter2_Object*>(self);
  obj->mVal.~TpgMFFCIter2();
  Py_TYPE(self)->tp_free(self);
}

// iter 関数
PyObject*
iter_func(
  PyObject* self
)
{
  auto& val = PyTpgMFFCIter2::_get_ref(self);
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
  auto& val = PyTpgMFFCIter2::_get_ref(self);
  try {
    if ( val.has_next() ) {
      return PyTpgMFFC::ToPyObject(val.next());
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


// @brief TpgMFFCIter2 オブジェクトを使用可能にする．
bool
PyTpgMFFCIter2::init(
  PyObject* m
)
{
  TpgMFFCIter2_Type.tp_name = "TpgMFFCIter2";
  TpgMFFCIter2_Type.tp_basicsize = sizeof(TpgMFFCIter2_Object);
  TpgMFFCIter2_Type.tp_itemsize = 0;
  TpgMFFCIter2_Type.tp_dealloc = dealloc_func;
  TpgMFFCIter2_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgMFFCIter2_Type.tp_doc = PyDoc_STR("Python extended object for TpgMFFCIter2");
  TpgMFFCIter2_Type.tp_iter = iter_func;
  TpgMFFCIter2_Type.tp_iternext = iternext_func;
  if ( !PyModule::reg_type(m, "TpgMFFCIter2", &TpgMFFCIter2_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgMFFCIter2 を PyObject に変換する．
PyObject*
PyTpgMFFCIter2::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgMFFCIter2::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgMFFCIter2_Object*>(obj);
  new (&my_obj->mVal) TpgMFFCIter2(val);
  return obj;
}

// @brief PyObject が TpgMFFCIter2 タイプか調べる．
bool
PyTpgMFFCIter2::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgMFFCIter2_Type);
}

// @brief PyObject から TpgMFFCIter2 を取り出す．
TpgMFFCIter2&
PyTpgMFFCIter2::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgMFFCIter2_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgMFFCIter2 を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgMFFCIter2::_typeobject()
{
  return &TpgMFFCIter2_Type;
}

END_NAMESPACE_DRUID
