
/// @file PyDtpgStats.cc
/// @brief Python DtpgStats の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDtpgStats.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DtpgStatsObject
{
  PyObject_HEAD
  DtpgStats mVal;
};

// Python 用のタイプ定義
PyTypeObject DtpgStatsType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DtpgStats_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  // 位置引数もキーワード引数もとらない．
  static const char* kw_list[] = {
    nullptr
  };
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "",
				    const_cast<char**>(kw_list)) ) {
    return nullptr;
  }
  auto obj = type->tp_alloc(type, 0);
  auto stats_obj = reinterpret_cast<DtpgStatsObject*>(obj);
  stats_obj->mVal.clear();
  return obj;
}

// 終了関数
void
DtpgStats_dealloc(
  PyObject* self
)
{
  auto dtpg_obj = reinterpret_cast<DtpgStatsObject*>(self);
  // 特に DtpgStats 用の終了処理は必要ない．
  Py_TYPE(self)->tp_free(self);
}

PyObject*
DtpgStats_update_det(
  PyObject* self,
  PyObject* args
)
{
  double sat_time = 0.0;
  double backtrace_time = 0.0;
  if ( !PyArg_ParseTuple(args, "dd", &sat_time, &backtrace_time) ) {
    return nullptr;
  }
  auto stats_obj = reinterpret_cast<DtpgStatsObject*>(self);
  auto& stats = stats_obj->mVal;
  stats.update_det(sat_time, backtrace_time);
  Py_RETURN_NONE;
}

PyObject*
DtpgStats_update_untest(
  PyObject* self,
  PyObject* args
)
{
  double sat_time = 0.0;
  if ( !PyArg_ParseTuple(args, "d", &sat_time) ) {
    return nullptr;
  }
  auto stats_obj = reinterpret_cast<DtpgStatsObject*>(self);
  auto& stats = stats_obj->mVal;
  stats.update_untest(sat_time);
  Py_RETURN_NONE;
}

PyObject*
DtpgStats_update_abort(
  PyObject* self,
  PyObject* args
)
{
  double sat_time = 0.0;
  if ( !PyArg_ParseTuple(args, "d", &sat_time) ) {
    return nullptr;
  }
  auto stats_obj = reinterpret_cast<DtpgStatsObject*>(self);
  auto& stats = stats_obj->mVal;
  stats.update_abort(sat_time);
  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef DtpgStats_methods[] = {
  {"update_det", DtpgStats_update_det, METH_VARARGS,
   PyDoc_STR("update detected stats")},
  {"update_untest", DtpgStats_update_untest, METH_VARARGS,
   PyDoc_STR("update untestable stats")},
  {"update_abort", DtpgStats_update_abort, METH_VARARGS,
   PyDoc_STR("update aborted stats")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
DtpgStats_total_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& stats = PyDtpgStats::_get_ref(self);
  return Py_BuildValue("i", stats.total_count());
}

PyObject*
DtpgStats_detect_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& stats = PyDtpgStats::_get_ref(self);
  return Py_BuildValue("i", stats.detect_count());
}

PyObject*
DtpgStats_detect_time(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& stats = PyDtpgStats::_get_ref(self);
  return Py_BuildValue("d", stats.detect_time());
}

PyObject*
DtpgStats_untest_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& stats = PyDtpgStats::_get_ref(self);
  return Py_BuildValue("i", stats.untest_count());
}

PyObject*
DtpgStats_untest_time(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& stats = PyDtpgStats::_get_ref(self);
  return Py_BuildValue("d", stats.untest_time());
}

PyObject*
DtpgStats_abort_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& stats = PyDtpgStats::_get_ref(self);
  return Py_BuildValue("i", stats.abort_count());
}

PyObject*
DtpgStats_abort_time(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& stats = PyDtpgStats::_get_ref(self);
  return Py_BuildValue("d", stats.abort_time());
}

PyObject*
DtpgStats_cnfgen_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& stats = PyDtpgStats::_get_ref(self);
  return Py_BuildValue("i", stats.cnfgen_count());
}

PyObject*
DtpgStats_cnfgen_time(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& stats = PyDtpgStats::_get_ref(self);
  return Py_BuildValue("d", stats.cnfgen_time());
}

PyObject*
DtpgStats_backtrace_time(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& stats = PyDtpgStats::_get_ref(self);
  return Py_BuildValue("d", stats.backtrace_time());
}

PyGetSetDef DtpgStats_getset[] = {
  {"total_count", DtpgStats_total_count, nullptr,
   PyDoc_STR("total count"), nullptr},
  {"detect_count", DtpgStats_detect_count, nullptr,
   PyDoc_STR("detected time"), nullptr},
  {"detect_time", DtpgStats_detect_time, nullptr,
   PyDoc_STR("detected count"), nullptr},
  {"untest_count", DtpgStats_untest_count, nullptr,
   PyDoc_STR("untestable time"), nullptr},
  {"untest_time", DtpgStats_untest_time, nullptr,
   PyDoc_STR("untestable count"), nullptr},
  {"abort_count", DtpgStats_abort_count, nullptr,
   PyDoc_STR("aborted count"), nullptr},
  {"abort_time", DtpgStats_abort_time, nullptr,
   PyDoc_STR("aborted time"), nullptr},
  {"cnfgen_count", DtpgStats_cnfgen_count, nullptr,
   PyDoc_STR("CNF-gen count"), nullptr},
  {"cnfgen_time", DtpgStats_cnfgen_time, nullptr,
   PyDoc_STR("CNF-gen time"), nullptr},
  {"backtrace_time", DtpgStats_backtrace_time, nullptr,
   PyDoc_STR("backtrace time"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief 'DtpgStats' オブジェクトを使用可能にする．
bool
PyDtpgStats::init(
  PyObject* m
)
{
  DtpgStatsType.tp_name = "DtpgStats";
  DtpgStatsType.tp_basicsize = sizeof(DtpgStatsObject);
  DtpgStatsType.tp_itemsize = 0;
  DtpgStatsType.tp_dealloc = DtpgStats_dealloc;
  DtpgStatsType.tp_flags = Py_TPFLAGS_DEFAULT;
  DtpgStatsType.tp_doc = PyDoc_STR("DtpgStats object");
  DtpgStatsType.tp_methods = DtpgStats_methods;
  DtpgStatsType.tp_getset = DtpgStats_getset;
  DtpgStatsType.tp_new = DtpgStats_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "DtpgStats", &DtpgStatsType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief DtpgStats を表す PyObject を作る．
PyObject*
PyDtpgStats::ToPyObject(
  const DtpgStats& val
)
{
  auto obj = DtpgStatsType.tp_alloc(&DtpgStatsType, 0);
  auto stats_obj = reinterpret_cast<DtpgStatsObject*>(obj);
  stats_obj->mVal = val;
  return obj;
}

// @brief PyObject が DtpgStats タイプか調べる．
bool
PyDtpgStats::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DtpgStats を表す PyObject から DtpgStats を取り出す．
const DtpgStats&
PyDtpgStats::_get_ref(
  PyObject* obj
)
{
  auto dtpg_obj = reinterpret_cast<DtpgStatsObject*>(obj);
  return dtpg_obj->mVal;
}

// @brief DtpgStats を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgStats::_typeobject()
{
  return &DtpgStatsType;
}

END_NAMESPACE_DRUID
