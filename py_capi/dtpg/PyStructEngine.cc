
/// @file PyStructEngine.cc
/// @brief Python StructEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyStructEngine.h"
#include "pym/PyTpgNetwork.h"
#include "pym/PySatBool3.h"
#include "pym/PyJsonValue.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct StructEngineObject
{
  PyObject_HEAD
  StructEngine mVal;
};

// Python 用のタイプ定義
PyTypeObject StructEngineType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
StructEngine_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "network",
    "options",
    nullptr
  };

  PyObject* network_obj = nullptr;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!|O",
				    const_cast<char**>(kwlist),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    &option_obj) ) {
    return nullptr;
  }

  auto& network = PyTpgNetwork::_get_ref(network_obj);
  JsonValue option;
  PyJsonValue::Deconv json_dec;
  if ( !json_dec(option_obj, option) ) {
    PyErr_SetString(PyExc_TypeError, "'option' should be a JsonValue type");
    return nullptr;
  }

  auto self = type->tp_alloc(type, 0);
  auto structengine_obj = reinterpret_cast<StructEngineObject*>(self);
  new (&structengine_obj->mVal) StructEngine(network, option);
  return self;
}

// 終了関数
void
StructEngine_dealloc(
  PyObject* self
)
{
  auto structengine_obj = reinterpret_cast<StructEngineObject*>(self);
  (structengine_obj->mVal).~StructEngine();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
StructEngine_add_subenc(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    nullptr
  };

  auto& val = PyStructEngine::_get_ref(self);
  return nullptr;
}

PyObject*
StructEngine_add_cur_node(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    nullptr
  };

  auto& val = PyStructEngine::_get_ref(self);
  return nullptr;
}

PyObject*
StructEngine_add_prev_node(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    nullptr
  };

  auto& val = PyStructEngine::_get_ref(self);
  return nullptr;
}

PyObject*
StructEngine_update(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyStructEngine::_get_ref(self);
  val.update();
  Py_RETURN_NONE;
}

PyObject*
StructEngine_solve(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "assumptions",
    nullptr
  };

  PyObject* assumptions_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O",
				    const_cast<char**>(kwlist),
				    &assumptions_obj) ) {
    return nullptr;
  }

  std::vector<SatLiteral> assumptions;
  // if ( !PySatLiteral::FromPyList(assumptions_obj, assumptions) ) {
  //    return nullptr;
  // }
  auto& val = PyStructEngine::_get_ref(self);
  auto res = val.solve(assumptions);
  return PySatBool3::ToPyObject(res);
}

// メソッド定義
PyMethodDef StructEngine_methods[] = {
  {"add_subenc", reinterpret_cast<PyCFunction>(StructEngine_add_subenc),
   METH_KEYWORDS | METH_VARARGS,
   PyDoc_STR("add SubEnc")},
  {"add_cur_node", reinterpret_cast<PyCFunction>(StructEngine_add_cur_node),
   METH_KEYWORDS | METH_VARARGS,
   PyDoc_STR("add current node")},
  {"add_prev_node", reinterpret_cast<PyCFunction>(StructEngine_add_prev_node),
   METH_KEYWORDS | METH_VARARGS,
   PyDoc_STR("add prev node")},
  {"update", StructEngine_update,
   METH_NOARGS,
   PyDoc_STR("update")},
  {"solve", reinterpret_cast<PyCFunction>(StructEngine_solve),
   METH_KEYWORDS | METH_VARARGS,
   PyDoc_STR("solve")},
  {nullptr, nullptr, 0, nullptr}
};

#if 0
PyObject*
StructEngine_network(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyStructEngine::Get(self);
  auto& network = val.network();
  return PyTpgNetwork
}

// get/set 関数定義
PyGetSetDef StructEngine_getset[] = {
  {"member", StructEngine_get, nullptr, PyDoc_STR("member getter"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr},
};
#endif

END_NONAMESPACE


// @brief 'StructEngine' オブジェクトを使用可能にする．
bool
PyStructEngine::init(
  PyObject* m
)
{
  StructEngineType.tp_name = "StructEngine";
  StructEngineType.tp_basicsize = sizeof(StructEngineObject);
  StructEngineType.tp_itemsize = 0;
  StructEngineType.tp_dealloc = StructEngine_dealloc;
  StructEngineType.tp_flags = Py_TPFLAGS_DEFAULT;
  StructEngineType.tp_doc = PyDoc_STR("StructEngine object");
  StructEngineType.tp_methods = StructEngine_methods;
  //StructEngineType.tp_getset = StructEngine_getset;
  StructEngineType.tp_new = StructEngine_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "StructEngine", &StructEngineType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が StructEngine タイプか調べる．
bool
PyStructEngine::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief StructEngine を表す PyObject から StructEngine を取り出す．
StructEngine&
PyStructEngine::_get_ref(
  PyObject* obj
)
{
  auto structengine_obj = reinterpret_cast<StructEngineObject*>(obj);
  return structengine_obj->mVal;
}

// @brief StructEngine を表すオブジェクトの型定義を返す．
PyTypeObject*
PyStructEngine::_typeobject()
{
  return &StructEngineType;
}

END_NAMESPACE_DRUID
