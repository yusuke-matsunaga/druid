
/// @file PyDtpgStats.cc
/// @brief PyDtpgStats の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDtpgStats.h"
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
struct DtpgStats_Object
{
  PyObject_HEAD
  DtpgStats mVal;
};

// Python 用のタイプ定義
PyTypeObject DtpgStats_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyDtpgStats::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<DtpgStats_Object*>(self);
  obj->mVal.~DtpgStats();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
clear(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
merge(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "src",
    nullptr
  };
  PyObject* src_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyDtpgStats::_typeobject(), &src_obj) ) {
    return nullptr;
  }
  DtpgStats src;
  if ( src_obj != nullptr ) {
    if ( !PyDtpgStats::FromPyObject(src_obj, src) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to DtpgStats");
      return nullptr;
    }
  }
  auto& val = PyDtpgStats::_get_ref(self);
  try {
    val.merge(src);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  {"merge",
   reinterpret_cast<PyCFunction>(merge),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("merge")},
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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
  auto& val = PyDtpgStats::_get_ref(self);
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


// @brief DtpgStats オブジェクトを使用可能にする．
bool
PyDtpgStats::init(
  PyObject* m
)
{
  DtpgStats_Type.tp_name = "DtpgStats";
  DtpgStats_Type.tp_basicsize = sizeof(DtpgStats_Object);
  DtpgStats_Type.tp_itemsize = 0;
  DtpgStats_Type.tp_dealloc = dealloc_func;
  DtpgStats_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  DtpgStats_Type.tp_doc = PyDoc_STR("Python extended object for DtpgStats");
  DtpgStats_Type.tp_methods = methods;
  DtpgStats_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "DtpgStats", &DtpgStats_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// DtpgStats を PyObject に変換する．
PyObject*
PyDtpgStats::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyDtpgStats::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<DtpgStats_Object*>(obj);
  new (&my_obj->mVal) DtpgStats(val);
  return obj;
}

// PyObject を DtpgStats に変換する．
bool
PyDtpgStats::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyDtpgStats::Check(obj) ) {
    val = PyDtpgStats::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が DtpgStats タイプか調べる．
bool
PyDtpgStats::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &DtpgStats_Type);
}

// @brief PyObject から DtpgStats を取り出す．
DtpgStats&
PyDtpgStats::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<DtpgStats_Object*>(obj);
  return my_obj->mVal;
}

// @brief DtpgStats を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgStats::_typeobject()
{
  return &DtpgStats_Type;
}

END_NAMESPACE_DRUID
