
/// @file PyTpgNode.cc
/// @brief PyTpgNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgNode.h"
#include "pym/PyTpgNodeList.h"
#include "pym/PyVal3.h"
#include "pym/PyPrimType.h"
#include "pym/PyLong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgNode_Object
{
  PyObject_HEAD
  TpgNode mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgNode_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgNode::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgNode_Object*>(self);
  obj->mVal.~TpgNode();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
is_valid(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_valid());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
fanin(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "l",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.fanin(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
fanin_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyTpgNodeList::ToPyObject(val.fanin_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
fanout(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "l",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.fanout(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
fanout_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyTpgNodeList::ToPyObject(val.fanout_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_primary_input(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_primary_input());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_primary_output(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_primary_output());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_dff_input(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_dff_input());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_dff_output(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_dff_output());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_ppi(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_ppi());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_ppo(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_ppo());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_logic(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_logic());
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
  {"is_valid",
   is_valid,
   METH_NOARGS,
   PyDoc_STR("True if valid")},
  {"fanin",
   reinterpret_cast<PyCFunction>(fanin),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return fanin node")},
  {"fanin_list",
   fanin_list,
   METH_NOARGS,
   PyDoc_STR("return list of fanin nodes")},
  {"fanout",
   reinterpret_cast<PyCFunction>(fanout),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return fanout node")},
  {"fanout_list",
   fanout_list,
   METH_NOARGS,
   PyDoc_STR("return list of fanout nodes")},
  {"is_primary_input",
   is_primary_input,
   METH_NOARGS,
   PyDoc_STR("True if Primary Input")},
  {"is_primary_output",
   is_primary_output,
   METH_NOARGS,
   PyDoc_STR("True if Primary Output")},
  {"is_dff_input",
   is_dff_input,
   METH_NOARGS,
   PyDoc_STR("True if DFF Input")},
  {"is_dff_output",
   is_dff_output,
   METH_NOARGS,
   PyDoc_STR("True if DFF Output")},
  {"is_ppi",
   is_ppi,
   METH_NOARGS,
   PyDoc_STR("True if Pseudo Primary Input")},
  {"is_ppo",
   is_ppo,
   METH_NOARGS,
   PyDoc_STR("True if Pseudo Primary Output")},
  {"is_logic",
   is_logic,
   METH_NOARGS,
   PyDoc_STR("True if Logic type")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
get_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyLong::ToPyObject(val.id());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_fanin_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyLong::ToPyObject(val.fanin_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_fanout_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyLong::ToPyObject(val.fanout_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_ffr_root(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.ffr_root());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_mffc_root(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.mffc_root());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_imm_dom(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.imm_dom());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_input_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyLong::ToPyObject(val.input_id());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_output_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyLong::ToPyObject(val.output_id());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_output_id2(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyLong::ToPyObject(val.output_id2());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_dff_id(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyLong::ToPyObject(val.dff_id());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_alt_node(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.alt_node());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_gate_type(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyPrimType::ToPyObject(val.gate_type());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_cval(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyVal3::ToPyObject(val.cval());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_nval(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyVal3::ToPyObject(val.nval());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_coval(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyVal3::ToPyObject(val.coval());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_noval(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyTpgNode::_get_ref(self);
  try {
    return PyVal3::ToPyObject(val.noval());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// getter/setter定義
PyGetSetDef getsets[] = {
  {"id", get_id, nullptr, PyDoc_STR(""), nullptr},
  {"fanin_num", get_fanin_num, nullptr, PyDoc_STR(""), nullptr},
  {"fanout_num", get_fanout_num, nullptr, PyDoc_STR(""), nullptr},
  {"ffr_root", get_ffr_root, nullptr, PyDoc_STR(""), nullptr},
  {"mffc_root", get_mffc_root, nullptr, PyDoc_STR(""), nullptr},
  {"imm_dom", get_imm_dom, nullptr, PyDoc_STR(""), nullptr},
  {"input_id", get_input_id, nullptr, PyDoc_STR(""), nullptr},
  {"output_id", get_output_id, nullptr, PyDoc_STR(""), nullptr},
  {"output_id2", get_output_id2, nullptr, PyDoc_STR(""), nullptr},
  {"dff_id", get_dff_id, nullptr, PyDoc_STR(""), nullptr},
  {"alt_node", get_alt_node, nullptr, PyDoc_STR(""), nullptr},
  {"gate_type", get_gate_type, nullptr, PyDoc_STR(""), nullptr},
  {"cval", get_cval, nullptr, PyDoc_STR(""), nullptr},
  {"nval", get_nval, nullptr, PyDoc_STR(""), nullptr},
  {"coval", get_coval, nullptr, PyDoc_STR(""), nullptr},
  {"noval", get_noval, nullptr, PyDoc_STR(""), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief TpgNode オブジェクトを使用可能にする．
bool
PyTpgNode::init(
  PyObject* m
)
{
  TpgNode_Type.tp_name = "TpgNode";
  TpgNode_Type.tp_basicsize = sizeof(TpgNode_Object);
  TpgNode_Type.tp_itemsize = 0;
  TpgNode_Type.tp_dealloc = dealloc_func;
  TpgNode_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgNode_Type.tp_doc = PyDoc_STR("Python extended object for TpgNode");
  TpgNode_Type.tp_methods = methods;
  TpgNode_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "TpgNode", &TpgNode_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgNode を PyObject に変換する．
PyObject*
PyTpgNode::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgNode::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgNode_Object*>(obj);
  new (&my_obj->mVal) TpgNode(val);
  return obj;
}

// PyObject を TpgNode に変換する．
bool
PyTpgNode::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyTpgNode::Check(obj) ) {
    val = PyTpgNode::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が TpgNode タイプか調べる．
bool
PyTpgNode::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgNode_Type);
}

// @brief PyObject から TpgNode を取り出す．
TpgNode&
PyTpgNode::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgNode_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgNode を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgNode::_typeobject()
{
  return &TpgNode_Type;
}

END_NAMESPACE_DRUID
