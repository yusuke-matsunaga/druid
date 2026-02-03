
/// @file PyTpgNodeIter2.cc
/// @brief PyTpgNodeIter2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgNodeIter2.h"
#include "pym/PyTpgNode.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgNodeIter2_Object
{
  PyObject_HEAD
  TpgNodeIter2 mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgNodeIter2_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgNodeIter2::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgNodeIter2_Object*>(self);
  obj->mVal.~TpgNodeIter2();
  Py_TYPE(self)->tp_free(self);
}

// iter 関数
PyObject*
iter_func(
  PyObject* self
)
{
  auto& val = PyTpgNodeIter2::_get_ref(self);
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
  auto& val = PyTpgNodeIter2::_get_ref(self);
  try {
    if ( val.has_next() ) {
      return PyTpgNode::ToPyObject(val.next());
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


// @brief TpgNodeIter2 オブジェクトを使用可能にする．
bool
PyTpgNodeIter2::init(
  PyObject* m
)
{
  TpgNodeIter2_Type.tp_name = "TpgNodeIter2";
  TpgNodeIter2_Type.tp_basicsize = sizeof(TpgNodeIter2_Object);
  TpgNodeIter2_Type.tp_itemsize = 0;
  TpgNodeIter2_Type.tp_dealloc = dealloc_func;
  TpgNodeIter2_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgNodeIter2_Type.tp_doc = PyDoc_STR("Python extended object for TpgNodeIter2");
  TpgNodeIter2_Type.tp_iter = iter_func;
  TpgNodeIter2_Type.tp_iternext = iternext_func;
  if ( !PyModule::reg_type(m, "TpgNodeIter2", &TpgNodeIter2_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgNodeIter2 を PyObject に変換する．
PyObject*
PyTpgNodeIter2::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgNodeIter2::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgNodeIter2_Object*>(obj);
  new (&my_obj->mVal) TpgNodeIter2(val);
  return obj;
}

// @brief PyObject が TpgNodeIter2 タイプか調べる．
bool
PyTpgNodeIter2::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgNodeIter2_Type);
}

// @brief PyObject から TpgNodeIter2 を取り出す．
TpgNodeIter2&
PyTpgNodeIter2::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgNodeIter2_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgNodeIter2 を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgNodeIter2::_typeobject()
{
  return &TpgNodeIter2_Type;
}

END_NAMESPACE_DRUID
