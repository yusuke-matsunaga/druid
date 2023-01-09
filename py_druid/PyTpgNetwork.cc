
/// @file PyTpgNetwork.cc
/// @brief Python TpgNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PyTpgNetwork.h"
#include "PyTpgFault.h"
#include "ym/PyClibCellLibrary.h"
#include "ym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TpgNetworkObject
{
  PyObject_HEAD
  TpgNetwork* mPtr;
};

// Python 用のタイプ定義
PyTypeObject TpgNetworkType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
TpgNetwork_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'TpgNetwork' is disabled");
  return nullptr;
  auto self = type->tp_alloc(type, 0);
  auto tpgnetwork_obj = reinterpret_cast<TpgNetworkObject*>(self);
  tpgnetwork_obj->mPtr = new TpgNetwork;
  return self;
}

// 終了関数
void
TpgNetwork_dealloc(
  PyObject* self
)
{
  auto tpgnetwork_obj = reinterpret_cast<TpgNetworkObject*>(self);
  delete tpgnetwork_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
TpgNetwork_read_blif(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "cell_library",
    nullptr
  };
  const char* blif_file = nullptr;
  PyObject* clib_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|$!O",
				    const_cast<char**>(kwlist),
				    &blif_file,
				    PyClibCellLibrary::_typeobject(),
				    &clib_obj) ) {
    return nullptr;
  }
  auto obj = TpgNetworkType.tp_alloc(&TpgNetworkType, 0);
  auto tpgnetwork_obj = reinterpret_cast<TpgNetworkObject*>(obj);
  auto network_p = new TpgNetwork;
  tpgnetwork_obj->mPtr = network_p;

  // blif ファイルを読み込む．
  if ( clib_obj == nullptr ) {
    if ( !network_p->read_blif(blif_file) ) {
      PyErr_SetString(PyExc_ValueError, "read failed");
      TpgNetwork_dealloc(obj);
      return nullptr;
    }
  }
  else {
    auto& cell_library = PyClibCellLibrary::_get(clib_obj);
    if ( !network_p->read_blif(blif_file, cell_library) ) {
      PyErr_SetString(PyExc_ValueError, "read failed");
      TpgNetwork_dealloc(obj);
      return nullptr;
    }
  }
  return obj;
}

PyObject*
TpgNetwork_read_bench(
  PyObject* Py_UNUSED(self),
  PyObject* args
)
{
  const char* bench_file = nullptr;
  if ( !PyArg_ParseTuple(args, "s", &bench_file) ) {
    return nullptr;
  }
  auto obj = TpgNetworkType.tp_alloc(&TpgNetworkType, 0);
  auto tpgnetwork_obj = reinterpret_cast<TpgNetworkObject*>(obj);
  auto network_p = new TpgNetwork;
  tpgnetwork_obj->mPtr = network_p;

  // iscas89(.bench) ファイルを読み込む．
  if ( !network_p->read_iscas89(bench_file) ) {
    PyErr_SetString(PyExc_ValueError, "read failed");
    TpgNetwork_dealloc(obj);
    return nullptr;
  }
  return obj;
}

PyObject*
TpgNetwork_rep_fault_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto network_p = PyTpgNetwork::_get(self);
  auto& fault_list = network_p->rep_fault_list();
  SizeType n = fault_list.size();
  auto ans_obj = PyList_New(n);
  Py_IncRef(ans_obj);
  SizeType index = 0;
  for ( auto fault: fault_list ) {
    auto fault_obj = PyTpgFault::ToPyObject(fault);
    PyList_SET_ITEM(ans_obj, index, fault_obj);
    ++ index;
  }
  return ans_obj;
}

// メソッド定義
PyMethodDef TpgNetwork_methods[] = {
  {"read_blif", reinterpret_cast<PyCFunction>(TpgNetwork_read_blif),
   METH_VARARGS | METH_STATIC, PyDoc_STR("read 'blif' format")},
  {"read_bench", TpgNetwork_read_bench,
   METH_VARARGS | METH_STATIC, PyDoc_STR("read 'iscas89(.bench)' format")},
  {"rep_fault_list", TpgNetwork_rep_fault_list,
   METH_NOARGS, PyDoc_STR("generate a list for all representative faults")},
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief 'TpgNetwork' オブジェクトを使用可能にする．
bool
PyTpgNetwork::init(
  PyObject* m
)
{
  TpgNetworkType.tp_name = "TpgNetwork";
  TpgNetworkType.tp_basicsize = sizeof(TpgNetworkObject);
  TpgNetworkType.tp_itemsize = 0;
  TpgNetworkType.tp_dealloc = TpgNetwork_dealloc;
  TpgNetworkType.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgNetworkType.tp_doc = PyDoc_STR("TpgNetwork object");
  TpgNetworkType.tp_methods = TpgNetwork_methods;
  TpgNetworkType.tp_new = TpgNetwork_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "TpgNetwork", &TpgNetworkType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が TpgNetwork タイプか調べる．
bool
PyTpgNetwork::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TpgNetwork を表す PyObject から TpgNetwork を取り出す．
TpgNetwork*
PyTpgNetwork::_get(
  PyObject* obj
)
{
  auto tpgnetwork_obj = reinterpret_cast<TpgNetworkObject*>(obj);
  return tpgnetwork_obj->mPtr;
}

// @brief TpgNetwork を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgNetwork::_typeobject()
{
  return &TpgNetworkType;
}

END_NAMESPACE_DRUID
