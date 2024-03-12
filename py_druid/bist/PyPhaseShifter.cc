
/// @file PyPhaseShifter.cc
/// @brief Python PhaseShifter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "PyPhaseShifter.h"
#include "PyBitVector.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct PhaseShifterObject
{
  PyObject_HEAD
  PhaseShifter* mPtr;
};

// Python 用のタイプ定義
PyTypeObject PhaseShifterType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
PhaseShifter_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "bitlen",
    "input_config",
    nullptr
  };
  SizeType bitlen = 0;
  PyObject* input_config_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "iO",
				    const_cast<char**>(kwlist),
				    &bitlen, &input_config_obj) ) {
    return nullptr;
  }
  vector<vector<SizeType>> input_config;
  if ( !PySequence_Check(input_config_obj) ) {
    PyErr_SetString(PyExc_TypeError,
		    "2nd parameter must be a sequence of sequences of int");
    return nullptr;
  }
  SizeType n = PySequence_Size(input_config_obj);
  input_config.resize(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto obj1 = PySequence_GetItem(input_config_obj, i);
    if ( !PySequence_Check(obj1) ) {
      Py_DECREF(obj1);
      PyErr_SetString(PyExc_TypeError,
		      "2nd parameter must be a sequence of sequences of int");
      return nullptr;
    }
    SizeType n1 = PySequence_Size(obj1);
    input_config[i].reserve(n1);
    for ( SizeType j = 0; j < n1; ++ j ) {
      auto obj2 = PySequence_GetItem(obj1, j);
      if ( !PyLong_Check(obj2) ) {
	Py_DECREF(obj2);
	Py_DECREF(obj1);
	PyErr_SetString(PyExc_TypeError,
			"2nd parameter must be a sequence of sequences of int");
	return nullptr;
      }
      SizeType val = PyLong_AsLong(obj2);
      input_config[i].push_back(val);
      Py_DECREF(obj2);
    }
    Py_DECREF(obj1);
  }

  auto obj = type->tp_alloc(type, 0);
  auto ps_obj = reinterpret_cast<PhaseShifterObject*>(obj);
  ps_obj->mPtr = new PhaseShifter{bitlen, input_config};
  return obj;
}

// 終了関数
void
PhaseShifter_dealloc(
  PyObject* self
)
{
  auto ps_obj = reinterpret_cast<PhaseShifterObject*>(self);
  delete ps_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

// str() 関数
PyObject*
PhaseShifter_str(
  PyObject* self
)
{
  auto& ps = PyPhaseShifter::Get(self);
  ostringstream buf;
  SizeType n = ps.input_num();
  for ( SizeType i = 0; i < n; ++ i ) {
    auto& ic = ps.input_config(i);
    buf << "[";
    const char* comma = "";
    for ( auto p: ic ) {
      buf << comma << p;
      comma = ", ";
    }
    buf << "]";
  }
  auto tmp_str = buf.str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
PhaseShifter_convert(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "bits",
    nullptr
  };

  PyObject* bits_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
				    const_cast<char**>(kwlist),
				    PyBitVector::_typeobject(), &bits_obj) ) {
    return nullptr;
  }
  auto& bits = PyBitVector::Get(bits_obj);
  auto& ps = PyPhaseShifter::Get(self);
  auto ans = ps.convert(bits);
  return PyBitVector::ToPyObject(ans);
}

PyObject*
PhaseShifter_input_config(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto& ps = PyPhaseShifter::Get(self);
  if ( pos >= ps.input_num() ) {
    PyErr_SetString(PyExc_ValueError, "pos is out of range");
    return nullptr;
  }
  auto& input_config = ps.input_config(pos);
  SizeType n = input_config.size();
  PyObject* ans_obj = PyList_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    SizeType j = input_config[i];
    auto obj1 = PyLong_FromLong(j);
    PyList_SetItem(ans_obj, i, obj1);
  }
  return ans_obj;
}

// メソッド定義
PyMethodDef PhaseShifter_methods[] = {
  {"convert", reinterpret_cast<PyCFunction>(PhaseShifter_convert),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("convert bitvector")},
  {"input_config", PhaseShifter_input_config, METH_VARARGS,
   PyDoc_STR("returns an input configuration")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
PhaseShifter_input_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& ps = PyPhaseShifter::Get(self);
  return Py_BuildValue("k", ps.input_num());
}

PyGetSetDef PhaseShifter_getset[] = {
  {"input_num", PhaseShifter_input_num, nullptr,
   PyDoc_STR("input num"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief 'PhaseShifter' オブジェクトを使用可能にする．
bool
PyPhaseShifter::init(
  PyObject* m
)
{
  PhaseShifterType.tp_name = "PhaseShifter";
  PhaseShifterType.tp_basicsize = sizeof(PhaseShifterObject);
  PhaseShifterType.tp_itemsize = 0;
  PhaseShifterType.tp_dealloc = PhaseShifter_dealloc;
  PhaseShifterType.tp_flags = Py_TPFLAGS_DEFAULT;
  PhaseShifterType.tp_doc = PyDoc_STR("PhaseShifter object");
  PhaseShifterType.tp_methods = PhaseShifter_methods;
  PhaseShifterType.tp_getset = PhaseShifter_getset;
  PhaseShifterType.tp_new = PhaseShifter_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "PhaseShifter", &PhaseShifterType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PhaseShifter を PyObject に変換する．
PyObject*
PyPhaseShifter::ToPyObject(
  const PhaseShifter& val
)
{
  auto obj = PhaseShifterType.tp_alloc(&PhaseShifterType, 0);
  auto ps_obj = reinterpret_cast<PhaseShifterObject*>(obj);
  (*ps_obj->mPtr) = val;
  return obj;
}

// @brief PyObject が PhaseShifter タイプか調べる．
bool
PyPhaseShifter::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief PhaseShifter を表す PyObject から PhaseShifter を取り出す．
const PhaseShifter&
PyPhaseShifter::Get(
  PyObject* obj
)
{
  auto ps_obj = reinterpret_cast<PhaseShifterObject*>(obj);
  return *ps_obj->mPtr;
}

// @brief PhaseShifter を表すオブジェクトの型定義を返す．
PyTypeObject*
PyPhaseShifter::_typeobject()
{
  return &PhaseShifterType;
}

END_NAMESPACE_DRUID
