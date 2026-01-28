
/// @file PyStructEngine.cc
/// @brief PyStructEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyStructEngine.h"
#include "pym/PyTpgNetwork.h"
#include "pym/PyTpgNode.h"
#include "pym/PyTpgNodeList.h"
#include "pym/PyAssign.h"
#include "pym/PyAssignList.h"
#include "pym/PySatBool3.h"
#include "pym/PySatLiteral.h"
#include "pym/PySatLiteralList.h"
#include "pym/PySatStats.h"
#include "pym/PyExpr.h"
#include "pym/PyList.h"
#include "pym/PyFloat.h"
#include "pym/PyInt.h"
#include "pym/PyJsonValue.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct StructEngine_Object
{
  PyObject_HEAD
  StructEngine mVal;
};

// Python 用のタイプ定義
PyTypeObject StructEngine_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyStructEngine::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<StructEngine_Object*>(self);
  obj->mVal.~StructEngine();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
add_cur_node(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "node",
    nullptr
  };
  PyObject* node_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyTpgNode::_typeobject(), &node_obj) ) {
    return nullptr;
  }
  TpgNode node;
  if ( node_obj != nullptr ) {
    if ( !PyTpgNode::FromPyObject(node_obj, node) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TpgNode");
      return nullptr;
    }
  }
  auto& val = PyStructEngine::_get_ref(self);
  try {
    val.add_cur_node(node);
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
add_cur_node_list(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "node_list",
    nullptr
  };
  PyObject* node_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyTpgNodeList::_typeobject(), &node_list_obj) ) {
    return nullptr;
  }
  auto& node_list = PyTpgNodeList::_get_ref(node_list_obj);
  auto& val = PyStructEngine::_get_ref(self);
  try {
    val.add_cur_node_list(node_list);
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
add_prev_node(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "node",
    nullptr
  };
  PyObject* node_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyTpgNode::_typeobject(), &node_obj) ) {
    return nullptr;
  }
  TpgNode node;
  if ( node_obj != nullptr ) {
    if ( !PyTpgNode::FromPyObject(node_obj, node) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TpgNode");
      return nullptr;
    }
  }
  auto& val = PyStructEngine::_get_ref(self);
  try {
    val.add_prev_node(node);
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
add_prev_node_list(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "node_list",
    nullptr
  };
  PyObject* node_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyTpgNodeList::_typeobject(), &node_list_obj) ) {
    return nullptr;
  }
  auto& node_list = PyTpgNodeList::_get_ref(node_list_obj);
  auto& val = PyStructEngine::_get_ref(self);
  try {
    val.add_prev_node_list(node_list);
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
update(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyStructEngine::_get_ref(self);
  try {
    val.update();
    Py_RETURN_NONE;
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
justify(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "assign_list",
    nullptr
  };
  PyObject* assign_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyAssignList::_typeobject(), &assign_list_obj) ) {
    return nullptr;
  }
  AssignList assign_list;
  if ( assign_list_obj != nullptr ) {
    if ( !PyAssignList::FromPyObject(assign_list_obj, assign_list) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to AssignList");
      return nullptr;
    }
  }
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PyAssignList::ToPyObject(val.justify(assign_list));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_pi_assign(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PyAssignList::ToPyObject(val.get_pi_assign());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
conv_to_ltieral(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "assign",
    nullptr
  };
  PyObject* assign_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyAssign::_typeobject(), &assign_obj) ) {
    return nullptr;
  }
  Assign assign;
  if ( assign_obj != nullptr ) {
    if ( !PyAssign::FromPyObject(assign_obj, assign) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to Assign");
      return nullptr;
    }
  }
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PySatLiteral::ToPyObject(val.conv_to_literal(assign));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
conv_to_literal_list(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "assign_list",
    nullptr
  };
  PyObject* assign_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyAssignList::_typeobject(), &assign_list_obj) ) {
    return nullptr;
  }
  AssignList assign_list;
  if ( assign_list_obj != nullptr ) {
    if ( !PyAssignList::FromPyObject(assign_list_obj, assign_list) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to AssignList");
      return nullptr;
    }
  }
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PyList<SatLiteral, PySatLiteral>::ToPyObject(val.conv_to_literal_list(assign_list));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
expr_to_cnf(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "expr",
    nullptr
  };
  PyObject* expr_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyExpr::_typeobject(), &expr_obj) ) {
    return nullptr;
  }
  Expr expr;
  if ( expr_obj != nullptr ) {
    if ( !PyExpr::FromPyObject(expr_obj, expr) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to Expr");
      return nullptr;
    }
  }
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PyList<SatLiteral, PySatLiteral>::ToPyObject(val.expr_to_cnf(expr));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
solve(
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
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|$O",
                                    const_cast<char**>(kwlist),
                                    &assumptions_obj) ) {
    return nullptr;
  }
  std::vector<SatLiteral> assumptions;
  if ( assumptions_obj != nullptr ) {
    if ( !PySatLiteralList::FromPyObject(assumptions_obj, assumptions) ) {
      PyErr_SetString(PyExc_ValueError, "could not convert to std::vector<SatLiteral>");
      return nullptr;
    }
  }
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PySatBool3::ToPyObject(val.solve(assumptions));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
gvar(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "node",
    nullptr
  };
  PyObject* node_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyTpgNode::_typeobject(), &node_obj) ) {
    return nullptr;
  }
  TpgNode node;
  if ( node_obj != nullptr ) {
    if ( !PyTpgNode::FromPyObject(node_obj, node) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TpgNode");
      return nullptr;
    }
  }
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PySatLiteral::ToPyObject(val.gvar(node));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
hvar(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "node",
    nullptr
  };
  PyObject* node_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PyTpgNode::_typeobject(), &node_obj) ) {
    return nullptr;
  }
  TpgNode node;
  if ( node_obj != nullptr ) {
    if ( !PyTpgNode::FromPyObject(node_obj, node) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TpgNode");
      return nullptr;
    }
  }
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PySatLiteral::ToPyObject(val.hvar(node));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
val(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "node",
    "time",
    nullptr
  };
  PyObject* node_obj = nullptr;
  int time;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!i",
                                    const_cast<char**>(kwlist),
                                    PyTpgNode::_typeobject(), &node_obj,
                                    &time) ) {
    return nullptr;
  }
  TpgNode node;
  if ( node_obj != nullptr ) {
    if ( !PyTpgNode::FromPyObject(node_obj, node) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to TpgNode");
      return nullptr;
    }
  }
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PyBool_FromLong(val.val(node, time));
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
cnf_time(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PyFloat::ToPyObject(val.cnf_time());
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
  {"add_cur_node",
   reinterpret_cast<PyCFunction>(add_cur_node),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add node in the current time")},
  {"add_cur_node_list",
   reinterpret_cast<PyCFunction>(add_cur_node_list),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add nodes in the current time")},
  {"add_prev_node",
   reinterpret_cast<PyCFunction>(add_prev_node),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add node in the previous time")},
  {"add_prev_node_list",
   reinterpret_cast<PyCFunction>(add_prev_node_list),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add nodes in the previous time")},
  {"update",
   update,
   METH_NOARGS,
   PyDoc_STR("udpate")},
  {"justify",
   reinterpret_cast<PyCFunction>(justify),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("do Justification")},
  {"get_pi_assign",
   get_pi_assign,
   METH_NOARGS,
   PyDoc_STR("return current assignments on Primary Inputs")},
  {"conv_to_ltieral",
   reinterpret_cast<PyCFunction>(conv_to_ltieral),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return SAT literal related to the assignment")},
  {"conv_to_literal_list",
   reinterpret_cast<PyCFunction>(conv_to_literal_list),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return SAT literals related to the assignments")},
  {"expr_to_cnf",
   reinterpret_cast<PyCFunction>(expr_to_cnf),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("make CNF related to the expression")},
  {"solve",
   reinterpret_cast<PyCFunction>(solve),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("solve the problem")},
  {"gvar",
   reinterpret_cast<PyCFunction>(gvar),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return G Variable for the node")},
  {"hvar",
   reinterpret_cast<PyCFunction>(hvar),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return H Variable for the node")},
  {"val",
   reinterpret_cast<PyCFunction>(val),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("return the value of the node")},
  {"cnf_time",
   cnf_time,
   METH_NOARGS,
   PyDoc_STR("return CPU time for CNF gneration")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
get_cur_node_list(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PyTpgNodeList::ToPyObject(val.cur_node_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_prev_node_list(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PyTpgNodeList::ToPyObject(val.prev_node_list());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_stats(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyStructEngine::_get_ref(self);
  try {
    return PySatStats::ToPyObject(val.get_stats());
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
  {"cur_node_list", get_cur_node_list, nullptr, PyDoc_STR("node list related to the current time"), nullptr},
  {"prev_node_list", get_prev_node_list, nullptr, PyDoc_STR("node list related to the previous time"), nullptr},
  {"stats", get_stats, nullptr, PyDoc_STR("SAT statistics"), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
};

// new 関数
PyObject*
new_func(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "network",
    "option",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!|$O",
                                    const_cast<char**>(kwlist),
                                    PyTpgNetwork::_typeobject(), &network_obj,
                                    &option_obj) ) {
    return nullptr;
  }
  auto& network = PyTpgNetwork::_get_ref(network_obj);
  JsonValue option;
  if ( option_obj != nullptr ) {
    if ( !PyJsonValue::FromPyObject(option_obj, option) ) {
      PyErr_SetString(PyExc_ValueError, "could not convert to JsonValue");
      return nullptr;
    }
  }
  try {
    auto self = type->tp_alloc(type, 0);
    auto my_obj = reinterpret_cast<StructEngine_Object*>(self);
    new (&my_obj->mVal) StructEngine(network, option);
    return self;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
  catch ( std::out_of_range err ) {
    std::ostringstream buf;
    buf << "out of range" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

END_NONAMESPACE


// @brief StructEngine オブジェクトを使用可能にする．
bool
PyStructEngine::init(
  PyObject* m
)
{
  StructEngine_Type.tp_name = "StructEngine";
  StructEngine_Type.tp_basicsize = sizeof(StructEngine_Object);
  StructEngine_Type.tp_itemsize = 0;
  StructEngine_Type.tp_dealloc = dealloc_func;
  StructEngine_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  StructEngine_Type.tp_doc = PyDoc_STR("Python extended object for StructEngine");
  StructEngine_Type.tp_methods = methods;
  StructEngine_Type.tp_getset = getsets;
  StructEngine_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "StructEngine", &StructEngine_Type) ) {
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
  return Py_IS_TYPE(obj, &StructEngine_Type);
}

// @brief PyObject から StructEngine を取り出す．
StructEngine&
PyStructEngine::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<StructEngine_Object*>(obj);
  return my_obj->mVal;
}

// @brief StructEngine を表すオブジェクトの型定義を返す．
PyTypeObject*
PyStructEngine::_typeobject()
{
  return &StructEngine_Type;
}

END_NAMESPACE_DRUID
