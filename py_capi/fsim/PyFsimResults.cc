
/// @file PyFsimResults.cc
/// @brief PyFsimResults の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyFsimResults.h"
#include "pym/PyDiffBits.h"
#include "pym/PyList.h"
#include "pym/PyUlong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct FsimResults_Object
{
  PyObject_HEAD
  FsimResults mVal;
};

// Python 用のタイプ定義
PyTypeObject FsimResults_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyFsimResults::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<FsimResults_Object*>(self);
  obj->mVal.~FsimResults();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
tv_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyFsimResults::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.tv_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
fault_list(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "tv_id",
    nullptr
  };
  unsigned long tv_id;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &tv_id) ) {
    return nullptr;
  }
  auto& val = PyFsimResults::_get_ref(self);
  try {
    return PyList<SizeType, PyUlong>::ToPyObject(val.fault_list(tv_id));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
diffbits(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "tv_id",
    "fault_id",
    nullptr
  };
  unsigned long tv_id;
  unsigned long fid;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "kk",
                                    const_cast<char**>(kwlist),
                                    &tv_id,
                                    &fid) ) {
    return nullptr;
  }
  auto& val = PyFsimResults::_get_ref(self);
  try {
    return PyDiffBits::ToPyObject(val.diffbits(tv_id, fid));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// メソッド定義
PyMethodDef methods[] = {
  {"tv_num",
   tv_num,
   METH_NOARGS,
   PyDoc_STR("return the number of TestVectors")},
  {"fault_list",
   reinterpret_cast<PyCFunction>(fault_list),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return the list of fault IDs")},
  {"diffbits",
   reinterpret_cast<PyCFunction>(diffbits),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return DiffBits of the simulation result")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief FsimResults オブジェクトを使用可能にする．
bool
PyFsimResults::init(
  PyObject* m
)
{
  FsimResults_Type.tp_name = "FsimResults";
  FsimResults_Type.tp_basicsize = sizeof(FsimResults_Object);
  FsimResults_Type.tp_itemsize = 0;
  FsimResults_Type.tp_dealloc = dealloc_func;
  FsimResults_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  FsimResults_Type.tp_doc = PyDoc_STR("Python extended object for FsimResults");
  FsimResults_Type.tp_methods = methods;
  if ( !PyModule::reg_type(m, "FsimResults", &FsimResults_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// FsimResults を PyObject に変換する．
PyObject*
PyFsimResults::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyFsimResults::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<FsimResults_Object*>(obj);
  new (&my_obj->mVal) FsimResults(val);
  return obj;
}

// PyObject を FsimResults に変換する．
bool
PyFsimResults::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyFsimResults::Check(obj) ) {
    val = PyFsimResults::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が FsimResults タイプか調べる．
bool
PyFsimResults::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &FsimResults_Type);
}

// @brief PyObject から FsimResults を取り出す．
FsimResults&
PyFsimResults::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<FsimResults_Object*>(obj);
  return my_obj->mVal;
}

// @brief FsimResults を表すオブジェクトの型定義を返す．
PyTypeObject*
PyFsimResults::_typeobject()
{
  return &FsimResults_Type;
}

END_NAMESPACE_DRUID
