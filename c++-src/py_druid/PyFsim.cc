
/// @file PyFsim.cc
/// @brief Python Fsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PyFsim.h"
#include "PyTpgNetwork.h"
#include "PyTestVector.h"
#include "PyInputVector.h"
#include "PyDffVector.h"
#include "PyFaultType.h"
#include "PyTpgFault.h"
#include "ym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct FsimObject
{
  PyObject_HEAD
  Fsim* mPtr;
};

// Python 用のタイプ定義
PyTypeObject FsimType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
Fsim_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  auto self = type->tp_alloc(type, 0);
  auto fsim_obj = reinterpret_cast<FsimObject*>(self);
  fsim_obj->mPtr = nullptr;
  return self;
}

// 終了関数
void
Fsim_dealloc(
  PyObject* self
)
{
  auto fsim_obj = reinterpret_cast<FsimObject*>(self);
  delete fsim_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

// 初期化関数(__init__()相当)
int
Fsim_init(
  PyObject* self,
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

  const TpgNetwork* network;
  if ( !PyTpgNetwork::FromPyObject(network_obj, network) ) {
    return -1;
  }

  FaultType fault_type;
  if ( !PyFaultType::FromPyObject(fault_type_obj, fault_type) ) {
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

  auto fsim_obj = reinterpret_cast<FsimObject*>(self);
  fsim_obj->mPtr = new Fsim{*network, fault_type, has_x};

  return 0;
}

PyObject*
Fsim_set_skip_all(
  FsimObject* self
)
{
  self->mPtr->set_skip_all();
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
      if ( !PyTpgFault::FromPyObject(obj1, fault) ) {
	return nullptr;
      }
      fault_list.push_back(fault);
    }
    self->mPtr->set_skip(fault_list);
  }
  else {
    const TpgFault* fault = nullptr;
    if ( !PyTpgFault::FromPyObject(obj, fault) ) {
      return nullptr;
    }
    self->mPtr->set_skip(fault);
  }
  Py_RETURN_NONE;
}

PyObject*
Fsim_clear_skip_all(
  FsimObject* self
)
{
  self->mPtr->clear_skip_all();
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
      if ( !PyTpgFault::FromPyObject(obj1, fault) ) {
	return nullptr;
      }
      fault_list.push_back(fault);
    }
    self->mPtr->clear_skip(fault_list);
  }
  else {
    const TpgFault* fault = nullptr;
    if ( !PyTpgFault::FromPyObject(obj, fault) ) {
      return nullptr;
    }
    self->mPtr->clear_skip(fault);
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
  if ( !PyTestVector::FromPyObject(obj1, tv) ) {
    return nullptr;
  }
  const TpgFault* fault = nullptr;
  if ( !PyTpgFault::FromPyObject(obj2, fault) ) {
    return nullptr;
  }
  bool ans = self->mPtr->spsfp(tv, fault);
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
  if ( !PyTestVector::FromPyObject(obj1, tv) ) {
    return nullptr;
  }
  SizeType n = self->mPtr->sppfp(tv);
  return PyLong_FromLong(n);
}

PyObject*
Fsim_ppsfp(
  FsimObject* self,
  PyObject* Py_UNUSED(args)
)
{
  SizeType n = self->mPtr->ppsfp();
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
  if ( !PyInputVector::FromPyObject(obj1, iv) ) {
    return nullptr;
  }
  SizeType n = self->mPtr->calc_wsa(iv, weighted);
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
  if ( !PyInputVector::FromPyObject(obj1, iv) ) {
    return nullptr;
  }
  DffVector dv;
  if ( !PyDffVector::FromPyObject(obj2, dv) ) {
    return nullptr;
  }
  self->mPtr->set_state(iv, dv);
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
  self->mPtr->get_state(iv, dv);
  auto obj1 = PyInputVector::ToPyObject(iv);
  auto obj2 = PyDffVector::ToPyObject(dv);
  return Py_BuildValue("OO", obj1, obj2);
}

PyObject*
Fsim_clear_patterns(
  FsimObject* self,
  PyObject* Py_UNUSED(args)
)
{
  self->mPtr->clear_patterns();
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
  if ( !PyTestVector::FromPyObject(obj1, tv) ) {
    return nullptr;
  }
  self->mPtr->set_pattern(pos, tv);
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
  auto tv = self->mPtr->get_pattern(pos);
  return PyTestVector::ToPyObject(tv);
}

PyObject*
Fsim_det_fault_num(
  FsimObject* self,
  PyObject* Py_UNUSED(args)
)
{
  SizeType n = self->mPtr->det_fault_num();
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
  auto f = self->mPtr->det_fault(pos);
  return PyTpgFault::ToPyObject(f);
}

PyObject*
Fsim_det_fault_list(
  FsimObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fault_list = self->mPtr->det_fault_list();
  SizeType n = fault_list.num();
  auto ans_obj = PyList_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto f = fault_list[i];
    auto obj1 = PyTpgFault::ToPyObject(f);
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
  auto pat = self->mPtr->det_fault_pat(pos);
  return PyLong_FromLong(pat);
}

PyObject*
Fsim_det_fault_pat_list(
  FsimObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto pat_list = self->mPtr->det_fault_pat_list();
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

END_NONAMESPACE


// @brief 'Fsim' オブジェクトを使用可能にする．
bool
PyFsim::init(
  PyObject* m
)
{
  FsimType.tp_name = "Fsim";
  FsimType.tp_basicsize = sizeof(FsimObject);
  FsimType.tp_itemsize = 0;
  FsimType.tp_dealloc = Fsim_dealloc;
  FsimType.tp_flags = Py_TPFLAGS_DEFAULT;
  FsimType.tp_doc = PyDoc_STR("Fsim object");
  FsimType.tp_methods = Fsim_methods;
  FsimType.tp_init = Fsim_init;
  FsimType.tp_new = Fsim_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "Fsim", &FsimType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject から Fsim を取り出す．
bool
PyFsim::FromPyObject(
  PyObject* obj,
  Fsim*& val
)
{
  if ( !_check(obj) ) {
    PyErr_SetString(PyExc_TypeError, "object is not a Fsim type");
    return false;
  }
  val = _get(obj);
  return true;
}

// @brief PyObject が Fsim タイプか調べる．
bool
PyFsim::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief Fsim を表す PyObject から Fsim を取り出す．
Fsim*
PyFsim::_get(
  PyObject* obj
)
{
  auto fsim_obj = reinterpret_cast<FsimObject*>(obj);
  return fsim_obj->mPtr;
}

// @brief Fsim を表すオブジェクトの型定義を返す．
PyTypeObject*
PyFsim::_typeobject()
{
  return &FsimType;
}

END_NAMESPACE_DRUID
