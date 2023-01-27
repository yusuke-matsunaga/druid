
/// @file PyDtpgMFFC.cc
/// @brief Python DtpgMFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "PyDtpgMFFC.h"
#include "ym/PyModule.h"
#include "PyFaultStatus.h"
#include "PyFaultType.h"
#include "PyTestVector.h"
#include "PyTpgFault.h"
#include "PyTpgNetwork.h"
#include "TpgMFFC.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DtpgMFFCObject
{
  PyObject_HEAD
  DtpgMFFC* mPtr;
};

// Python 用のタイプ定義
PyTypeObject DtpgMFFCType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DtpgMFFC_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "network",
    "fault_type",
    "mffc",
    "just_type",
    "solver_type",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_type_obj = nullptr;
  int mffc_id = -1;
  const char* just_type_str = nullptr;
  const char* solver_type_str = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!i|ss",
				    const_cast<char**>(kwlist),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    PyFaultType::_typeobject(), &fault_type_obj,
				    &mffc_id,
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
  auto dtpg_obj = reinterpret_cast<DtpgMFFCObject*>(obj);
  auto network_p = PyTpgNetwork::_get(network_obj);
  auto fault_type = PyFaultType::_get(fault_type_obj);
  auto mffc = network_p->mffc(mffc_id);
  dtpg_obj->mPtr = new DtpgMFFC{*network_p, fault_type, mffc,
				just_type, solver_type};
  return obj;
}

// 終了関数
void
DtpgMFFC_dealloc(
  PyObject* self
)
{
  auto dtpg_obj = reinterpret_cast<DtpgMFFCObject*>(self);
  delete dtpg_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
DtpgMFFC_gen_pattern(
  PyObject* self,
  PyObject* args
)
{
  PyObject* fault_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!", PyTpgFault::_typeobject(), &fault_obj) ) {
    return nullptr;
  }
  auto fault = PyTpgFault::_get(fault_obj);
  auto dtpg_obj = reinterpret_cast<DtpgMFFCObject*>(self);
  auto result = dtpg_obj->mPtr->gen_pattern(fault);
  auto status_obj = PyFaultStatus::ToPyObject(result.status());
  auto tv_obj = PyTestVector::ToPyObject(result.testvector());
  return Py_BuildValue("OO", status_obj, tv_obj);
}

// メソッド定義
PyMethodDef DtpgMFFC_methods[] = {
  {"gen_pattern", DtpgMFFC_gen_pattern, METH_VARARGS,
   PyDoc_STR("generate test pattern for the specified fault")},
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief 'DtpgMFFC' オブジェクトを使用可能にする．
bool
PyDtpgMFFC::init(
  PyObject* m
)
{
  DtpgMFFCType.tp_name = "DtpgMFFC";
  DtpgMFFCType.tp_basicsize = sizeof(DtpgMFFCObject);
  DtpgMFFCType.tp_itemsize = 0;
  DtpgMFFCType.tp_dealloc = DtpgMFFC_dealloc;
  DtpgMFFCType.tp_flags = Py_TPFLAGS_DEFAULT;
  DtpgMFFCType.tp_doc = PyDoc_STR("DtpgMFFC object");
  DtpgMFFCType.tp_methods = DtpgMFFC_methods;
  DtpgMFFCType.tp_new = DtpgMFFC_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "DtpgMFFC", &DtpgMFFCType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が DtpgMFFC タイプか調べる．
bool
PyDtpgMFFC::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DtpgMFFC を表す PyObject から DtpgMFFC を取り出す．
DtpgMFFC*
PyDtpgMFFC::_get(
  PyObject* obj
)
{
  auto dtpg_obj = reinterpret_cast<DtpgMFFCObject*>(obj);
  return dtpg_obj->mPtr;
}

// @brief DtpgMFFC を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgMFFC::_typeobject()
{
  return &DtpgMFFCType;
}

END_NAMESPACE_DRUID
