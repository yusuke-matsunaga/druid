
/// @file PyTpgMgr.cc
/// @brief Python TpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "PyTpgMgr.h"
#include "pym/PyModule.h"
#include "PyTpgNetwork.h"
#include "PyFaultType.h"
#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TpgMgrObject
{
  PyObject_HEAD
  TpgMgr* mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgMgrType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
TpgMgr_new(
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
  auto tpgmgr_obj = reinterpret_cast<TpgMgrObject*>(self);
  tpgmgr_obj->mVal = new TpgMgr{network, fault_type, dtpg_type_str, just_type, solver_type};
  return self;
}

// 終了関数
void
TpgMgr_dealloc(
  PyObject* self
)
{
  auto tpgmgr_obj = reinterpret_cast<TpgMgrObject*>(self);
  delete tpgmgr_obj->mVal;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
TpgMgr_run(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& tpgmgr = PyTpgMgr::Get(self);
  tpgmgr.run();
  Py_RETURN_NONE;
}

PyObject*
TpgMgr_add_dop(
  PyObject* self,
  PyObject* args
)
{
  auto& tpgmgr = PyTpgMgr::Get(self);
  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef TpgMgr_methods[] = {
  {"run", TpgMgr_run, METH_NOARGS,
   PyDoc_STR("run")},
  {"add_dop", TpgMgr_add_dop, METH_VARARGS,
   PyDoc_STR("add DetectOp")},
  {nullptr, nullptr, 0, nullptr}
};

#if 0
// get() 関数の例
PyObject*
TpgMgr_get(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto val = PyTpgMgr::_get(self);
}

// get/set 関数定義
PyGetSetDef TpgMgr_getset[] = {
  {"member", TpgMgr_get, nullptr, PyDoc_STR("member getter"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr},
};
#endif

END_NONAMESPACE


// @brief 'TpgMgr' オブジェクトを使用可能にする．
bool
PyTpgMgr::init(
  PyObject* m
)
{
  TpgMgrType.tp_name = "TpgMgr";
  TpgMgrType.tp_basicsize = sizeof(TpgMgrObject);
  TpgMgrType.tp_itemsize = 0;
  TpgMgrType.tp_dealloc = TpgMgr_dealloc;
  TpgMgrType.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgMgrType.tp_doc = PyDoc_STR("TpgMgr object");
  TpgMgrType.tp_methods = TpgMgr_methods;
  //TpgMgrType.tp_getset = TpgMgr_getset;
  TpgMgrType.tp_new = TpgMgr_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "TpgMgr", &TpgMgrType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が TpgMgr タイプか調べる．
bool
PyTpgMgr::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TpgMgr を表す PyObject から TpgMgr を取り出す．
TpgMgr&
PyTpgMgr::Get(
  PyObject* obj
)
{
  auto tpgmgr_obj = reinterpret_cast<TpgMgrObject*>(obj);
  return *tpgmgr_obj->mVal;
}

// @brief TpgMgr を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgMgr::_typeobject()
{
  return &TpgMgrType;
}

END_NAMESPACE_DRUID
