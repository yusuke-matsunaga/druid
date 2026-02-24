
/// @file PyDtpgMgr.cc
/// @brief PyDtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDtpgMgr.h"
#include "pym/PyTpgFaultList.h"
#include "pym/PyDtpgResults.h"
#include "pym/PyDtpgStats.h"
#include "pym/PyJsonValue.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct DtpgMgr_Object
{
  PyObject_HEAD
  DtpgMgr mVal;
};

// Python 用のタイプ定義
PyTypeObject DtpgMgr_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyDtpgMgr::init() 中で初期化する．
};

PyObject*
run(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "fault_list",
    "option",
    nullptr
  };
  PyObject* fault_list_obj = nullptr;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!|$O",
                                    const_cast<char**>(kwlist),
                                    PyTpgFaultList::_typeobject(), &fault_list_obj,
                                    &option_obj) ) {
    return nullptr;
  }
  TpgFaultList fault_list;
  if ( fault_list_obj != nullptr ) {
    if ( !PyTpgFaultList::FromPyObject(fault_list_obj, fault_list) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TpgFaultList");
      return nullptr;
    }
  }
  JsonValue option;
  if ( option_obj != nullptr ) {
    if ( !PyJsonValue::FromPyObject(option_obj, option) ) {
      PyErr_SetString(PyExc_ValueError, "could not convert to JsonValue");
      return nullptr;
    }
  }
  try {
    DtpgResults results;
    auto stats = DtpgMgr::run(fault_list, results, option);
    auto val1 = PyDtpgStats::ToPyObject(stats);
    auto val2 = PyDtpgResults::ToPyObject(results);
    return Py_BuildValue("(OO)", val1, val2);
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// メソッド定義
PyMethodDef methods[] = {
  {"run",
   reinterpret_cast<PyCFunction>(run),
   METH_VARARGS | METH_KEYWORDS | METH_STATIC,
   PyDoc_STR("do DTPG")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief DtpgMgr オブジェクトを使用可能にする．
bool
PyDtpgMgr::init(
  PyObject* m
)
{
  DtpgMgr_Type.tp_name = "DtpgMgr";
  DtpgMgr_Type.tp_basicsize = sizeof(DtpgMgr_Object);
  DtpgMgr_Type.tp_itemsize = 0;
  DtpgMgr_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  DtpgMgr_Type.tp_doc = PyDoc_STR("Python extended object for DtpgMgr");
  DtpgMgr_Type.tp_methods = methods;
  if ( !PyModule::reg_type(m, "DtpgMgr", &DtpgMgr_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が DtpgMgr タイプか調べる．
bool
PyDtpgMgr::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &DtpgMgr_Type);
}

// @brief PyObject から DtpgMgr を取り出す．
DtpgMgr&
PyDtpgMgr::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<DtpgMgr_Object*>(obj);
  return my_obj->mVal;
}

// @brief DtpgMgr を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgMgr::_typeobject()
{
  return &DtpgMgr_Type;
}

END_NAMESPACE_DRUID
