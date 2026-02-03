
/// @file PyTpgNetwork.cc
/// @brief PyTpgNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgNetwork.h"
#include "pym/PyTpgNode.h"
#include "pym/PyTpgNodeList.h"
#include "pym/PyTpgFault.h"
#include "pym/PyTpgFaultList.h"
#include "pym/PyTpgMFFC.h"
#include "pym/PyTpgMFFCList.h"
#include "pym/PyTpgFFR.h"
#include "pym/PyTpgFFRList.h"
#include "pym/PyTpgGate.h"
#include "pym/PyTpgGateList.h"
#include "pym/PyBnModel.h"
#include "pym/PyFaultType.h"
#include "pym/pyfstream.h"
#include "pym/PyUlong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgNetwork_Object
{
  PyObject_HEAD
  TpgNetwork mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgNetwork_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgNetwork::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgNetwork_Object*>(self);
  obj->mVal.~TpgNetwork();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
from_bn(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "model",
    "fault_type",
    nullptr
  };
  PyObject* model_obj = nullptr;
  PyObject* fault_type_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!",
                                    const_cast<char**>(kwlist),
                                    PyBnModel::_typeobject(), &model_obj,
                                    PyFaultType::_typeobject(), &fault_type_obj) ) {
    return nullptr;
  }
  BnModel model;
  if ( model_obj != nullptr ) {
    if ( !PyBnModel::FromPyObject(model_obj, model) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to BnModel");
      return nullptr;
    }
  }
  FaultType fault_type = FaultType::None;
  if ( fault_type_obj != nullptr ) {
    if ( !PyFaultType::FromPyObject(fault_type_obj, fault_type) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to FaultType");
      return nullptr;
    }
  }
  try {
    return PyTpgNetwork::ToPyObject(TpgNetwork::from_bn(model, fault_type));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
read_blif(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "filename",
    "fault_type",
    nullptr
  };
  const char* filename_tmp = nullptr;
  PyObject* fault_type_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO!",
                                    const_cast<char**>(kwlist),
                                    &filename_tmp,
                                    PyFaultType::_typeobject(), &fault_type_obj) ) {
    return nullptr;
  }
  std::string filename;
  if ( filename_tmp != nullptr ) {
    filename = std::string(filename_tmp);
  }
  FaultType fault_type = FaultType::None;
  if ( fault_type_obj != nullptr ) {
    if ( !PyFaultType::FromPyObject(fault_type_obj, fault_type) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to FaultType");
      return nullptr;
    }
  }
  try {
    return PyTpgNetwork::ToPyObject(TpgNetwork::read_blif(filename, fault_type));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_valid(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
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
node_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.node_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
node(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.node(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
node_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNodeList::ToPyObject(val.node_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
input_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.input_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
input(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.input(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
input_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNodeList::ToPyObject(val.input_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
output_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.output_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
output(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.output(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
output_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNodeList::ToPyObject(val.output_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
output2(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.output2(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ppi_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.ppi_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ppi(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.ppi(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ppi_name(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.ppi(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ppi_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNodeList::ToPyObject(val.ppi_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ppo_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.ppo_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ppo(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.ppo(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ppo_name(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.ppo(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ppo_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNodeList::ToPyObject(val.ppo_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
mffc_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.mffc_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
mffc(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgMFFC::ToPyObject(val.mffc(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
mffc_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgMFFCList::ToPyObject(val.mffc_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ffr_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.ffr_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ffr(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgFFR::ToPyObject(val.ffr(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ffr_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgFFRList::ToPyObject(val.ffr_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
dff_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.dff_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
dff_input(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.dff_input(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
dff_output(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgNode::ToPyObject(val.dff_output(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
gate_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.gate_num());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
gate(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "pos",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgGate::ToPyObject(val.gate(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
gate_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgGateList::ToPyObject(val.gate_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
fault_type(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyFaultType::ToPyObject(val.fault_type());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
has_prev_state(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyBool_FromLong(val.has_prev_state());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
rep_fault_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgFaultList::ToPyObject(val.rep_fault_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
max_fault_id(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.max_fault_id());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
fault(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "fault_id",
    nullptr
  };
  unsigned long pos;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "k",
                                    const_cast<char**>(kwlist),
                                    &pos) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    return PyTpgFault::ToPyObject(val.fault(pos));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
print(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "fout",
    nullptr
  };
  PyObject* fout_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
                                    const_cast<char**>(kwlist),
                                    &fout_obj) ) {
    return nullptr;
  }
  auto& val = PyTpgNetwork::_get_ref(self);
  try {
    auto s = opyfstream(fout_obj);
    val.print(s);
    Py_RETURN_NONE;
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
  {"from_bn",
   reinterpret_cast<PyCFunction>(from_bn),
   METH_VARARGS | METH_KEYWORDS | METH_STATIC,
   PyDoc_STR("BnModel から変換する．\n"
             ":param BnModel model: 元となる BnModel")},
  {"read_blif",
   reinterpret_cast<PyCFunction>(read_blif),
   METH_VARARGS | METH_KEYWORDS | METH_STATIC,
   PyDoc_STR("'blif' ファイルを読み込む．\n"
             ":param str filename: blif ファイル名")},
  {"is_valid",
   is_valid,
   METH_NOARGS,
   PyDoc_STR("適正な値を持つ時 True を返す．")},
  {"node_num",
   node_num,
   METH_NOARGS,
   PyDoc_STR("ノード数を返す．")},
  {"node",
   reinterpret_cast<PyCFunction>(node),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("ノード返す．\n"
             ":param int pos: 位置番号 ( 0 <= pos < node_num() )")},
  {"node_list",
   node_list,
   METH_NOARGS,
   PyDoc_STR("ノードのリストを返す．")},
  {"input_num",
   input_num,
   METH_NOARGS,
   PyDoc_STR("入力数を返す．")},
  {"input",
   reinterpret_cast<PyCFunction>(input),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("入力ノードを返す．\n"
             ":param int pos: 入力番号 ( 0 <= pos < input_num() )")},
  {"input_list",
   input_list,
   METH_NOARGS,
   PyDoc_STR("入力ノードのリストを返す．")},
  {"output_num",
   output_num,
   METH_NOARGS,
   PyDoc_STR("出力数を返す．")},
  {"output",
   reinterpret_cast<PyCFunction>(output),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("出力ノードを返す．\n"
             ":param int pos: 出力番号 ( 0 <= pos < output_num() )")},
  {"output_list",
   output_list,
   METH_NOARGS,
   PyDoc_STR("出力ノードのリストを返す．")},
  {"output2",
   reinterpret_cast<PyCFunction>(output2),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("出力ノードを返す．(サイズの昇順)\n"
             ":param int pos: 出力番号 ( 0 <= pos < output_num() )")},
  {"ppi_num",
   ppi_num,
   METH_NOARGS,
   PyDoc_STR("PPI数(入力数+DFF数)を返す．")},
  {"ppi",
   reinterpret_cast<PyCFunction>(ppi),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("PPIノードを返す．\n"
             ":param int pos: PPI番号 ( 0 <= pos < ppi_num() )")},
  {"ppi_name",
   reinterpret_cast<PyCFunction>(ppi_name),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("PPI名を返す．\n"
             ":param int pos: PPI番号 ( 0 <= pos < ppi_num() )")},
  {"ppi_list",
   ppi_list,
   METH_NOARGS,
   PyDoc_STR("PPIノードのリストを返す．")},
  {"ppo_num",
   ppo_num,
   METH_NOARGS,
   PyDoc_STR("PPO数(出力数+DFF数)を返す．")},
  {"ppo",
   reinterpret_cast<PyCFunction>(ppo),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("PPOノードを返す．\n"
             ":param int pos: PPO番号 ( 0 <= pos < ppo_num() )")},
  {"ppo_name",
   reinterpret_cast<PyCFunction>(ppo_name),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("PPO名を返す．\n"
             ":param int pos: PPO番号 ( 0 <= pos < ppo_num() )")},
  {"ppo_list",
   ppo_list,
   METH_NOARGS,
   PyDoc_STR("PPOノードのリストを返す．")},
  {"mffc_num",
   mffc_num,
   METH_NOARGS,
   PyDoc_STR("MFFC数を返す．")},
  {"mffc",
   reinterpret_cast<PyCFunction>(mffc),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("MFFCを返す．\n"
             ":param int pos: MFFC番号 ( 0 <= pos < mffc_num() )")},
  {"mffc_list",
   mffc_list,
   METH_NOARGS,
   PyDoc_STR("MFFCのリストを返す．")},
  {"ffr_num",
   ffr_num,
   METH_NOARGS,
   PyDoc_STR("FFR数を返す．")},
  {"ffr",
   reinterpret_cast<PyCFunction>(ffr),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("FFRを返す．\n"
             ":param int pos: FFR番号 ( 0 <= pos < ffr_num() )")},
  {"ffr_list",
   ffr_list,
   METH_NOARGS,
   PyDoc_STR("FFRのリストを返す．")},
  {"dff_num",
   dff_num,
   METH_NOARGS,
   PyDoc_STR("DFF数を返す．")},
  {"dff_input",
   reinterpret_cast<PyCFunction>(dff_input),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("DFFの入力ノードを返す．\n"
             ":param int pos: DFF番号 ( 0 <= pos < dff_num() )")},
  {"dff_output",
   reinterpret_cast<PyCFunction>(dff_output),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("DFFの出力ノードを返す．\n"
             ":param int pos: DFF番号 ( 0 <= pos < dff_num() )")},
  {"gate_num",
   gate_num,
   METH_NOARGS,
   PyDoc_STR("ゲート数を返す．")},
  {"gate",
   reinterpret_cast<PyCFunction>(gate),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("ゲートを返す．\n"
             ":param int pos: ゲート番号 ( 0 <= pos < gate_num() )")},
  {"gate_list",
   gate_list,
   METH_NOARGS,
   PyDoc_STR("ゲートのリストを返す．")},
  {"fault_type",
   fault_type,
   METH_NOARGS,
   PyDoc_STR("故障の種類(FaultType)を返す．")},
  {"has_prev_state",
   has_prev_state,
   METH_NOARGS,
   PyDoc_STR("1時刻前の状態を持つ時 True を返す．")},
  {"rep_fault_list",
   rep_fault_list,
   METH_NOARGS,
   PyDoc_STR("代表故障のリストを返す．")},
  {"max_fault_id",
   max_fault_id,
   METH_NOARGS,
   PyDoc_STR("故障番号の最大値を返す．")},
  {"fault",
   reinterpret_cast<PyCFunction>(fault),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("故障を返す．\n"
             ":param int fault_id: 故障番号 ( 0 <= fault_id <= max_fault_id() )")},
  {"print",
   reinterpret_cast<PyCFunction>(print),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("内容を出力する．\n"
             ":param file_object fout: 出力先のファイルオブジェクト")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

// new 関数
PyObject*
new_func(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    nullptr
  };
  // 余分な引数を取らないことを確認しておく．
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "", const_cast<char**>(kwlist)) ) {
    return nullptr;
  }
  try {
    auto self = type->tp_alloc(type, 0);
    return self;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
  catch ( std::out_of_range err ) {
    std::ostringstream buf;
    buf << "out of range" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

END_NONAMESPACE


// @brief TpgNetwork オブジェクトを使用可能にする．
bool
PyTpgNetwork::init(
  PyObject* m
)
{
  TpgNetwork_Type.tp_name = "TpgNetwork";
  TpgNetwork_Type.tp_basicsize = sizeof(TpgNetwork_Object);
  TpgNetwork_Type.tp_itemsize = 0;
  TpgNetwork_Type.tp_dealloc = dealloc_func;
  TpgNetwork_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgNetwork_Type.tp_doc = PyDoc_STR("基本的には TpgNode のネットワーク(DAG)を表す．\n"
                           "\n"
                           "ただし，フルスキャンの順序回路を扱うためにフリップフロップの入出力をそれぞれ疑似出力，疑似入力としてあつかう．\n"
                           "\n"
                           "- 本当の入力と疑似入力をあわせて PPI(Pseudo Primary Input) と呼ぶ．\n"
                           "- 本当の出力と疑似出力をあわせて PPO(Pseudo Primary Output) と呼ぶ．\n"
                           "\n"
                           "フリップフロップのクロック系の回路は無視される．\n"
                           "セット/リセット系の回路も無視される．");
  TpgNetwork_Type.tp_methods = methods;
  TpgNetwork_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "TpgNetwork", &TpgNetwork_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgNetwork を PyObject に変換する．
PyObject*
PyTpgNetwork::Conv::operator()(
  ElemType&& val ///< [in] 元の値
)
{
  auto type = PyTpgNetwork::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgNetwork_Object*>(obj);
  new (&my_obj->mVal) TpgNetwork(std::move(val));
  return obj;
}

// @brief PyObject が TpgNetwork タイプか調べる．
bool
PyTpgNetwork::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgNetwork_Type);
}

// @brief PyObject から TpgNetwork を取り出す．
TpgNetwork&
PyTpgNetwork::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgNetwork_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgNetwork を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgNetwork::_typeobject()
{
  return &TpgNetwork_Type;
}

END_NAMESPACE_DRUID
