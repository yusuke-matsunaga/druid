
/// @file PyDetCond.cc
/// @brief Python DetCond の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDetCond.h"
#include "pym/PyDetCondType.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DetCondObject
{
  PyObject_HEAD
  DetCond mVal;
};

// Python 用のタイプ定義
PyTypeObject DetCondType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DetCond_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'DetCond' is disabled.");
  return nullptr;
}

// 終了関数
void
DetCond_dealloc(
  PyObject* self
)
{
  auto testcond_obj = reinterpret_cast<DetCondObject*>(self);
  testcond_obj->mVal.~DetCond();
  Py_TYPE(self)->tp_free(self);
}

// メソッド定義
PyMethodDef DetCond_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
DetCond_type(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyDetCond::Get(self);
  return PyDetCondType::ToPyObject(val.type());
}

// getsetter 定義
PyGetSetDef DetCond_getsetters[] = {
  {"type", DetCond_type, nullptr, PyDoc_STR("type"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief 'DetCond' オブジェクトを使用可能にする．
bool
PyDetCond::init(
  PyObject* m
)
{
  DetCondType.tp_name = "DetCond";
  DetCondType.tp_basicsize = sizeof(DetCondObject);
  DetCondType.tp_itemsize = 0;
  DetCondType.tp_dealloc = DetCond_dealloc;
  DetCondType.tp_flags = Py_TPFLAGS_DEFAULT;
  DetCondType.tp_doc = PyDoc_STR("DetCond object");
  DetCondType.tp_methods = DetCond_methods;
  DetCondType.tp_getset = DetCond_getsetters;
  DetCondType.tp_new = DetCond_new;
  if ( PyType_Ready(&DetCondType) < 0 ) {
    return false;
  }

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "DetCond", &DetCondType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief DetCond を PyObject に変換する．
PyObject*
PyDetCond::ToPyObject(
  const DetCond& val
)
{
  auto obj = DetCondType.tp_alloc(&DetCondType, 0);
  auto testcond_obj = reinterpret_cast<DetCondObject*>(obj);
  new (&testcond_obj->mVal) DetCond{val};
  return obj;
}

// @brief DetCond のリストを PyObject に変換する．
PyObject*
PyDetCond::ToPyList(
  const vector<DetCond>& val_list
)
{
  SizeType n = val_list.size();
  auto ans_obj = PyList_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto cond = val_list[i];
    auto cond_obj = ToPyObject(cond);
    PyList_SET_ITEM(ans_obj, i, cond_obj);
  }
  return ans_obj;
}

// @brief DetCond のリストを表す PyObject から DetCond のリストを取り出す．
bool
PyDetCond::FromPyList(
  PyObject* obj,
  std::vector<DetCond>& cond_list
)
{
  cond_list.clear();
  if ( !PySequence_Check(obj) ) {
    return false;
  }
  auto n = PySequence_Size(obj);
  cond_list.reserve(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto item_obj = PySequence_GetItem(obj, i);
    if ( !Check(item_obj) ) {
      return false;
    }
    auto& cond = Get(item_obj);
    cond_list.push_back(cond);
  }
  return true;
}

// @brief PyObject が DetCond タイプか調べる．
bool
PyDetCond::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DetCond を表す PyObject から DetCond を取り出す．
const DetCond&
PyDetCond::Get(
  PyObject* obj
)
{
  auto testcond_obj = reinterpret_cast<DetCondObject*>(obj);
  return testcond_obj->mVal;
}

// @brief DetCond を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDetCond::_typeobject()
{
  return &DetCondType;
}

END_NAMESPACE_DRUID
