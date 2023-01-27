
/// @file PyDtpgFFR.cc
/// @brief Python DtpgFFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "PyDtpgFFR.h"
#include "ym/PyModule.h"
#include "PyFaultStatus.h"
#include "PyFaultType.h"
#include "PyTestVector.h"
#include "PyTpgFault.h"
#include "PyTpgNetwork.h"
#include "PyTpgFFR.h"
#include "TpgFFR.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DtpgFFRObject
{
  PyObject_HEAD
  DtpgFFR* mPtr;
};

// Python 用のタイプ定義
PyTypeObject DtpgFFRType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DtpgFFR_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "network",
    "fault_type",
    "ffr",
    "just_type",
    "solver_type",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_type_obj = nullptr;
  PyObject* ffr_obj = nullptr;
  const char* just_type_str = nullptr;
  const char* solver_type_str = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!O!|ss",
				    const_cast<char**>(kwlist),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    PyFaultType::_typeobject(), &fault_type_obj,
				    PyTpgFFR::_typeobject(), &ffr_obj,
				    &just_type_str,
				    &solver_type_str) ) {
    return nullptr;
  }
  string just_type;
  if ( just_type_str != nullptr ) {
    just_type = just_type_str;
  }
  SatSolverType solver_type;
  if ( solver_type_str != nullptr ) {
    solver_type = SatSolverType{solver_type_str};
  }
  auto obj = type->tp_alloc(type, 0);
  auto dtpg_obj = reinterpret_cast<DtpgFFRObject*>(obj);
  auto& network = PyTpgNetwork::_get(network_obj);
  auto fault_type = PyFaultType::_get(fault_type_obj);
  auto ffr = PyTpgFFR::_get(ffr_obj);
  dtpg_obj->mPtr = new DtpgFFR{network, fault_type, ffr,
			       just_type, solver_type};
  return obj;
}

// 終了関数
void
DtpgFFR_dealloc(
  PyObject* self
)
{
  auto dtpg_obj = reinterpret_cast<DtpgFFRObject*>(self);
  delete dtpg_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
DtpgFFR_gen_pattern(
  PyObject* self,
  PyObject* args
)
{
  PyObject* fault_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!", PyTpgFault::_typeobject(), &fault_obj) ) {
    return nullptr;
  }
  auto fault = PyTpgFault::_get(fault_obj);
  auto dtpg_obj = reinterpret_cast<DtpgFFRObject*>(self);
  auto result = dtpg_obj->mPtr->gen_pattern(fault);
  auto status_obj = PyFaultStatus::ToPyObject(result.status());
  auto tv_obj = PyTestVector::ToPyObject(result.testvector());
  return Py_BuildValue("OO", status_obj, tv_obj);
}

// メソッド定義
PyMethodDef DtpgFFR_methods[] = {
  {"gen_pattern", DtpgFFR_gen_pattern, METH_VARARGS,
   PyDoc_STR("generate test pattern for the specified fault")},
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief 'DtpgFFR' オブジェクトを使用可能にする．
bool
PyDtpgFFR::init(
  PyObject* m
)
{
  DtpgFFRType.tp_name = "DtpgFFR";
  DtpgFFRType.tp_basicsize = sizeof(DtpgFFRObject);
  DtpgFFRType.tp_itemsize = 0;
  DtpgFFRType.tp_dealloc = DtpgFFR_dealloc;
  DtpgFFRType.tp_flags = Py_TPFLAGS_DEFAULT;
  DtpgFFRType.tp_doc = PyDoc_STR("DtpgFFR object");
  DtpgFFRType.tp_methods = DtpgFFR_methods;
  DtpgFFRType.tp_new = DtpgFFR_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "DtpgFFR", &DtpgFFRType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が DtpgFFR タイプか調べる．
bool
PyDtpgFFR::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DtpgFFR を表す PyObject から DtpgFFR を取り出す．
DtpgFFR*
PyDtpgFFR::_get(
  PyObject* obj
)
{
  auto dtpg_obj = reinterpret_cast<DtpgFFRObject*>(obj);
  return dtpg_obj->mPtr;
}

// @brief DtpgFFR を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgFFR::_typeobject()
{
  return &DtpgFFRType;
}

END_NAMESPACE_DRUID
