
/// @file PyClassifier.cc
/// @brief Python Classifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgNetwork.h"
#include "pym/PyTestVector.h"
#include "pym/PyFaultType.h"
#include "pym/PyTpgFault.h"
#include "Classifier.h"


BEGIN_NAMESPACE_DRUID

PyObject*
classify(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "network",
    "fault_list",
    "fault_type",
    "tv_list",
    "drop",
    "ppsfp",
    "multi",
    "verbose",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_list_obj = nullptr;
  PyObject* fault_type_obj = nullptr;
  PyObject* tv_list_obj = nullptr;
  int i_drop = 0;
  int i_ppsfp = 0;
  int i_multi = 0;
  int i_verbose = 0;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!OO!O|pppp",
				    const_cast<char**>(kwlist),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    &fault_list_obj,
				    PyFaultType::_typeobject(), &fault_type_obj,
				    &tv_list_obj,
				    &i_drop, &i_ppsfp, &i_multi, &i_verbose) ) {
    return nullptr;
  }

  auto& network = PyTpgNetwork::Get(network_obj);
  vector<const TpgFault*> fault_list;
  if ( !PyTpgFault::FromPyList(fault_list_obj, fault_list) ) {
    return nullptr;
  }
  auto fault_type = PyFaultType::Get(fault_type_obj);
  vector<TestVector> tv_list;
  if ( !PyTestVector::FromPyList(tv_list_obj, tv_list) ) {
    return nullptr;
  }
  bool drop = static_cast<bool>(i_drop);
  bool ppsfp = static_cast<bool>(i_ppsfp);
  bool multi = static_cast<bool>(i_multi);
  bool verbose = static_cast<bool>(i_verbose);

  Classifier::set_verbose(verbose);

  auto fg_list = Classifier::run(network, fault_list, tv_list, drop, ppsfp, multi);
  SizeType NG = fg_list.size();
  auto ans_obj = PyList_New(NG);
  for ( SizeType i = 0; i < NG; ++ i ) {
    auto& fg = fg_list[i];
    SizeType N = fg.size();
    auto group_obj = PyList_New(N);
    for ( SizeType j = 0; j < N; ++ j ) {
      auto& f = fg[j];
      auto fault_obj = PyTpgFault::ToPyObject(f);
      PyList_SetItem(group_obj, j, fault_obj);
    }
    PyList_SetItem(ans_obj, i, group_obj);
  }

  return ans_obj;
}

END_NAMESPACE_DRUID
