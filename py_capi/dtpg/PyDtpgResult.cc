
/// @file PyDtpgResult.cc
/// @brief PyDtpgResult の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDtpgResult.h"
#include "pym/PyTestVector.h"
#include "pym/PyFaultStatus.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct DtpgResult_Object
{
  PyObject_HEAD
  DtpgResult mVal;
};

// Python 用のタイプ定義
PyTypeObject DtpgResult_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyDtpgResult::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<DtpgResult_Object*>(self);
  obj->mVal.~DtpgResult();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
detected(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "testvect",
    nullptr
  };
  PyObject* testvect_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyTestVector::_typeobject(), &testvect_obj) ) {
    return nullptr;
  }
  TestVector testvect;
  if ( testvect_obj != nullptr ) {
    if ( !PyTestVector::FromPyObject(testvect_obj, testvect) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TestVector");
      return nullptr;
    }
  }
  try {
    return PyDtpgResult::ToPyObject(DtpgResult::detected(testvect));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
untestable(
  PyObject* Py_UNUSED(self),
  PyObject* Py_UNUSED(args)
)
{
  try {
    return PyDtpgResult::ToPyObject(DtpgResult::untestable());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
undetected(
  PyObject* Py_UNUSED(self),
  PyObject* Py_UNUSED(args)
)
{
  try {
    return PyDtpgResult::ToPyObject(DtpgResult::undetected());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
set(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "status",
    "testvect",
    nullptr
  };
  PyObject* status_obj = nullptr;
  PyObject* testvect_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|$O!O!",
                                    const_cast<char**>(kwlist),
                                    PyFaultStatus::_typeobject(), &status_obj,
                                    PyTestVector::_typeobject(), &testvect_obj) ) {
    return nullptr;
  }
  auto& val = PyDtpgResult::_get_ref(self);
  try {
    if ( status_obj != nullptr ) {
      if ( testvect_obj != nullptr ) {
        PyErr_SetString(PyExc_ValueError, "status and testvector are mutially exclusive");
        return nullptr;
      }
      auto& status = PyFaultStatus::_get_ref(status_obj);
      val.set(status);
    }
    else if ( testvect_obj != nullptr ) {
      auto& testvect = PyTestVector::_get_ref(testvect_obj);
      val.set(testvect);
    }
    else {
      PyErr_SetString(PyExc_ValueError, "either status or testvect must be specified");
      return nullptr;
    }
    Py_RETURN_NONE;
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
  {"detected",
   reinterpret_cast<PyCFunction>(detected),
   METH_VARARGS | METH_KEYWORDS | METH_STATIC,
   PyDoc_STR("return DETECTED object")},
  {"untestable",
   untestable,
   METH_NOARGS | METH_STATIC,
   PyDoc_STR("return UNTESTABLE object")},
  {"undetected",
   undetected,
   METH_NOARGS | METH_STATIC,
   PyDoc_STR("return UNDETECTED object")},
  {"set",
   reinterpret_cast<PyCFunction>(set),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set status")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
get_status(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResult::_get_ref(self);
  try {
    return PyFaultStatus::ToPyObject(val.status());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_testvector(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDtpgResult::_get_ref(self);
  try {
    return PyTestVector::ToPyObject(val.testvector());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// getter/setter定義
PyGetSetDef getsets[] = {
  {"status", get_status, nullptr, PyDoc_STR("fault status"), nullptr},
  {"testvector", get_testvector, nullptr, PyDoc_STR("testvector"), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief DtpgResult オブジェクトを使用可能にする．
bool
PyDtpgResult::init(
  PyObject* m
)
{
  DtpgResult_Type.tp_name = "DtpgResult";
  DtpgResult_Type.tp_basicsize = sizeof(DtpgResult_Object);
  DtpgResult_Type.tp_itemsize = 0;
  DtpgResult_Type.tp_dealloc = dealloc_func;
  DtpgResult_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  DtpgResult_Type.tp_doc = PyDoc_STR("Python extended object for DtpgResult");
  DtpgResult_Type.tp_methods = methods;
  DtpgResult_Type.tp_getset = getsets;
  if ( !PyModule::reg_type(m, "DtpgResult", &DtpgResult_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// DtpgResult を PyObject に変換する．
PyObject*
PyDtpgResult::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyDtpgResult::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<DtpgResult_Object*>(obj);
  new (&my_obj->mVal) DtpgResult(val);
  return obj;
}

// @brief PyObject が DtpgResult タイプか調べる．
bool
PyDtpgResult::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &DtpgResult_Type);
}

// @brief PyObject から DtpgResult を取り出す．
DtpgResult&
PyDtpgResult::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<DtpgResult_Object*>(obj);
  return my_obj->mVal;
}

// @brief DtpgResult を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgResult::_typeobject()
{
  return &DtpgResult_Type;
}

END_NAMESPACE_DRUID
