
/// @file PyTpgNetwork.cc
/// @brief Python TpgNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PyTpgNetwork.h"
#include "PyTpgMFFC.h"
#include "PyTpgFFR.h"
#include "PyTpgFault.h"
#include "pym/PyClibCellLibrary.h"
#include "pym/PyModule.h"


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
  // blif ファイルを読み込む．
  ClibCellLibrary cell_library;
  if ( clib_obj != nullptr ) {
    cell_library = PyClibCellLibrary::Get(clib_obj);
  }
  try {
    auto src_network = TpgNetwork::read_blif(blif_file, cell_library);
    auto obj = TpgNetworkType.tp_alloc(&TpgNetworkType, 0);
    auto tpgnetwork_obj = reinterpret_cast<TpgNetworkObject*>(obj);
    tpgnetwork_obj->mPtr = new TpgNetwork{std::move(src_network)};
    return obj;
  }
  catch ( std::invalid_argument ) {
    ostringstream buf;
    buf << "Error occured in reading \"" << blif_file << "\"";
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
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
  try {
    auto src_network = TpgNetwork::read_iscas89(bench_file);
    auto obj = TpgNetworkType.tp_alloc(&TpgNetworkType, 0);
    auto tpgnetwork_obj = reinterpret_cast<TpgNetworkObject*>(obj);
    tpgnetwork_obj->mPtr = new TpgNetwork{std::move(src_network)};
    return obj;
  }
  catch ( std::invalid_argument ) {
    ostringstream buf;
    buf << "Error occured in reading \"" << bench_file << "\"";
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// メソッド定義
PyMethodDef TpgNetwork_methods[] = {
  {"read_blif", reinterpret_cast<PyCFunction>(TpgNetwork_read_blif),
   METH_VARARGS | METH_STATIC, PyDoc_STR("read 'blif' format")},
  {"read_bench", TpgNetwork_read_bench,
   METH_VARARGS | METH_STATIC, PyDoc_STR("read 'iscas89(.bench)' format")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
TpgNetwork_node_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyTpgNetwork::Get(self);
  auto val = network.node_num();
  return PyLong_FromLong(val);
}

PyObject*
TpgNetwork_input_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyTpgNetwork::Get(self);
  auto val = network.input_num();
  return PyLong_FromLong(val);
}

PyObject*
TpgNetwork_output_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyTpgNetwork::Get(self);
  auto val = network.output_num();
  return PyLong_FromLong(val);
}

PyObject*
TpgNetwork_ppi_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyTpgNetwork::Get(self);
  auto val = network.ppi_num();
  return PyLong_FromLong(val);
}

PyObject*
TpgNetwork_ppo_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyTpgNetwork::Get(self);
  auto val = network.ppo_num();
  return PyLong_FromLong(val);
}

PyObject*
TpgNetwork_mffc_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyTpgNetwork::Get(self);
  auto val = network.mffc_num();
  return PyLong_FromLong(val);
}

PyObject*
TpgNetwork_ffr_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyTpgNetwork::Get(self);
  auto val = network.ffr_num();
  return PyLong_FromLong(val);
}

PyObject*
TpgNetwork_dff_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyTpgNetwork::Get(self);
  auto val = network.dff_num();
  return PyLong_FromLong(val);
}

PyObject*
TpgNetwork_mffc_list(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyTpgNetwork::Get(self);
  SizeType n = network.mffc_num();
  auto ans_obj = PyList_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto mffc = network.mffc(i);
    auto obj1 = PyTpgMFFC::ToPyObject(mffc);
    PyList_SetItem(ans_obj, i, obj1);
  }
  return ans_obj;
}

PyObject*
TpgNetwork_ffr_list(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyTpgNetwork::Get(self);
  SizeType n = network.ffr_num();
  auto ans_obj = PyList_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto ffr = network.ffr(i);
    auto obj1 = PyTpgFFR::ToPyObject(ffr);
    PyList_SetItem(ans_obj, i, obj1);
  }
  return ans_obj;
}

PyObject*
TpgNetwork_rep_fault_list(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& network = PyTpgNetwork::Get(self);
  auto& fault_list = network.rep_fault_list();
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

// get/set 関数定義
PyGetSetDef TpgNetwork_getset[] = {
  {"node_num", TpgNetwork_node_num, nullptr, PyDoc_STR("# of nodes"), nullptr},
  {"input_num", TpgNetwork_input_num, nullptr, PyDoc_STR("# of inputs"), nullptr},
  {"output_num", TpgNetwork_output_num, nullptr, PyDoc_STR("# of outputs"), nullptr},
  {"ppi_num", TpgNetwork_ppi_num, nullptr, PyDoc_STR("# of PPIs"), nullptr},
  {"ppo_num", TpgNetwork_ppo_num, nullptr, PyDoc_STR("# of PPOs"), nullptr},
  {"mffc_num", TpgNetwork_mffc_num, nullptr, PyDoc_STR("# of MFFCs"), nullptr},
  {"ffr_num", TpgNetwork_ffr_num, nullptr, PyDoc_STR("# of FFRs"), nullptr},
  {"dff_num", TpgNetwork_dff_num, nullptr, PyDoc_STR("# of D-FFs"), nullptr},
  {"mffc_list", TpgNetwork_mffc_list, nullptr, PyDoc_STR("list for all MFFCs"), nullptr},
  {"ffr_list", TpgNetwork_ffr_list, nullptr, PyDoc_STR("list for all FFRs"), nullptr},
  {"rep_fault_list", TpgNetwork_rep_fault_list, nullptr,
   PyDoc_STR("list for all representative faults"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr},
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
  TpgNetworkType.tp_getset = TpgNetwork_getset;
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
PyTpgNetwork::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TpgNetwork を表す PyObject から TpgNetwork を取り出す．
const TpgNetwork&
PyTpgNetwork::Get(
  PyObject* obj
)
{
  auto tpgnetwork_obj = reinterpret_cast<TpgNetworkObject*>(obj);
  return *tpgnetwork_obj->mPtr;
}

// @brief TpgNetwork を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgNetwork::_typeobject()
{
  return &TpgNetworkType;
}

END_NAMESPACE_DRUID
