
/// @file PyAssignExpr.cc
/// @brief Python AssignExpr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyAssignExpr.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct AssignExprObject
{
  PyObject_HEAD
  AssignExpr* mVal;
};

// Python 用のタイプ定義
PyTypeObject AssignExprType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
AssignExpr_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'AssignExpr' is disabled.");
  return nullptr;
}

// 終了関数
void
AssignExpr_dealloc(
  PyObject* self
)
{
  auto testcond_obj = reinterpret_cast<AssignExprObject*>(self);
  delete testcond_obj->mVal;
  Py_TYPE(self)->tp_free(self);
}

// メソッド定義
PyMethodDef AssignExpr_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief 'AssignExpr' オブジェクトを使用可能にする．
bool
PyAssignExpr::init(
  PyObject* m
)
{
  AssignExprType.tp_name = "AssignExpr";
  AssignExprType.tp_basicsize = sizeof(AssignExprObject);
  AssignExprType.tp_itemsize = 0;
  AssignExprType.tp_dealloc = AssignExpr_dealloc;
  AssignExprType.tp_flags = Py_TPFLAGS_DEFAULT;
  AssignExprType.tp_doc = PyDoc_STR("AssignExpr object");
  AssignExprType.tp_methods = AssignExpr_methods;
  AssignExprType.tp_new = AssignExpr_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "AssignExpr", &AssignExprType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief AssignExpr を PyObject に変換する．
PyObject*
PyAssignExpr::ToPyObject(
  const AssignExpr& val
)
{
  auto obj = AssignExprType.tp_alloc(&AssignExprType, 0);
  auto testcond_obj = reinterpret_cast<AssignExprObject*>(obj);
  testcond_obj->mVal = new AssignExpr{val};
  return obj;
}

// @brief PyObject が AssignExpr タイプか調べる．
bool
PyAssignExpr::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief AssignExpr を表す PyObject から AssignExpr を取り出す．
AssignExpr
PyAssignExpr::Get(
  PyObject* obj
)
{
  auto testcond_obj = reinterpret_cast<AssignExprObject*>(obj);
  return *testcond_obj->mVal;
}

// @brief AssignExpr を表す PyObject から AssignExpr を取り出す．
const AssignExpr&
PyAssignExpr::_Get(
  PyObject* obj
)
{
  auto testcond_obj = reinterpret_cast<AssignExprObject*>(obj);
  return *testcond_obj->mVal;
}

// @brief AssignExpr を表すオブジェクトの型定義を返す．
PyTypeObject*
PyAssignExpr::_typeobject()
{
  return &AssignExprType;
}

END_NAMESPACE_DRUID
