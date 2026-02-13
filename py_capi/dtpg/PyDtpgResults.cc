
/// @file PyDtpgResults.cc
/// @brief PyDtpgResults の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDtpgResults.h"
#include "pym/PyTpgFault.h"
#include "pym/PyTestVector.h"
#include "pym/PyAssignList.h"
#include "pym/PyFaultStatus.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct DtpgResults_Object
{
  PyObject_HEAD
  DtpgResults mVal;
};

// Python 用のタイプ定義
PyTypeObject DtpgResults_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyDtpgResults::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<DtpgResults_Object*>(self);
  obj->mVal.~DtpgResults();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
clear(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    val.clear();
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
set_detected(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "fault",
    "testvect",
    nullptr
  };
  PyObject* fault_obj = nullptr;
  PyObject* testvect_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!",
                                    const_cast<char**>(kwlist),
                                    PyTpgFault::_typeobject(), &fault_obj,
                                    PyTestVector::_typeobject(), &testvect_obj) ) {
    return nullptr;
  }
  TpgFault fault;
  if ( fault_obj != nullptr ) {
    if ( !PyTpgFault::FromPyObject(fault_obj, fault) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TpgFault");
      return nullptr;
    }
  }
  TestVector testvect;
  if ( testvect_obj != nullptr ) {
    if ( !PyTestVector::FromPyObject(testvect_obj, testvect) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TestVector");
      return nullptr;
    }
  }
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    val.set_detected(fault, testvect);
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
set_untestable(
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
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    val.set_untestable(fault);
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
status(
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
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyFaultStatus::ToPyObject(val.status(fault));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
has_testvector(
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
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyBool_FromLong(val.has_testvector(fault));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
testvector(
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
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyTestVector::ToPyObject(val.testvector(fault));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
has_assign_list(
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
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyBool_FromLong(val.has_assign_list(fault));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
assign_list(
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
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyAssignList::ToPyObject(val.assign_list(fault));
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
  {"clear",
   clear,
   METH_NOARGS,
   PyDoc_STR("clear")},
  {"set_detected",
   reinterpret_cast<PyCFunction>(set_detected),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set DETECTED")},
  {"set_untestable",
   reinterpret_cast<PyCFunction>(set_untestable),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set UNTESTABLE")},
  {"status",
   reinterpret_cast<PyCFunction>(status),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("get status")},
  {"has_testvector",
   reinterpret_cast<PyCFunction>(has_testvector),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return True if having a TestVector")},
  {"testvector",
   reinterpret_cast<PyCFunction>(testvector),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return TestVector of the fault")},
  {"has_assign_list",
   reinterpret_cast<PyCFunction>(has_assign_list),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return True if having a AssignList")},
  {"assign_list",
   reinterpret_cast<PyCFunction>(assign_list),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return AssignList of the fault")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief DtpgResults オブジェクトを使用可能にする．
bool
PyDtpgResults::init(
  PyObject* m
)
{
  DtpgResults_Type.tp_name = "DtpgResults";
  DtpgResults_Type.tp_basicsize = sizeof(DtpgResults_Object);
  DtpgResults_Type.tp_itemsize = 0;
  DtpgResults_Type.tp_dealloc = dealloc_func;
  DtpgResults_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  DtpgResults_Type.tp_doc = PyDoc_STR("Python extended object for DtpgResults");
  DtpgResults_Type.tp_methods = methods;
  if ( !PyModule::reg_type(m, "DtpgResults", &DtpgResults_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// DtpgResults を PyObject に変換する．
PyObject*
PyDtpgResults::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyDtpgResults::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<DtpgResults_Object*>(obj);
  new (&my_obj->mVal) DtpgResults(val);
  return obj;
}

// @brief PyObject が DtpgResults タイプか調べる．
bool
PyDtpgResults::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &DtpgResults_Type);
}

// @brief PyObject から DtpgResults を取り出す．
DtpgResults&
PyDtpgResults::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<DtpgResults_Object*>(obj);
  return my_obj->mVal;
}

// @brief DtpgResults を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgResults::_typeobject()
{
  return &DtpgResults_Type;
}

END_NAMESPACE_DRUID
