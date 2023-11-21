
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
#include "PyTpgFaultMgr.h"
#include "pym/PyModule.h"


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
  // 位置引数もキーワード引数も受け取らない．
  static const char* kwlist[] = {
    nullptr
  };
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "",
				    const_cast<char**>(kwlist)) ) {
    return nullptr;
  }

  auto self = type->tp_alloc(type, 0);
  auto fsim_obj = reinterpret_cast<FsimObject*>(self);
  fsim_obj->mPtr = new Fsim;
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
Fsim_initialize(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "network",
    "prev_state",
    "val_type",
    nullptr
  };
  PyObject* network_obj = nullptr;
  int i_prev_state = 0;
  int val_type = 0;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!pi",
				    const_cast<char**>(kwlist),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    &i_prev_state, &val_type) ) {
    return nullptr;
  }

  auto& network = PyTpgNetwork::Get(network_obj);
  bool prev_state = static_cast<bool>(i_prev_state);

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
  auto fsim_obj = reinterpret_cast<FsimObject*>(self);
  fsim_obj->mPtr->initialize(network, prev_state, has_x);
  Py_RETURN_NONE;
}

PyObject*
Fsim_set_fault_list(
  PyObject* self,
  PyObject* args
)
{
  PyObject* fault_list_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &fault_list_obj) ) {
    return nullptr;
  }

  vector<TpgFault> fault_list;
  if ( !PyTpgFault::FromPyList(fault_list_obj, fault_list) ) {
    return nullptr;
  }
  auto fsim = PyFsim::Get(self);
  fsim->set_fault_list(fault_list);
  Py_RETURN_NONE;
}

PyObject*
Fsim_set_skip_all(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fsim = PyFsim::Get(self);
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

  vector<TpgFault> fault_list;
  if ( !PyTpgFault::FromPyList(obj, fault_list) ) {
    return nullptr;
  }
  auto fsim = PyFsim::Get(self);
  fsim->set_skip(fault_list);
  Py_RETURN_NONE;
}

PyObject*
Fsim_clear_skip_all(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto fsim = PyFsim::Get(self);
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

  vector<TpgFault> fault_list;
  if ( !PyTpgFault::FromPyList(obj, fault_list) ) {
    return nullptr;
  }
  auto fsim = PyFsim::Get(self);
  fsim->clear_skip(fault_list);
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
  auto tv = PyTestVector::Get(obj1);
  auto fault = PyTpgFault::Get(obj2);
  auto fsim = PyFsim::Get(self);
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
  auto tv = PyTestVector::Get(obj1);
  auto fsim = PyFsim::Get(self);
  auto fault_list = fsim->sppfp(tv);
  return PyTpgFault::ToPyList(fault_list);
}

PyObject*
Fsim_ppsfp(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "tv_list",
    "callback",
    nullptr
  };
  PyObject* tv_list_obj = nullptr;
  PyObject* callback_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "OO",
				    const_cast<char**>(kwlist),
				    &tv_list_obj, &callback_obj) ) {
    return nullptr;
  }
  if ( !PyCallable_Check(callback_obj) ) {
    PyErr_SetString(PyExc_TypeError, "2nd parameter must be callable");
    return nullptr;
  }

  vector<TestVector> tv_list;
  if ( !PyTestVector::FromPyList(tv_list_obj, tv_list) ) {
    return nullptr;
  }
  auto fsim = PyFsim::Get(self);
  bool ng = false;
  bool ans = fsim->ppsfp(tv_list, [&](SizeType index,
				      TpgFault f,
				      DiffBits dbits)->bool
  {
    auto fault_obj = PyTpgFault::ToPyObject(f);
    auto cb_args = Py_BuildValue("kO", index, fault_obj);
    auto ans_obj = PyObject_CallObject(callback_obj, cb_args);
    Py_DECREF(cb_args);
    if ( ans_obj == nullptr ) {
      ng = true;
      return false;
    }
    auto ans = PyObject_IsTrue(ans_obj);
    Py_DECREF(ans_obj);
    return ans;
  });
  if ( ng ) {
    return nullptr;
  }
  return PyBool_FromLong(ans);
}

PyObject*
Fsim_calc_wsa(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  int weighted = false;
  if ( !PyArg_ParseTuple(args, "O!p",
			 PyInputVector::_typeobject(), &obj1,
			 &weighted) ) {
    return nullptr;
  }
  auto iv = PyInputVector::Get(obj1);
  auto fsim = PyFsim::Get(self);
  SizeType n = fsim->calc_wsa(iv, static_cast<bool>(weighted));
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
  auto iv = PyInputVector::Get(obj1);
  auto dv = PyDffVector::Get(obj2);
  auto fsim = PyFsim::Get(self);
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
  auto fsim = PyFsim::Get(self);
  fsim->get_state(iv, dv);
  auto obj1 = PyInputVector::ToPyObject(iv);
  auto obj2 = PyDffVector::ToPyObject(dv);
  return Py_BuildValue("OO", obj1, obj2);
}

// メソッド定義
PyMethodDef Fsim_methods[] = {
  {"initialize", reinterpret_cast<PyCFunction>(Fsim_initialize),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("initialize")},
  {"set_fault_list", Fsim_set_fault_list, METH_VARARGS,
   PyDoc_STR("set fault list")},
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
  {"ppsfp", reinterpret_cast<PyCFunction>(Fsim_ppsfp),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("do Parallel Pattern Single Fault Propagation simulation")},
  {"calc_wsa", Fsim_calc_wsa, METH_NOARGS,
   PyDoc_STR("calculate weighted sum of activities")},
  {"set_state", Fsim_set_state, METH_VARARGS,
   PyDoc_STR("set the internal state")},
  {"get_state", Fsim_get_state, METH_NOARGS,
   PyDoc_STR("get the internal state")},
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
PyFsim::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief Fsim を表す PyObject から Fsim を取り出す．
Fsim*
PyFsim::Get(
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
