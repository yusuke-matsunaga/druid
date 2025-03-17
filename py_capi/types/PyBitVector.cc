
/// @file PyBitVector.cc
/// @brief Python BitVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyBitVector.h"
#include "pym/PyVal3.h"
#include "pym/PyMt19937.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct BitVectorObject
{
  PyObject_HEAD
  BitVector* mPtr;
};

// Python 用のタイプ定義
PyTypeObject BitVectorType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
BitVector_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  SizeType len = 0;
  static const char* kwlist[] = {
    "len",
    nullptr
  };
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i",
				    const_cast<char**>(kwlist),
				    &len) ) {
    return nullptr;
  }

  auto self = type->tp_alloc(type, 0);
  auto bv_obj = reinterpret_cast<BitVectorObject*>(self);
  bv_obj->mPtr = new BitVector{len};
  return self;
}

// 終了関数
void
BitVector_dealloc(
  PyObject* self
)
{
  auto bv_obj = reinterpret_cast<BitVectorObject*>(self);
  delete bv_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

// repr() 関数
PyObject*
BitVector_str(
  PyObject* self
)
{
  auto& bv = PyBitVector::_get_ref(self);
  auto tmp_str = bv.bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
BitVector_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  // TODO: pos の範囲チェック
  auto& bv = PyBitVector::_get_ref(self);
  auto val = bv.val(pos);
  return PyVal3::ToPyObject(val);
}

PyObject*
BitVector_x_count(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& bv = PyBitVector::_get_ref(self);
  auto val = bv.x_count();
  return PyLong_FromLong(val);
}

PyObject*
BitVector_bin_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& bv = PyBitVector::_get_ref(self);
  auto tmp_str = bv.bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
BitVector_hex_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& bv = PyBitVector::_get_ref(self);
  auto tmp_str = bv.hex_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
BitVector_init_method(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto bv_obj = reinterpret_cast<BitVectorObject*>(self);
  auto& bv = *(bv_obj->mPtr);
  bv.init();
  Py_RETURN_NONE;
}

PyObject*
BitVector_from_bin(
  PyObject* Py_UNUSED(self),
  PyObject* args
)
{
  char* bin_str = nullptr;
  if ( !PyArg_ParseTuple(args, "s", &bin_str) ) {
    return nullptr;
  }
  auto bv = BitVector::from_bin(bin_str);
  return PyBitVector::ToPyObject(bv);
}

PyObject*
BitVector_from_hex(
  PyObject* Py_UNUSED(self),
  PyObject* args
)
{
  SizeType len = 0;
  char* hex_str = nullptr;
  if ( !PyArg_ParseTuple(args, "ks", &len, &hex_str) ) {
    return nullptr;
  }
  auto bv = BitVector::from_hex(len, hex_str);
  return PyBitVector::ToPyObject(bv);
}

PyObject*
BitVector_from_bits(
  PyObject* Py_UNUSED(self),
  PyObject* args
)
{
  PyObject* bits_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &bits_obj) ) {
    return nullptr;
  }
  if ( !PySequence_Check(bits_obj) ) {
    PyErr_SetString(PyExc_TypeError, "not as list of bits(0, 1)");
    return nullptr;
  }
  SizeType n = PySequence_Size(bits_obj);
  string bin_str = "";
  for ( SizeType i = 0; i < n; ++ i ) {
    auto obj1 = PySequence_GetItem(bits_obj, i);
    if ( !PyLong_Check(obj1) ) {
      Py_DECREF(obj1);
      PyErr_SetString(PyExc_TypeError, "not as list of bits(0, 1)");
      return nullptr;
    }
    auto v = PyLong_AsLong(obj1);
    if ( v == 0 ) {
      bin_str += '0';
    }
    else if ( v == 1 ) {
      bin_str += '1';
    }
    else {
      Py_DECREF(obj1);
      PyErr_SetString(PyExc_TypeError, "not as list of bits(0, 1)");
      return nullptr;
    }
  }
  auto bv = BitVector::from_bin(bin_str);
  return PyBitVector::ToPyObject(bv);
}

PyObject*
BitVector_set_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "kO", &pos, &obj) ) {
    return nullptr;
  }
  Val3 val;
  if ( !PyVal3::FromPyObject(obj, val) ) {
    return nullptr;
  }
  auto bv_obj = reinterpret_cast<BitVectorObject*>(self);
  auto& bv = *(bv_obj->mPtr);
  bv.set_val(pos, val);
  Py_RETURN_NONE;
}

PyObject*
BitVector_set_from_random(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!", PyMt19937::_typeobject(), &obj) ) {
    return nullptr;
  }

  auto& mt19937 = PyMt19937::_get_ref(obj);
  auto bv_obj = reinterpret_cast<BitVectorObject*>(self);
  auto& bv = *(bv_obj->mPtr);
  bv.set_from_random(mt19937);
  Py_RETURN_NONE;
}

PyObject*
BitVector_fix_x_from_random(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!", PyMt19937::_typeobject(), &obj) ) {
    return nullptr;
  }

  auto& mt19937 = PyMt19937::_get_ref(obj);
  auto bv_obj = reinterpret_cast<BitVectorObject*>(self);
  auto& bv = *(bv_obj->mPtr);
  bv.fix_x_from_random(mt19937);
  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef BitVector_methods[] = {
  {"val", BitVector_val, METH_VARARGS,
   PyDoc_STR("returns a value of the specified bit")},
  {"x_count", BitVector_x_count, METH_NOARGS,
   PyDoc_STR("returns a number of 'X' bits")},
  {"bin_str", BitVector_bin_str, METH_NOARGS,
   PyDoc_STR("returns a string representation in binary format")},
  {"hex_str", BitVector_hex_str, METH_NOARGS,
   PyDoc_STR("returns a string representation in hexadecimal format")},
  {"init", BitVector_init_method, METH_NOARGS,
   PyDoc_STR("returns a string representation in hexadecimal format")},
  {"from_bin", reinterpret_cast<PyCFunction>(BitVector_from_bin),
   METH_VARARGS | METH_KEYWORDS | METH_STATIC,
   PyDoc_STR("create new object from BIN string")},
  {"from_hex", reinterpret_cast<PyCFunction>(BitVector_from_hex),
   METH_VARARGS | METH_KEYWORDS | METH_STATIC,
   PyDoc_STR("create new object from HEX string")},
  {"from_bits", BitVector_from_bits,
   METH_VARARGS  | METH_STATIC,
   PyDoc_STR("create new object from bit list")},
  {"set_val", BitVector_set_val, METH_VARARGS,
   PyDoc_STR("set value of the specified PPI")},
  {"set_from_random", BitVector_set_from_random, METH_VARARGS,
   PyDoc_STR("set value randomly")},
  {"fix_x_from_random", BitVector_fix_x_from_random, METH_VARARGS,
   PyDoc_STR("fix 'X' values randomly")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
BitVector_len(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& bv = PyBitVector::_get_ref(self);
  return Py_BuildValue("k", bv.len());
}

PyGetSetDef BitVector_getset[] = {
  {"len", BitVector_len, nullptr,
   PyDoc_STR("length"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr}
};

PyObject*
BitVector_and(
  PyObject* self,
  PyObject* other
)
{
  if ( !Py_IS_TYPE(self, &BitVectorType) ||
       !Py_IS_TYPE(other, &BitVectorType) ) {
    // 型が合わない．
    PyErr_SetString(PyExc_TypeError, "both arguments sould be BitVector type");
    return nullptr;
  }
  auto& bv = PyBitVector::_get_ref(self);
  auto& bv1 = PyBitVector::_get_ref(other);
  auto ans = bv & bv1;
  return PyBitVector::ToPyObject(ans);
}

// 数値演算メソッド定義
PyNumberMethods BitVectorNumber = {
  .nb_and = BitVector_and,
};

END_NONAMESPACE


// @brief 'BitVector' オブジェクトを使用可能にする．
bool
PyBitVector::init(
  PyObject* m
)
{
  BitVectorType.tp_name = "BitVector";
  BitVectorType.tp_basicsize = sizeof(BitVectorObject);
  BitVectorType.tp_itemsize = 0;
  BitVectorType.tp_dealloc = BitVector_dealloc;
  BitVectorType.tp_flags = Py_TPFLAGS_DEFAULT;
  BitVectorType.tp_doc = PyDoc_STR("BitVector object");
  BitVectorType.tp_methods = BitVector_methods;
  BitVectorType.tp_getset = BitVector_getset;
  BitVectorType.tp_new = BitVector_new;
  BitVectorType.tp_str = BitVector_str;
  BitVectorType.tp_as_number = &BitVectorNumber;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "BitVector", &BitVectorType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief BitVector を PyObject に変換する．
PyObject*
PyBitVectorConv::operator()(
  BitVector val
)
{
  auto obj = BitVectorType.tp_alloc(&BitVectorType, 0);
  auto bv_obj = reinterpret_cast<BitVectorObject*>(obj);
  bv_obj->mPtr = new BitVector{val};
  return obj;
}

// @brief PyObject* から const BitVector* を取り出す．
bool
PyBitVectorDeconv::operator()(
  PyObject* obj,
  BitVector& val
)
{
  if ( PyBitVector::_check(obj) ) {
    val = PyBitVector::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が BitVector タイプか調べる．
bool
PyBitVector::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief BitVector を表す PyObject から BitVector を取り出す．
const BitVector&
PyBitVector::_get_ref(
  PyObject* obj
)
{
  auto bv_obj = reinterpret_cast<BitVectorObject*>(obj);
  return *bv_obj->mPtr;
}

// @brief BitVector を表すオブジェクトの型定義を返す．
PyTypeObject*
PyBitVector::_typeobject()
{
  return &BitVectorType;
}

END_NAMESPACE_DRUID
