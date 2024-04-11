
/// @file PyFsim.cc
/// @brief Python Fsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PyFsim.h"
#include "PyDiffBits.h"
#include "PyDiffBitsArray.h"
#include "PyTpgNetwork.h"
#include "PyTestVector.h"
#include "PyInputVector.h"
#include "PyDffVector.h"
#include "PyFaultType.h"
#include "PyTpgFault.h"
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
    "fault_list",
    "val_type",
    "multi",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_list_obj = nullptr;
  int val_type = 0;
  int i_multi = 0;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!Oi|p",
				    const_cast<char**>(kwlist),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    &fault_list_obj,
				    &val_type, &i_multi) ) {
    return nullptr;
  }

  auto& network = PyTpgNetwork::Get(network_obj);
  vector<const TpgFault*> fault_list;
  if ( !PyTpgFault::FromPyList(fault_list_obj, fault_list) ) {
    return nullptr;
  }
  bool multi = static_cast<bool>(i_multi);

  bool has_x = false;
  if ( val_type == 2 ) {
    ;
  }
  else if ( val_type == 3 ) {
    has_x = true;
  }
  else {
    PyErr_SetString(PyExc_ValueError, "argument 2 must be 2 or 3");
    return nullptr;
  }
  auto fsim_obj = reinterpret_cast<FsimObject*>(self);
  fsim_obj->mPtr->initialize(network, fault_list, has_x, multi);
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
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "fault_list",
    nullptr
  };

  PyObject* obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
				    const_cast<char**>(kwlist),
				    &obj) ) {
    return nullptr;
  }

  vector<const TpgFault*> fault_list;
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
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "fault_list",
    nullptr
  };

  PyObject* obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
				    const_cast<char**>(kwlist),
				    &obj) ) {
    return nullptr;
  }

  vector<const TpgFault*> fault_list;
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
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "tv",
    "fault",
    nullptr
  };

  PyObject* obj1 = nullptr;
  PyObject* obj2 = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!",
				    const_cast<char**>(kwlist),
				    PyTestVector::_typeobject(), &obj1,
				    PyTpgFault::_typeobject(), &obj2) ) {
    return nullptr;
  }
  auto tv = PyTestVector::Get(obj1);
  auto fault = PyTpgFault::Get(obj2);
  auto fsim = PyFsim::Get(self);
  DiffBits dbits;
  bool ans = fsim->spsfp(tv, fault, dbits);
  PyObject* dbits_obj = Py_None;
  if ( ans ) {
    dbits_obj = PyDiffBits::ToPyObject(dbits);
  }
  return Py_BuildValue("(pO)", ans, dbits_obj);
}

PyObject*
Fsim_sppfp(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "tv",
    nullptr
  };

  PyObject* obj1 = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
				    const_cast<char**>(kwlist),
				    PyTestVector::_typeobject(), &obj1) ) {
    return nullptr;
  }
  auto tv = PyTestVector::Get(obj1);
  auto fsim = PyFsim::Get(self);
  vector<pair<const TpgFault*, DiffBits>> ans_list;
  fsim->sppfp(tv,
	      [&](
		const TpgFault* f,
		const DiffBits& dbits
	      )
	      {
		ans_list.push_back({f, dbits});
	      });
  SizeType n = ans_list.size();
  auto ans_obj = PyList_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto& p = ans_list[i];
    auto fault = p.first;
    auto fault_obj = PyTpgFault::ToPyObject(fault);
    auto dbits = p.second;
    auto dbits_obj = PyDiffBits::ToPyObject(dbits);
    auto tuple_obj = Py_BuildValue("(OO)", fault_obj, dbits_obj);
    PyList_SetItem(ans_obj, i, tuple_obj);
  }
  return ans_obj;
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
  if ( PyList_Size(tv_list_obj) > PV_BITLEN ) {
    PyErr_SetString(PyExc_ValueError, "1st parameter's size is too big");
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
  fsim->ppsfp(tv_list,
	      [&](
		const TpgFault* f,
		const DiffBitsArray& dbits_array
	      )
	      {
		auto fault_obj = PyTpgFault::ToPyObject(f);
		auto dbits_obj = PyDiffBitsArray::ToPyObject(dbits_array);
		auto cb_args = Py_BuildValue("OO", fault_obj, dbits_obj);
		PyObject_CallObject(callback_obj, cb_args);
		Py_DECREF(cb_args);
	      });
  Py_RETURN_NONE;
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
  {"set_skip_all", Fsim_set_skip_all, METH_NOARGS,
   PyDoc_STR("set skip mark for all fatuls")},
  {"set_skip", reinterpret_cast<PyCFunction>(Fsim_set_skip),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set skip mark for the designatated faults")},
  {"clear_skip_all", Fsim_set_skip_all, METH_NOARGS,
   PyDoc_STR("clear skip mark for all fatuls")},
  {"clear_skip", reinterpret_cast<PyCFunction>(Fsim_set_skip),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("clear skip mark for the designatated faults")},
  {"spsfp", reinterpret_cast<PyCFunction>(Fsim_spsfp),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("do Single Pattern Single Fault Propagation simulation")},
  {"sppfp", reinterpret_cast<PyCFunction>(Fsim_sppfp),
   METH_VARARGS | METH_KEYWORDS,
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

PyObject*
Fsim_ppsfp_bitlen(
  PyObject* Py_UNUSED(self),
  void* Py_UNUSED(colosure)
)
{
  return Py_BuildValue("i", PV_BITLEN);
}

// getsetter 定義
PyGetSetDef Fsim_getsetters[] = {
  {"ppsfp_bitlen", Fsim_ppsfp_bitlen, nullptr,
   PyDoc_STR("bit length for PPSFP"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr}
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
  FsimType.tp_getset = Fsim_getsetters;
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
