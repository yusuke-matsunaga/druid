
/// @file PyDtpgMgr.cc
/// @brief Python DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "PyDtpgMgr.h"
#include "pym/PyModule.h"
#include "PyFsim.h"
#include "PyTpgNetwork.h"
#include "PyTpgFaultMgr.h"
#include "PyTestVector.h"
#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DtpgMgrObject
{
  PyObject_HEAD
  DtpgMgr* mVal;
};

// Python 用のタイプ定義
PyTypeObject DtpgMgrType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DtpgMgr_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kw_list[] = {
    "network",
    "fault_mgr",
    "dtpg_type",
    "just_type",
    "sat_type",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_mgr_obj = nullptr;
  const char* dtpg_type_str = nullptr;
  const char* just_type_str = nullptr;
  const char* sat_type_str = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!s|ss",
				    const_cast<char**>(kw_list),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    PyTpgFaultMgr::_typeobject(), &fault_mgr_obj,
				    &dtpg_type_str,
				    &just_type_str,
				    &sat_type_str) ) {
    return nullptr;
  }
  auto& network = PyTpgNetwork::Get(network_obj);
  auto& fault_mgr = PyTpgFaultMgr::Get(fault_mgr_obj);
  string just_type;
  if ( just_type_str != nullptr ) {
    just_type = just_type_str;
  }
  SatSolverType solver_type;
  if ( sat_type_str != nullptr ) {
    solver_type = SatSolverType{sat_type_str};
  }
  auto self = type->tp_alloc(type, 0);
  auto tpgmgr_obj = reinterpret_cast<DtpgMgrObject*>(self);
  tpgmgr_obj->mVal = new DtpgMgr{network, fault_mgr, dtpg_type_str, just_type, solver_type};
  return self;
}

// 終了関数
void
DtpgMgr_dealloc(
  PyObject* self
)
{
  auto tpgmgr_obj = reinterpret_cast<DtpgMgrObject*>(self);
  delete tpgmgr_obj->mVal;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
DtpgMgr_run(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  mgr.run();
  Py_RETURN_NONE;
}

PyObject*
DtpgMgr_add_dop(
  PyObject* self,
  PyObject* args
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  Py_RETURN_NONE;
}

PyObject*
DtpgMgr_add_base_dop(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  mgr.add_base_dop();
  Py_RETURN_NONE;
}

PyObject*
DtpgMgr_add_drop_dop(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  mgr.add_drop_dop();
  Py_RETURN_NONE;
}

PyObject*
DtpgMgr_add_tvlist_dop(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  mgr.add_tvlist_dop();
  Py_RETURN_NONE;
}

PyObject*
DtpgMgr_add_verify_dop(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  mgr.add_verify_dop();
  Py_RETURN_NONE;
}

PyObject*
DtpgMgr_add_base_uop(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  mgr.add_base_uop();
  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef DtpgMgr_methods[] = {
  {"run", DtpgMgr_run, METH_NOARGS,
   PyDoc_STR("run")},
  {"add_dop", DtpgMgr_add_dop, METH_VARARGS,
   PyDoc_STR("add DetectOp")},
  {"add_base_dop", DtpgMgr_add_base_dop, METH_NOARGS,
   PyDoc_STR("add BaseDetectOp")},
  {"add_drop_dop", DtpgMgr_add_drop_dop, METH_NOARGS,
   PyDoc_STR("add DropDetectOp")},
  {"add_tvlist_dop", DtpgMgr_add_tvlist_dop, METH_NOARGS,
   PyDoc_STR("add TvListDetectOp")},
  {"add_verify_dop", DtpgMgr_add_verify_dop, METH_NOARGS,
   PyDoc_STR("add VerifyDetectOp")},
  {"add_base_uop", DtpgMgr_add_base_uop, METH_NOARGS,
   PyDoc_STR("add BaseUntestOp")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
DtpgMgr_detect_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  auto val = mgr.detect_count();
  return PyLong_FromLong(val);
}

PyObject*
DtpgMgr_untest_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  auto val = mgr.untest_count();
  return PyLong_FromLong(val);
}

PyObject*
DtpgMgr_abort_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  auto val = mgr.abort_count();
  return PyLong_FromLong(val);
}

PyObject*
DtpgMgr_tv_list(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  auto& tv_list = mgr.tv_list();
  SizeType n = tv_list.size();
  auto ans_obj = PyList_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto& tv = tv_list[i];
    auto obj1 = PyTestVector::ToPyObject(tv);
    PyList_SetItem(ans_obj, i, obj1);
  }
  return ans_obj;
}

PyObject*
DtpgMgr_stats(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  auto& stats = mgr.dtpg_stats();
  // 未完
  Py_RETURN_NONE;
}

// get/set 関数定義
PyGetSetDef DtpgMgr_getset[] = {
  {"detect_count", DtpgMgr_detect_count, nullptr,
   PyDoc_STR("# of detected faults")},
  {"untest_count", DtpgMgr_untest_count, nullptr,
   PyDoc_STR("# of untestable faults")},
  {"abort_count", DtpgMgr_abort_count, nullptr,
   PyDoc_STR("# of aborted faults")},
  {"tv_list", DtpgMgr_tv_list, nullptr,
   PyDoc_STR("list of TestVector")},
  {"dtpg_stats", DtpgMgr_stats, nullptr,
   PyDoc_STR("statistics of DTPG operation")},
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief 'DtpgMgr' オブジェクトを使用可能にする．
bool
PyDtpgMgr::init(
  PyObject* m
)
{
  DtpgMgrType.tp_name = "DtpgMgr";
  DtpgMgrType.tp_basicsize = sizeof(DtpgMgrObject);
  DtpgMgrType.tp_itemsize = 0;
  DtpgMgrType.tp_dealloc = DtpgMgr_dealloc;
  DtpgMgrType.tp_flags = Py_TPFLAGS_DEFAULT;
  DtpgMgrType.tp_doc = PyDoc_STR("DtpgMgr object");
  DtpgMgrType.tp_methods = DtpgMgr_methods;
  DtpgMgrType.tp_new = DtpgMgr_new;
  DtpgMgrType.tp_getset = DtpgMgr_getset;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "DtpgMgr", &DtpgMgrType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が DtpgMgr タイプか調べる．
bool
PyDtpgMgr::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DtpgMgr を表す PyObject から DtpgMgr を取り出す．
DtpgMgr&
PyDtpgMgr::Get(
  PyObject* obj
)
{
  auto tpgmgr_obj = reinterpret_cast<DtpgMgrObject*>(obj);
  return *tpgmgr_obj->mVal;
}

// @brief DtpgMgr を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgMgr::_typeobject()
{
  return &DtpgMgrType;
}

END_NAMESPACE_DRUID
