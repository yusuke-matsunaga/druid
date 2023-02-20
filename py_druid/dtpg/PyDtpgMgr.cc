
/// @file PyDtpgMgr.cc
/// @brief Python DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "PyDtpgMgr.h"
#include "pym/PyModule.h"
#include "PyTpgNetwork.h"
#include "PyFaultType.h"
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
    "fault_type",
    "dtpg_type",
    "just_type",
    "sat_type",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_type_obj = nullptr;
  const char* dtpg_type_str = nullptr;
  const char* just_type_str = nullptr;
  const char* sat_type_str = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!s|ss",
				    const_cast<char**>(kw_list),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    PyFaultType::_typeobject(), &fault_type_obj,
				    &dtpg_type_str,
				    &just_type_str,
				    &sat_type_str) ) {
    return nullptr;
  }
  auto& network = PyTpgNetwork::Get(network_obj);
  auto fault_type = PyFaultType::Get(fault_type_obj);
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
  tpgmgr_obj->mVal = new DtpgMgr{network, fault_type, dtpg_type_str, just_type, solver_type};
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
  auto& tpgmgr = PyDtpgMgr::Get(self);
  tpgmgr.run();
  Py_RETURN_NONE;
}

PyObject*
DtpgMgr_add_dop(
  PyObject* self,
  PyObject* args
)
{
  auto& tpgmgr = PyDtpgMgr::Get(self);
  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef DtpgMgr_methods[] = {
  {"run", DtpgMgr_run, METH_NOARGS,
   PyDoc_STR("run")},
  {"add_dop", DtpgMgr_add_dop, METH_VARARGS,
   PyDoc_STR("add DetectOp")},
  {nullptr, nullptr, 0, nullptr}
};

#if 0
// get() 関数の例
PyObject*
DtpgMgr_get(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto val = PyDtpgMgr::_get(self);
}

// get/set 関数定義
PyGetSetDef DtpgMgr_getset[] = {
  {"member", DtpgMgr_get, nullptr, PyDoc_STR("member getter"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr},
};
#endif

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
  //DtpgMgrType.tp_getset = DtpgMgr_getset;
  DtpgMgrType.tp_new = DtpgMgr_new;

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
