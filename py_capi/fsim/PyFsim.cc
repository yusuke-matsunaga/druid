
/// @file PyFsim.cc
/// @brief PyFsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyFsim.h"
#include "pym/PyTpgNetwork.h"
#include "pym/PyTpgFault.h"
#include "pym/PyTpgFaultList.h"
#include "pym/PyTestVector.h"
#include "pym/PyAssignList.h"
#include "pym/PyDiffBits.h"
#include "pym/PyFsimResults.h"
#include "pym/PyJsonValue.h"
#include "pym/PyList.h"
#include "pym/PyBool.h"
#include "pym/PyUlong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct Fsim_Object
{
  PyObject_HEAD
  Fsim mVal;
};

// Python 用のタイプ定義
PyTypeObject Fsim_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyFsim::init() 中で初期化する．
};
#include "parse_faults.cc"

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<Fsim_Object*>(self);
  obj->mVal.~Fsim();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
set_skip_all(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyFsim::_get_ref(self);
  try {
    val.set_skip_all();
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
set_skip(
  PyObject* self,
  PyObject* args
)
{
  TpgFaultList fault_list;
  if ( !parse_faults(args, fault_list) ) {
    return nullptr;
  }
  auto& val = PyFsim::_get_ref(self);
  try {
    val.set_skip(fault_list);
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
clear_skip_all(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyFsim::_get_ref(self);
  try {
    val.clear_skip_all();
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
clear_skip(
  PyObject* self,
  PyObject* args
)
{
  TpgFaultList fault_list;
  if ( !parse_faults(args, fault_list) ) {
    return nullptr;
  }
  auto& val = PyFsim::_get_ref(self);
  try {
    val.clear_skip(fault_list);
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_skip(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "fault",
    nullptr
  };
  PyObject* fault_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyTpgFault::_typeobject(), &fault_obj) ) {
    return nullptr;
  }
  TpgFault fault;
  if ( fault_obj != nullptr ) {
    if ( !PyTpgFault::FromPyObject(fault_obj, fault) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TpgFault");
      return nullptr;
    }
  }
  auto& val = PyFsim::_get_ref(self);
  try {
    return PyBool_FromLong(val.get_skip(fault));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
spsfp(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "tv",
    "fault",
    nullptr
  };
  PyObject* tv_obj = nullptr;
  PyObject* fault_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "OO!",
                                    const_cast<char**>(kwlist),
                                    &tv_obj,
                                    PyTpgFault::_typeobject(), &fault_obj) ) {
    return nullptr;
  }
  TpgFault fault;
  if ( fault_obj != nullptr ) {
    if ( !PyTpgFault::FromPyObject(fault_obj, fault) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TpgFault");
      return nullptr;
    }
  }
  auto& val = PyFsim::_get_ref(self);
  try {
    if ( PyTestVector::Check(tv_obj) ) {
      auto& tv = PyTestVector::_get_ref(tv_obj);
      DiffBits dbits;
      auto res = val.spsfp(tv, fault, dbits);
      auto res_obj = PyBool::ToPyObject(res);
      auto dbits_obj = PyDiffBits::ToPyObject(dbits);
      return Py_BuildValue("OO", res_obj, dbits_obj);
    }
    if ( PyAssignList::Check(tv_obj) ) {
      auto& assign_list = PyAssignList::_get_ref(tv_obj);
      DiffBits dbits;
      auto res = val.spsfp(assign_list, fault, dbits);
      auto res_obj = PyBool::ToPyObject(res);
      auto dbits_obj = PyDiffBits::ToPyObject(dbits);
      return Py_BuildValue("OO", res_obj, dbits_obj);
    }
    PyErr_SetString(PyExc_TypeError, "argument 1 should be TestVector or AssignList");
    return nullptr;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
xspsfp(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "assign_list",
    "fault",
    nullptr
  };
  PyObject* assign_list_obj = nullptr;
  PyObject* fault_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!",
                                    const_cast<char**>(kwlist),
                                    PyAssignList::_typeobject(), &assign_list_obj,
                                    PyTpgFault::_typeobject(), &fault_obj) ) {
    return nullptr;
  }
  AssignList assign_list;
  if ( assign_list_obj != nullptr ) {
    if ( !PyAssignList::FromPyObject(assign_list_obj, assign_list) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to AssignList");
      return nullptr;
    }
  }
  TpgFault fault;
  if ( fault_obj != nullptr ) {
    if ( !PyTpgFault::FromPyObject(fault_obj, fault) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TpgFault");
      return nullptr;
    }
  }
  auto& val = PyFsim::_get_ref(self);
  try {
    DiffBits dbits;
    auto res = val.xspsfp(assign_list, fault, dbits);
    auto res_obj = PyBool::ToPyObject(res);
    auto dbits_obj = PyDiffBits::ToPyObject(dbits);
    return Py_BuildValue("OO", res_obj, dbits_obj);
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
sppfp(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "tv",
    nullptr
  };
  PyObject* tv_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
                                    const_cast<char**>(kwlist),
                                    &tv_obj) ) {
    return nullptr;
  }
  auto& val = PyFsim::_get_ref(self);
  try {
    if ( PyTestVector::Check(tv_obj) ) {
      auto& tv = PyTestVector::_get_ref(tv_obj);
      return PyFsimResults::ToPyObject(val.sppfp(tv));
    }
    if ( PyAssignList::Check(tv_obj) ) {
      auto& assign_list = PyAssignList::_get_ref(tv_obj);
      return PyFsimResults::ToPyObject(val.sppfp(assign_list));
    }
    PyErr_SetString(PyExc_TypeError, "argument 1 should be TestVector or AssignList");
    return nullptr;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
xsppfp(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "assign_list",
    nullptr
  };
  PyObject* assign_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyAssignList::_typeobject(), &assign_list_obj) ) {
    return nullptr;
  }
  AssignList assign_list;
  if ( assign_list_obj != nullptr ) {
    if ( !PyAssignList::FromPyObject(assign_list_obj, assign_list) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to AssignList");
      return nullptr;
    }
  }
  auto& val = PyFsim::_get_ref(self);
  try {
    return PyFsimResults::ToPyObject(val.xsppfp(assign_list));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
ppsfp(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "tv_list",
    nullptr
  };
  PyObject* tv_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
                                    const_cast<char**>(kwlist),
                                    &tv_list_obj) ) {
    return nullptr;
  }
  std::vector<TestVector> tv_list;
  if ( tv_list_obj != nullptr ) {
    if ( !PyList<TestVector, PyTestVector>::FromPyObject(tv_list_obj, tv_list) ) {
      PyErr_SetString(PyExc_ValueError, "could not convert to std::vector<TestVector>");
      return nullptr;
    }
  }
  auto& val = PyFsim::_get_ref(self);
  try {
    return PyFsimResults::ToPyObject(val.ppsfp(tv_list));
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
  {"set_skip_all",
   set_skip_all,
   METH_NOARGS,
   PyDoc_STR("set \"skip\" mark to all faults")},
  {"set_skip",
   set_skip,
   METH_VARARGS,
   PyDoc_STR("set \"skip\" mark")},
  {"clear_skip_all",
   clear_skip_all,
   METH_NOARGS,
   PyDoc_STR("clear \"skip\" mark to all faults")},
  {"clear_skip",
   clear_skip,
   METH_VARARGS,
   PyDoc_STR("clear \"skip\" mark")},
  {"get_skip",
   reinterpret_cast<PyCFunction>(get_skip),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("get \"skip\" mark")},
  {"spsfp",
   reinterpret_cast<PyCFunction>(spsfp),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("do SPSFP fault simulation")},
  {"xspsfp",
   reinterpret_cast<PyCFunction>(xspsfp),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("do SPSFP fault simulation with X values")},
  {"sppfp",
   reinterpret_cast<PyCFunction>(sppfp),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("do SPPFP fault simulation")},
  {"xsppfp",
   reinterpret_cast<PyCFunction>(xsppfp),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("do SPPFP fault simulation with X values")},
  {"ppsfp",
   reinterpret_cast<PyCFunction>(ppsfp),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("do SPPFP fault simulation with X values")},
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
    "network",
    "fault_list",
    "option",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_list_obj = nullptr;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!|$O",
                                    const_cast<char**>(kwlist),
                                    PyTpgNetwork::_typeobject(), &network_obj,
                                    PyTpgFaultList::_typeobject(), &fault_list_obj,
                                    &option_obj) ) {
    return nullptr;
  }
  auto& network = PyTpgNetwork::_get_ref(network_obj);
  TpgFaultList fault_list;
  if ( fault_list_obj != nullptr ) {
    if ( !PyTpgFaultList::FromPyObject(fault_list_obj, fault_list) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TpgFaultList");
      return nullptr;
    }
  }
  JsonValue option;
  if ( option_obj != nullptr ) {
    if ( !PyJsonValue::FromPyObject(option_obj, option) ) {
      PyErr_SetString(PyExc_ValueError, "could not convert to JsonValue");
      return nullptr;
    }
  }
  try {
    auto self = type->tp_alloc(type, 0);
    auto my_obj = reinterpret_cast<Fsim_Object*>(self);
    new (&my_obj->mVal) Fsim(network, fault_list, option);
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


// @brief Fsim オブジェクトを使用可能にする．
bool
PyFsim::init(
  PyObject* m
)
{
  Fsim_Type.tp_name = "Fsim";
  Fsim_Type.tp_basicsize = sizeof(Fsim_Object);
  Fsim_Type.tp_itemsize = 0;
  Fsim_Type.tp_dealloc = dealloc_func;
  Fsim_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  Fsim_Type.tp_doc = PyDoc_STR("Python extended object for Fsim");
  Fsim_Type.tp_methods = methods;
  Fsim_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "Fsim", &Fsim_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が Fsim タイプか調べる．
bool
PyFsim::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &Fsim_Type);
}

// @brief PyObject から Fsim を取り出す．
Fsim&
PyFsim::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<Fsim_Object*>(obj);
  return my_obj->mVal;
}

// @brief Fsim を表すオブジェクトの型定義を返す．
PyTypeObject*
PyFsim::_typeobject()
{
  return &Fsim_Type;
}

END_NAMESPACE_DRUID
