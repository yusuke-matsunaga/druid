
/// @file tpg_types_module.cc
/// @brief Python 用の tpg_types モジュールを定義する．
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "pym/PyVal3.h"
#include "pym/PyFval2.h"
#include "pym/PyFaultType.h"
#include "pym/PyFaultStatus.h"
#include "pym/PyTpgNetwork.h"
#include "pym/PyTpgNode.h"
#include "pym/PyTpgNodeList.h"
#include "pym/PyTpgNodeIter2.h"
#include "pym/PyTpgMFFC.h"
#include "pym/PyTpgMFFCList.h"
#include "pym/PyTpgMFFCIter2.h"
#include "pym/PyTpgFFR.h"
#include "pym/PyTpgFFRList.h"
#include "pym/PyTpgFFRIter2.h"
#include "pym/PyTpgGate.h"
#include "pym/PyTpgGateList.h"
#include "pym/PyTpgGateIter2.h"
#include "pym/PyTpgFault.h"
#include "pym/PyTpgFaultList.h"
#include "pym/PyTpgFaultIter2.h"
#include "pym/PyTestVector.h"
#include "pym/PyInputVector.h"
#include "pym/PyAssign.h"
#include "pym/PyAssignList.h"
#include "pym/PyAssignIter2.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// メソッド定義
PyMethodDef methods[] = {
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

// モジュール定義構造体
PyModuleDef tpg_types_module = {
  PyModuleDef_HEAD_INIT,
  "tpg_types",
  PyDoc_STR(""),
  -1,
  methods,
};

END_NONAMESPACE

PyMODINIT_FUNC
PyInit_tpg_types()
{
  auto m = PyModule::init(&tpg_types_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PyVal3::init(m) ) {
    goto error;
  }
  if ( !PyFval2::init(m) ) {
    goto error;
  }
  if ( !PyFaultType::init(m) ) {
    goto error;
  }
  if ( !PyFaultStatus::init(m) ) {
    goto error;
  }
  if ( !PyTpgNetwork::init(m) ) {
    goto error;
  }
  if ( !PyTpgNode::init(m) ) {
    goto error;
  }
  if ( !PyTpgNodeList::init(m) ) {
    goto error;
  }
  if ( !PyTpgNodeIter2::init(m) ) {
    goto error;
  }
  if ( !PyTpgMFFC::init(m) ) {
    goto error;
  }
  if ( !PyTpgMFFCList::init(m) ) {
    goto error;
  }
  if ( !PyTpgMFFCIter2::init(m) ) {
    goto error;
  }
  if ( !PyTpgFFR::init(m) ) {
    goto error;
  }
  if ( !PyTpgFFRList::init(m) ) {
    goto error;
  }
  if ( !PyTpgFFRIter2::init(m) ) {
    goto error;
  }
  if ( !PyTpgGate::init(m) ) {
    goto error;
  }
  if ( !PyTpgGateList::init(m) ) {
    goto error;
  }
  if ( !PyTpgGateIter2::init(m) ) {
    goto error;
  }
  if ( !PyTpgFault::init(m) ) {
    goto error;
  }
  if ( !PyTpgFaultList::init(m) ) {
    goto error;
  }
  if ( !PyTpgFaultIter2::init(m) ) {
    goto error;
  }
  if ( !PyTestVector::init(m) ) {
    goto error;
  }
  if ( !PyInputVector::init(m) ) {
    goto error;
  }
  if ( !PyAssign::init(m) ) {
    goto error;
  }
  if ( !PyAssignList::init(m) ) {
    goto error;
  }
  if ( !PyAssignIter2::init(m) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NAMESPACE_DRUID
