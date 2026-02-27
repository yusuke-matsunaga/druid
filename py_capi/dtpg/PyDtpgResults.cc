
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
#include "pym/PySatStats.h"
#include "pym/PyUlong.h"
#include "pym/PyFloat.h"
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
    "assign_list",
    "testvect",
    nullptr
  };
  PyObject* fault_obj = nullptr;
  PyObject* as_list_obj = nullptr;
  PyObject* testvect_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!O!",
                                    const_cast<char**>(kwlist),
                                    PyTpgFault::_typeobject(), &fault_obj,
                                    PyAssignList::_typeobject(), &as_list_obj,
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
  AssignList as_list;
  if ( as_list_obj != nullptr ) {
    if ( !PyAssignList::FromPyObject(as_list_obj, as_list) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to AssignList");
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
    val.set_detected(fault, as_list, testvect);
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
update_det(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "sat_time",
    "backtrace_time",
    nullptr
  };
  double sat_time;
  double backtrace_time;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "dd",
                                    const_cast<char**>(kwlist),
                                    &sat_time,
                                    &backtrace_time) ) {
    return nullptr;
  }
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    val.update_det(sat_time, backtrace_time);
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
update_untest(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "time",
    nullptr
  };
  double time;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "d",
                                    const_cast<char**>(kwlist),
                                    &time) ) {
    return nullptr;
  }
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    val.update_untest(time);
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
update_abort(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "time",
    nullptr
  };
  double time;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "d",
                                    const_cast<char**>(kwlist),
                                    &time) ) {
    return nullptr;
  }
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    val.update_abort(time);
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
update_cnf(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "time",
    nullptr
  };
  double time;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "d",
                                    const_cast<char**>(kwlist),
                                    &time) ) {
    return nullptr;
  }
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    val.update_cnf(time);
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
update_sat_stats(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "src_stats",
    nullptr
  };
  PyObject* src_stats_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PySatStats::_typeobject(), &src_stats_obj) ) {
    return nullptr;
  }
  SatStats src_stats;
  if ( src_stats_obj != nullptr ) {
    if ( !PySatStats::FromPyObject(src_stats_obj, src_stats) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatStats");
      return nullptr;
    }
  }
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    val.update_sat_stats(src_stats);
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
  {"assign_list",
   reinterpret_cast<PyCFunction>(assign_list),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return AssignList of the fault")},
  {"testvector",
   reinterpret_cast<PyCFunction>(testvector),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return TestVector of the fault")},
  {"update_det",
   reinterpret_cast<PyCFunction>(update_det),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("update statistics for detected faults")},
  {"update_untest",
   reinterpret_cast<PyCFunction>(update_untest),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("update statistics for untestable faults")},
  {"update_abort",
   reinterpret_cast<PyCFunction>(update_abort),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("update statistics for aborted faults")},
  {"update_cnf",
   reinterpret_cast<PyCFunction>(update_cnf),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("update statistics for CNF generation")},
  {"update_sat_stats",
   reinterpret_cast<PyCFunction>(update_sat_stats),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("update SAT statictics")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
get_total_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.total_count());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_detect_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.detect_count());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_detect_time(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyFloat::ToPyObject(val.detect_time());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_untest_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.untest_count());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_untest_time(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyFloat::ToPyObject(val.untest_time());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_abort_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.abort_count());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_abort_time(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyFloat::ToPyObject(val.abort_time());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_cnfgen_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyUlong::ToPyObject(val.cnfgen_count());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_cnfgen_time(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyFloat::ToPyObject(val.cnfgen_time());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_sat_stats(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PySatStats::ToPyObject(val.sat_stats());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_sat_stats_max(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PySatStats::ToPyObject(val.sat_stats_max());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_backtrace_time(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResults::_get_ref(self);
  try {
    return PyFloat::ToPyObject(val.backtrace_time());
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
  {"total_count", get_total_count, nullptr, PyDoc_STR("the number of total faults"), nullptr},
  {"detect_count", get_detect_count, nullptr, PyDoc_STR("the number of detected faults"), nullptr},
  {"detect_time", get_detect_time, nullptr, PyDoc_STR("CPU time for detected faults"), nullptr},
  {"untest_count", get_untest_count, nullptr, PyDoc_STR("the number of untestable faults"), nullptr},
  {"untest_time", get_untest_time, nullptr, PyDoc_STR("CPU time for untestable faults"), nullptr},
  {"abort_count", get_abort_count, nullptr, PyDoc_STR("the number of aborted faults"), nullptr},
  {"abort_time", get_abort_time, nullptr, PyDoc_STR("CPU time for aborted faults"), nullptr},
  {"cnfgen_count", get_cnfgen_count, nullptr, PyDoc_STR("CNF generation count"), nullptr},
  {"cnfgen_time", get_cnfgen_time, nullptr, PyDoc_STR("CPU time for CNF generation"), nullptr},
  {"sat_stats", get_sat_stats, nullptr, PyDoc_STR("SAT statictics"), nullptr},
  {"sat_stats_max", get_sat_stats_max, nullptr, PyDoc_STR("max SAT statictics"), nullptr},
  {"backtrace_time", get_backtrace_time, nullptr, PyDoc_STR("CPU time for backtrace"), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
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
  DtpgResults_Type.tp_getset = getsets;
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
