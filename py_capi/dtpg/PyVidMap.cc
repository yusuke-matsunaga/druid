
/// @file PyVidMap.cc
/// @brief PyVidMap の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyVidMap.h"
#include "pym/PyTpgNode.h"
#include "pym/PySatLiteral.h"
#include "pym/PyUlong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct VidMap_Object
{
  PyObject_HEAD
  VidMap mVal;
};

// Python 用のタイプ定義
PyTypeObject VidMap_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyVidMap::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<VidMap_Object*>(self);
  obj->mVal.~VidMap();
  Py_TYPE(self)->tp_free(self);
}

// return SAT literal related to the node
PyObject*
get(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "node",
    nullptr
  };
  PyObject* node_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyTpgNode::_typeobject(), &node_obj) ) {
    return nullptr;
  }
  TpgNode node;
  if ( node_obj != nullptr ) {
    if ( !PyTpgNode::FromPyObject(node_obj, node) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TpgNode");
      return nullptr;
    }
  }
  auto& val = PyVidMap::_get_ref(self);
  try {
    return PySatLiteral::ToPyObject(val(node));
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
  {"get",
   reinterpret_cast<PyCFunction>(get),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return SAT literal related to the node")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief VidMap オブジェクトを使用可能にする．
bool
PyVidMap::init(
  PyObject* m
)
{
  VidMap_Type.tp_name = "VidMap";
  VidMap_Type.tp_basicsize = sizeof(VidMap_Object);
  VidMap_Type.tp_itemsize = 0;
  VidMap_Type.tp_dealloc = dealloc_func;
  VidMap_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  VidMap_Type.tp_doc = PyDoc_STR("Python extended object for VidMap");
  VidMap_Type.tp_methods = methods;
  if ( !PyModule::reg_type(m, "VidMap", &VidMap_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// PyObject を VidMap に変換する．
bool
PyVidMap::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyVidMap::Check(obj) ) {
    val = PyVidMap::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が VidMap タイプか調べる．
bool
PyVidMap::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &VidMap_Type);
}

// @brief PyObject から VidMap を取り出す．
VidMap&
PyVidMap::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<VidMap_Object*>(obj);
  return my_obj->mVal;
}

// @brief VidMap を表すオブジェクトの型定義を返す．
PyTypeObject*
PyVidMap::_typeobject()
{
  return &VidMap_Type;
}

END_NAMESPACE_DRUID
