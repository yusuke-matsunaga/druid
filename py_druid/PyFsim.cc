
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
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "network",
    "fault_type",
    "val_type",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_type_obj = nullptr;
  int val_type = 0;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!i",
				    const_cast<char**>(kwlist),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    PyFaultType::_typeobject(), &fault_type_obj,
				    &val_type) ) {
    return nullptr;
  }

  auto network = PyTpgNetwork::_get(network_obj);
  auto fault_type = PyFaultType::_get(fault_type_obj);

  bool has_x = false;
  if ( val_type == 2 ) {
    ;
  }
  else if ( val_type == 3 ) {
    has_x = true;
  }
  else {
    PyErr_SetString(PyExc_ValueError, "argument 3 must be 2 or 3");
    return nullptr;
  }
  auto self = type->tp_alloc(type, 0);
  auto fsim_obj = reinterpret_cast<FsimObject*>(self);
  fsim_obj->mPtr = new Fsim{*network, fault_type, has_x};
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

PyObject*
Fsim_set_skip_all(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fsim = PyFsim::_get(self);
  fsim->set_skip_all();
  Py_RETURN_NONE;
}

PyObject*
Fsim_set_skip(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &obj) ) {
    return nullptr;
  }

  auto fsim = PyFsim::_get(self);
  if ( PySequence_Check(obj) ) {
    SizeType n = PySequence_Size(obj);
    vector<const TpgFault*> fault_list;
    fault_list.reserve(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      auto obj1 = PySequence_GetItem(obj, i);
      if ( !PyTpgFault::_check(obj1) ) {
	PyErr_SetString(PyExc_TypeError, "'TpgFault' type expected");
	return nullptr;
      }
      auto fault = PyTpgFault::_get(obj1);
      fault_list.push_back(fault);
    }
    fsim->set_skip(fault_list);
  }
  else {
    if ( !PyTpgFault::_check(obj) ) {
      PyErr_SetString(PyExc_TypeError, "'TpgFault' type expected");
      return nullptr;
    }
    auto fault = PyTpgFault::_get(obj);
    fsim->set_skip(fault);
  }
  Py_RETURN_NONE;
}

PyObject*
Fsim_clear_skip_all(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fsim = PyFsim::_get(self);
  fsim->clear_skip_all();
  Py_RETURN_NONE;
}

PyObject*
Fsim_clear_skip(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &obj) ) {
    return nullptr;
  }

  auto fsim = PyFsim::_get(self);
  if ( PySequence_Check(obj) ) {
    SizeType n = PySequence_Size(obj);
    vector<const TpgFault*> fault_list;
    fault_list.reserve(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      auto obj1 = PySequence_GetItem(obj, i);
      if ( !PyTpgFault::_check(obj1) ) {
	PyErr_SetString(PyExc_TypeError, "'TpgFault' type expected");
	return nullptr;
      }
      auto fault = PyTpgFault::_get(obj1);
      fault_list.push_back(fault);
    }
    fsim->clear_skip(fault_list);
  }
  else {
    if ( !PyTpgFault::_check(obj) ) {
      PyErr_SetString(PyExc_TypeError, "'TpgFault' type expected");
      return nullptr;
    }
    auto fault = PyTpgFault::_get(obj);
    fsim->clear_skip(fault);
  }
  Py_RETURN_NONE;
}

PyObject*
Fsim_spsfp(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  PyObject* obj2 = nullptr;
  if ( !PyArg_ParseTuple(args, "O!O!",
			 PyTestVector::_typeobject(), &obj1,
			 PyTpgFault::_typeobject(), &obj2) ) {
    return nullptr;
  }
  auto tv = PyTestVector::_get(obj1);
  auto fault = PyTpgFault::_get(obj2);
  auto fsim = PyFsim::_get(self);
  bool ans = fsim->spsfp(tv, fault);
  return PyBool_FromLong(ans);
}

PyObject*
Fsim_sppfp(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  if ( !PyArg_ParseTuple(args, "O!",
			 PyTestVector::_typeobject(), &obj1) ) {
    return nullptr;
  }
  auto tv = PyTestVector::_get(obj1);
  auto fsim = PyFsim::_get(self);
  SizeType n = fsim->sppfp(tv);
  return PyLong_FromLong(n);
}

PyObject*
Fsim_ppsfp(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fsim = PyFsim::_get(self);
  SizeType n = fsim->ppsfp();
  return PyLong_FromLong(n);
}

PyObject*
Fsim_calc_wsa(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  bool weighted = false;
  if ( !PyArg_ParseTuple(args, "O!i",
			 PyInputVector::_typeobject(), &obj1,
			 &weighted) ) {
    return nullptr;
  }
  auto iv = PyInputVector::_get(obj1);
  auto fsim = PyFsim::_get(self);
  SizeType n = fsim->calc_wsa(iv, weighted);
  return PyLong_FromLong(n);
}

PyObject*
Fsim_set_state(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  PyObject* obj2 = nullptr;
  if ( !PyArg_ParseTuple(args, "O!O!",
			 PyInputVector::_typeobject(), &obj1,
			 PyDffVector::_typeobject(), &obj2) ) {
    return nullptr;
  }
  auto iv = PyInputVector::_get(obj1);
  auto dv = PyDffVector::_get(obj2);
  auto fsim = PyFsim::_get(self);
  fsim->set_state(iv, dv);
  Py_RETURN_NONE;
}

PyObject*
Fsim_get_state(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  InputVector iv;
  DffVector dv;
  auto fsim = PyFsim::_get(self);
  fsim->get_state(iv, dv);
  auto obj1 = PyInputVector::ToPyObject(iv);
  auto obj2 = PyDffVector::ToPyObject(dv);
  return Py_BuildValue("OO", obj1, obj2);
}

PyObject*
Fsim_clear_patterns(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fsim = PyFsim::_get(self);
  fsim->clear_patterns();
  Py_RETURN_NONE;
}

PyObject*
Fsim_set_pattern(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  PyObject* obj1 = nullptr;
  if ( !PyArg_ParseTuple(args, "iO!", &pos,
			 PyTestVector::_typeobject(), &obj1) ) {
    return nullptr;
  }
  auto tv = PyTestVector::_get(obj1);
  auto fsim = PyFsim::_get(self);
  fsim->set_pattern(pos, tv);
  Py_RETURN_NONE;
}

PyObject*
Fsim_get_pattern(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto fsim = PyFsim::_get(self);
  auto tv = fsim->get_pattern(pos);
  return PyTestVector::ToPyObject(tv);
}

PyObject*
Fsim_det_fault_num(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fsim = PyFsim::_get(self);
  SizeType n = fsim->det_fault_num();
  return PyLong_FromLong(n);
}

PyObject*
Fsim_det_fault(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto fsim = PyFsim::_get(self);
  auto f = fsim->det_fault(pos);
  return PyTpgFault::ToPyObject(f);
}

PyObject*
Fsim_det_fault_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fsim = PyFsim::_get(self);
  auto fault_list = fsim->det_fault_list();
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
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto fsim = PyFsim::_get(self);
  auto pat = fsim->det_fault_pat(pos);
  return PyLong_FromLong(pat);
}

PyObject*
Fsim_det_fault_pat_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fsim = PyFsim::_get(self);
  auto pat_list = fsim->det_fault_pat_list();
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
  {"set_skip_all", Fsim_set_skip_all, METH_NOARGS,
   PyDoc_STR("set skip mark for all fatuls")},
  {"set_skip", Fsim_set_skip, METH_VARARGS,
   PyDoc_STR("set skip mark for the designatated faults")},
  {"clear_skip_all", Fsim_set_skip_all, METH_NOARGS,
   PyDoc_STR("clear skip mark for all fatuls")},
  {"clear_skip", Fsim_set_skip, METH_VARARGS,
   PyDoc_STR("clear skip mark for the designatated faults")},
  {"spsfp", Fsim_spsfp, METH_VARARGS,
   PyDoc_STR("do Single Pattern Single Fault Propagation simulation")},
  {"sppfp", Fsim_sppfp, METH_VARARGS,
   PyDoc_STR("do Single Pattern Parallel Fault Propagation simulation")},
  {"ppsfp", Fsim_ppsfp, METH_NOARGS,
   PyDoc_STR("do Parallel Pattern Single Fault Propagation simulation")},
  {"calc_wsa", Fsim_calc_wsa, METH_NOARGS,
   PyDoc_STR("calculate weighted sum of activities")},
  {"set_state", Fsim_set_state, METH_VARARGS,
   PyDoc_STR("set the internal state")},
  {"get_state", Fsim_get_state, METH_NOARGS,
   PyDoc_STR("get the internal state")},
  {"clear_patterns", Fsim_clear_patterns, METH_NOARGS,
   PyDoc_STR("clear patterns for ppsfp")},
  {"set_pattern", Fsim_set_pattern, METH_VARARGS,
   PyDoc_STR("set pattern for ppsfp")},
  {"get_pattern", Fsim_get_pattern, METH_VARARGS,
   PyDoc_STR("get pattern for ppsfp")},
  {"det_fault_num", Fsim_det_fault_num, METH_NOARGS,
   PyDoc_STR("return the number of detected faults")},
  {"det_fault", Fsim_det_fault, METH_VARARGS,
   PyDoc_STR("return the detected fault")},
  {"det_fault_list", Fsim_det_fault_list, METH_NOARGS,
   PyDoc_STR("return the list of all the detected faults")},
  {"det_fault_pat", Fsim_det_fault_pat, METH_VARARGS,
   PyDoc_STR("return the detecting pattern bits")},
  {"det_fault_pat_list", Fsim_det_fault_pat_list, METH_NOARGS,
   PyDoc_STR("return the list of the detecting pattern bits")},
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
  FsimType.tp_new = Fsim_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "Fsim", &FsimType) ) {
    goto error;
  }

  return true;

 error:

  return false;
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
