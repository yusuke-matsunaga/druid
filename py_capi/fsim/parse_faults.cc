
/// @file parse_fsim.cc
/// @brief parse_fsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

// TpgFault もしくは TpgFaultList をパースする．
bool
parse_faults(
  PyObject* args,
  TpgFaultList& fault_list
)
{
  if ( PyTpgFault::Check(args) ) {
    auto& fault = PyTpgFault::_get_ref(args);
    fault_list.push_back(fault);
    return true;
  }
  if ( PyTpgFaultList::Check(args) ) {
    fault_list = PyTpgFaultList::_get_ref(args);
    return true;
  }
  if ( PySequence_Check(args) ) {
    auto n = PySequence_Size(args);
    for ( SizeType i = 0; i < n; ++ i ) {
      auto arg = PySequence_GetItem(args, i);
      if ( !PyTpgFault::Check(arg) ) {
	goto error;
      }
      auto fault = PyTpgFault::_get_ref(arg);
      fault_list.push_back(fault);
    }
    return true;
  }
error:
  PyErr_SetString(PyExc_TypeError,
		  "TpgFault or sequence of TpgFaults are expected");
  return false;
}
