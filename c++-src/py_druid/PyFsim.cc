
/// @file PyFsim.cc
/// @brief PyFsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "Fsim.h"
#include "TestVector.h"
#include "InputVector.h"
#include "DffVector.h"


BEGIN_NAMESPACE_DRUID

extern bool FaultType_from_PyObj(PyObject*, FaultType&);
extern bool TpgNetwork_from_PyObj(PyObject*, TpgNetwork*&);
extern bool TpgFault_from_PyObj(PyObject*, const TpgFault*&);
extern bool TestVector_from_PyObj(PyObject*, TestVector&);
extern bool InputVector_from_PyObj(PyObject*, InputVector&);
extern bool DffVector_from_PyObj(PyObject*, DffVector&);
extern PyObject* PyObj_from_TpgFault(const TpgFault*);
extern PyObject* PyObj_from_TestVector(const TestVector&);
extern PyObject* PyObj_from_InputVector(const InputVector&);
extern PyObject* PyObj_from_DffVector(const DffVector&);

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct FsimObject
{
  PyObject_HEAD
  Fsim* mFsim;
};

// 生成関数
PyObject*
Fsim_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  auto self = reinterpret_cast<FsimObject*>(type->tp_alloc(type, 0));
  self->mFsim = nullptr;
  return reinterpret_cast<PyObject*>(self);
}

// 終了関数
void
Fsim_dealloc(
  FsimObject* self
)
{
  delete self->mFsim;
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

// 初期化関数(__init__()相当)
int
Fsim_init(
  FsimObject* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  PyObject* network_obj = nullptr;
  PyObject* fault_type_obj = nullptr;
  int val_type = 0;
  if ( !PyArg_ParseTuple(args, "OOi",
			 &network_obj,
			 &fault_type_obj,
			 &val_type) ) {
    return -1;
  }

  TpgNetwork* network;
  if ( !TpgNetwork_from_PyObj(network_obj, network) ) {
    return -1;
  }

  FaultType fault_type;
  if ( !FaultType_from_PyObj(fault_type_obj, fault_type) ) {
    return -1;
  }

  bool has_x = false;
  if ( val_type == 2 ) {
    ;
  }
  else if ( val_type == 3 ) {
    has_x = true;
  }
  else {
    PyErr_SetString(PyExc_ValueError, "3rd argument should be 2 or 3");
    return -1;
  }

  self->mFsim = new Fsim{*network, fault_type, has_x};

  return 0;
}

PyObject*
Fsim_set_skip_all(
  FsimObject* self
)
{
  self->mFsim->set_skip_all();
  Py_RETURN_NONE;
}

PyObject*
Fsim_set_skip(
  FsimObject* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &obj) ) {
    return nullptr;
  }
  if ( PySequence_Check(obj) ) {
    SizeType n = PySequence_Size(obj);
    vector<const TpgFault*> fault_list;
    fault_list.reserve(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      auto obj1 = PySequence_GetItem(obj, i);
      const TpgFault* fault = nullptr;
      if ( !TpgFault_from_PyObj(obj1, fault) ) {
	return nullptr;
      }
      fault_list.push_back(fault);
    }
    self->mFsim->set_skip(fault_list);
  }
  else {
    const TpgFault* fault = nullptr;
    if ( !TpgFault_from_PyObj(obj, fault) ) {
      return nullptr;
    }
    self->mFsim->set_skip(fault);
  }
  Py_RETURN_NONE;
}

PyObject*
Fsim_clear_skip_all(
  FsimObject* self
)
{
  self->mFsim->clear_skip_all();
  Py_RETURN_NONE;
}

PyObject*
Fsim_clear_skip(
  FsimObject* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &obj) ) {
    return nullptr;
  }
  if ( PySequence_Check(obj) ) {
    SizeType n = PySequence_Size(obj);
    vector<const TpgFault*> fault_list;
    fault_list.reserve(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      auto obj1 = PySequence_GetItem(obj, i);
      const TpgFault* fault = nullptr;
      if ( !TpgFault_from_PyObj(obj1, fault) ) {
	return nullptr;
      }
      fault_list.push_back(fault);
    }
    self->mFsim->clear_skip(fault_list);
  }
  else {
    const TpgFault* fault = nullptr;
    if ( !TpgFault_from_PyObj(obj, fault) ) {
      return nullptr;
    }
    self->mFsim->clear_skip(fault);
  }
  Py_RETURN_NONE;
}

PyObject*
Fsim_spsfp(
  FsimObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  PyObject* obj2 = nullptr;
  if ( !PyArg_ParseTuple(args, "OO", &obj1, &obj2) ) {
    return nullptr;
  }
  TestVector tv;
  if ( !TestVector_from_PyObj(obj1, tv) ) {
    return nullptr;
  }
  const TpgFault* fault = nullptr;
  if ( !TpgFault_from_PyObj(obj2, fault) ) {
    return nullptr;
  }
  bool ans = self->mFsim->spsfp(tv, fault);
  return PyBool_FromLong(ans);
}

PyObject*
Fsim_sppfp(
  FsimObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &obj1) ) {
    return nullptr;
  }
  TestVector tv;
  if ( !TestVector_from_PyObj(obj1, tv) ) {
    return nullptr;
  }
  SizeType n = self->mFsim->sppfp(tv);
  return PyLong_FromLong(n);
}

PyObject*
Fsim_ppsfp(
  FsimObject* self,
  PyObject* Py_UNUSED(args)
)
{
  SizeType n = self->mFsim->ppsfp();
  return PyLong_FromLong(n);
}

PyObject*
Fsim_calc_wsa(
  FsimObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  bool weighted = false;
  if ( !PyArg_ParseTuple(args, "Oi", &obj1, &weighted) ) {
    return nullptr;
  }
  InputVector iv;
  if ( !InputVector_from_PyObj(obj1, iv) ) {
    return nullptr;
  }
  SizeType n = self->mFsim->calc_wsa(iv, weighted);
  return PyLong_FromLong(n);
}

PyObject*
Fsim_set_state(
  FsimObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  PyObject* obj2 = nullptr;
  if ( !PyArg_ParseTuple(args, "OO", &obj1, &obj2) ) {
    return nullptr;
  }
  InputVector iv;
  if ( !InputVector_from_PyObj(obj1, iv) ) {
    return nullptr;
  }
  DffVector dv;
  if ( !DffVector_from_PyObj(obj2, dv) ) {
    return nullptr;
  }
  self->mFsim->set_state(iv, dv);
  Py_RETURN_NONE;
}

PyObject*
Fsim_get_state(
  FsimObject* self,
  PyObject* Py_UNUSED(args)
)
{
  InputVector iv;
  DffVector dv;
  self->mFsim->get_state(iv, dv);
  auto obj1 = PyObj_from_InputVector(iv);
  auto obj2 = PyObj_from_DffVector(dv);
  return Py_BuildValue("OO", obj1, obj2);
}

PyObject*
Fsim_clear_patterns(
  FsimObject* self,
  PyObject* Py_UNUSED(args)
)
{
  self->mFsim->clear_patterns();
  Py_RETURN_NONE;
}

PyObject*
Fsim_set_pattern(
  FsimObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  PyObject* obj1 = nullptr;
  if ( !PyArg_ParseTuple(args, "iO", &pos, &obj1) ) {
    return nullptr;
  }
  TestVector tv;
  if ( !TestVector_from_PyObj(obj1, tv) ) {
    return nullptr;
  }
  self->mFsim->set_pattern(pos, tv);
  Py_RETURN_NONE;
}

PyObject*
Fsim_get_pattern(
  FsimObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto tv = self->mFsim->get_pattern(pos);
  return PyObj_from_TestVector(tv);
}

PyObject*
Fsim_det_fault_num(
  FsimObject* self,
  PyObject* Py_UNUSED(args)
)
{
  SizeType n = self->mFsim->det_fault_num();
  return PyLong_FromLong(n);
}

PyObject*
Fsim_det_fault(
  FsimObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto f = self->mFsim->det_fault(pos);
  return PyObj_from_TpgFault(f);
}

PyObject*
Fsim_det_fault_list(
  FsimObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fault_list = self->mFsim->det_fault_list();
  SizeType n = fault_list.num();
  auto ans_obj = PyList_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto f = fault_list[i];
    auto obj1 = PyObj_from_TpgFault(f);
    PyList_SET_ITEM(ans_obj, i, obj1);
  }
  return ans_obj;
}

PyObject*
Fsim_det_fault_pat(
  FsimObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto pat = self->mFsim->det_fault_pat(pos);
  return PyLong_FromLong(pat);
}

PyObject*
Fsim_det_fault_pat_list(
  FsimObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto pat_list = self->mFsim->det_fault_pat_list();
  SizeType n = pat_list.num();
  auto ans_obj = PyList_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto pat = pat_list[i];
    auto obj1 = PyLong_FromLong(pat);
    PyList_SET_ITEM(ans_obj, i, obj1);
  }
  return ans_obj;
}

// メソッド定義
PyMethodDef Fsim_methods[] = {
  {"set_skip_all", reinterpret_cast<PyCFunction>(Fsim_set_skip_all),
   METH_NOARGS, PyDoc_STR("set skip mark for all fatuls")},
  {"set_skip", reinterpret_cast<PyCFunction>(Fsim_set_skip),
   METH_VARARGS, PyDoc_STR("set skip mark for the designatated faults")},
  {"clear_skip_all", reinterpret_cast<PyCFunction>(Fsim_set_skip_all),
   METH_NOARGS, PyDoc_STR("clear skip mark for all fatuls")},
  {"clear_skip", reinterpret_cast<PyCFunction>(Fsim_set_skip),
   METH_VARARGS, PyDoc_STR("clear skip mark for the designatated faults")},
  {"spsfp", reinterpret_cast<PyCFunction>(Fsim_spsfp),
   METH_VARARGS, PyDoc_STR("do Single Pattern Single Fault Propagation simulation")},
  {"sppfp", reinterpret_cast<PyCFunction>(Fsim_sppfp),
   METH_VARARGS, PyDoc_STR("do Single Pattern Parallel Fault Propagation simulation")},
  {"ppsfp", reinterpret_cast<PyCFunction>(Fsim_ppsfp),
   METH_NOARGS, PyDoc_STR("do Parallel Pattern Single Fault Propagation simulation")},
  {"calc_wsa", reinterpret_cast<PyCFunction>(Fsim_calc_wsa),
   METH_NOARGS, PyDoc_STR("calculate weighted sum of activities")},
  {"set_state", reinterpret_cast<PyCFunction>(Fsim_set_state),
   METH_VARARGS, PyDoc_STR("set the internal state")},
  {"get_state", reinterpret_cast<PyCFunction>(Fsim_get_state),
   METH_NOARGS, PyDoc_STR("get the internal state")},
  {"clear_patterns", reinterpret_cast<PyCFunction>(Fsim_clear_patterns),
   METH_NOARGS, PyDoc_STR("clear patterns for ppsfp")},
  {"set_pattern", reinterpret_cast<PyCFunction>(Fsim_set_pattern),
   METH_VARARGS, PyDoc_STR("set pattern for ppsfp")},
  {"get_pattern", reinterpret_cast<PyCFunction>(Fsim_get_pattern),
   METH_VARARGS, PyDoc_STR("get pattern for ppsfp")},
  {"det_fault_num", reinterpret_cast<PyCFunction>(Fsim_det_fault_num),
   METH_NOARGS, PyDoc_STR("return the number of detected faults")},
  {"det_fault", reinterpret_cast<PyCFunction>(Fsim_det_fault),
   METH_VARARGS, PyDoc_STR("return the detected fault")},
  {"det_fault_list", reinterpret_cast<PyCFunction>(Fsim_det_fault_list),
   METH_NOARGS, PyDoc_STR("return the list of all the detected faults")},
  {"det_fault_pat", reinterpret_cast<PyCFunction>(Fsim_det_fault_pat),
   METH_VARARGS, PyDoc_STR("return the detecting pattern bits")},
  {"det_fault_pat_list", reinterpret_cast<PyCFunction>(Fsim_det_fault_pat_list),
   METH_NOARGS, PyDoc_STR("return the list of the detecting pattern bits")},
  {nullptr, nullptr, 0, nullptr}
};

// Python 用のタイプ定義
PyTypeObject FsimType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りのメンバは Fsim_init
};

END_NONAMESPACE


// @brief 'Fsim' オブジェクトを使用可能にする．
bool
PyInit_Fsim(
  PyObject* m
)
{
  FsimType.tp_name = "druid.Fsim";
  FsimType.tp_basicsize = sizeof(FsimObject);
  FsimType.tp_itemsize = 0;
  FsimType.tp_dealloc = reinterpret_cast<destructor>(Fsim_dealloc);
  FsimType.tp_flags = Py_TPFLAGS_DEFAULT;
  FsimType.tp_doc = PyDoc_STR("Fsim objects");
  FsimType.tp_methods = Fsim_methods;
  FsimType.tp_init = reinterpret_cast<initproc>(Fsim_init);
  FsimType.tp_new = Fsim_new;
  if ( PyType_Ready(&FsimType) < 0 ) {
    return false;
  }
  Py_INCREF(&FsimType);
  if ( PyModule_AddObject(m, "Fsim",
			  reinterpret_cast<PyObject*>(&FsimType)) < 0 ) {
    Py_DECREF(&FsimType);
    return false;
  }
  return true;
}

END_NAMESPACE_DRUID
