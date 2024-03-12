
/// @file PyLFSR.cc
/// @brief Python LFSR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "PyLFSR.h"
#include "PyBitVector.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct LFSRObject
{
  PyObject_HEAD
  LFSR* mPtr;
};

// Python 用のタイプ定義
PyTypeObject LFSRType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
LFSR_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "bitlen",
    "tap_list",
    nullptr
  };
  SizeType bitlen = 0;
  PyObject* tap_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "kO",
				    const_cast<char**>(kwlist),
				    &bitlen, &tap_list_obj) ) {
    return nullptr;
  }
  if ( !PySequence_Check(tap_list_obj) ) {
    PyErr_SetString(PyExc_TypeError,
		    "2nd parameter must be a sequence of int");
    return nullptr;
  }
  SizeType n = PySequence_Size(tap_list_obj);
  vector<SizeType> tap_list(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto obj1 = PySequence_GetItem(tap_list_obj, i);
    if ( !PyLong_Check(obj1) ) {
      Py_DECREF(obj1);
      PyErr_SetString(PyExc_TypeError,
		      "2nd parameter must be a sequence of int");
      return nullptr;
    }
    SizeType n1 = PyLong_AsLong(obj1);
    if ( n1 < 0 || bitlen <= n1 ) {
      Py_DECREF(obj1);
      PyErr_SetString(PyExc_ValueError,
		      "range error in the 2nd parameter");
      return nullptr;
    }
    tap_list[i] = n1;
    Py_DECREF(obj1);
  }

  auto obj = type->tp_alloc(type, 0);
  auto lfsr_obj = reinterpret_cast<LFSRObject*>(obj);
  lfsr_obj->mPtr = new LFSR{bitlen, tap_list};
  return obj;
}

// 終了関数
void
LFSR_dealloc(
  PyObject* self
)
{
  auto lfsr_obj = reinterpret_cast<LFSRObject*>(self);
  delete lfsr_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

// str() 関数
PyObject*
LFSR_str(
  PyObject* self
)
{
  auto& lfsr = PyLFSR::Get(self);
  ostringstream buf;
  buf << "[";
  const char* comma = "";
  for ( auto p: lfsr.tap_list() ) {
    buf << comma << p;
    comma = ", ";
  }
  buf << "]: "
      << lfsr.bits().bin_str();
  auto tmp_str = buf.str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
LFSR_shift(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto lfsr_obj = reinterpret_cast<LFSRObject*>(self);
  auto& lfsr = *(lfsr_obj->mPtr);
  lfsr.shift();
  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef LFSR_methods[] = {
  {"shift", LFSR_shift, METH_NOARGS, PyDoc_STR("shift")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
LFSR_bitlen(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& lfsr = PyLFSR::Get(self);
  return Py_BuildValue("k", lfsr.bitlen());
}

PyObject*
LFSR_bits(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& lfsr = PyLFSR::Get(self);
  auto& bv = lfsr.bits();
  return PyBitVector::ToPyObject(bv);
}

int
LFSR_set_bits(
  PyObject* self,
  PyObject* val_obj,
  void* Py_UNUSED(closure)
)
{
  if ( !PyBitVector::Check(val_obj) ) {
    PyErr_SetString(PyExc_TypeError, "not a BitVector");
    return -1;
  }
  auto lfsr_obj = reinterpret_cast<LFSRObject*>(self);
  auto& lfsr = *(lfsr_obj->mPtr);
  auto& bv = PyBitVector::Get(val_obj);
  lfsr.set_bits(bv);
  return 0;
}

PyGetSetDef LFSR_getset[] = {
  {"bitlen", LFSR_bitlen, nullptr,
   PyDoc_STR("bit length"), nullptr},
  {"bits", LFSR_bits, LFSR_set_bits,
   PyDoc_STR("bits"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief 'LFSR' オブジェクトを使用可能にする．
bool
PyLFSR::init(
  PyObject* m
)
{
  LFSRType.tp_name = "LFSR";
  LFSRType.tp_basicsize = sizeof(LFSRObject);
  LFSRType.tp_itemsize = 0;
  LFSRType.tp_dealloc = LFSR_dealloc;
  LFSRType.tp_flags = Py_TPFLAGS_DEFAULT;
  LFSRType.tp_doc = PyDoc_STR("LFSR object");
  LFSRType.tp_methods = LFSR_methods;
  LFSRType.tp_getset = LFSR_getset;
  LFSRType.tp_new = LFSR_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "LFSR", &LFSRType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief LFSR を PyObject に変換する．
PyObject*
PyLFSR::ToPyObject(
  const LFSR& val
)
{
  auto obj = LFSRType.tp_alloc(&LFSRType, 0);
  auto ps_obj = reinterpret_cast<LFSRObject*>(obj);
  (*ps_obj->mPtr) = val;
  return obj;
}

// @brief PyObject が LFSR タイプか調べる．
bool
PyLFSR::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief LFSR を表す PyObject から LFSR を取り出す．
const LFSR&
PyLFSR::Get(
  PyObject* obj
)
{
  auto ps_obj = reinterpret_cast<LFSRObject*>(obj);
  return *ps_obj->mPtr;
}

// @brief LFSR を表すオブジェクトの型定義を返す．
PyTypeObject*
PyLFSR::_typeobject()
{
  return &LFSRType;
}

END_NAMESPACE_DRUID
