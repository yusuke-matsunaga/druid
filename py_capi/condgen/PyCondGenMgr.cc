
/// @file PyCondGenMgr.cc
/// @brief Python CondGenMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "PyCondGenMgr.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct CondGenMgrObject
{
  PyObject_HEAD
  CondGenMgr mVal;
};

// Python 用のタイプ定義
PyTypeObject CondGenMgrType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
CondGenMgr_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  auto self = type->tp_alloc(type, 0);
  // auto condgenmgr_obj = reinterpret_cast<CondGenMgrObject*>(self);
  // 必要なら condgenmgr_obj->mVal の初期化を行う．
  return self;
}

// 終了関数
void
CondGenMgr_dealloc(
  PyObject* self
)
{
  // auto condgenmgr_obj = reinterpret_cast<CondGenMgrObject*>(self);
  // 必要なら condgenmgr_obj->mVal の終了処理を行う．
  // delete condgenmgr_obj->mVal
  Py_TYPE(self)->tp_free(self);
}

// 初期化関数(__init__()相当)
int
CondGenMgr_init(
  PyObject* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  return 0;
}

// repr() 関数
PyObject*
CondGenMgr_repr(
  PyObject* self
)
{
  auto& val = PyCondGenMgr::Get(self);
  // val から 文字列を作る．
  const char* tmp_str = nullptr;
  return Py_BuildValue("s", tmp_str);
}

// メソッド定義
PyMethodDef CondGenMgr_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// get() 関数の例
PyObject*
CondGenMgr_get(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyCondGenMgr::Get(self);
}

// get/set 関数定義
PyGetSetDef CondGenMgr_getset[] = {
  {"member", CondGenMgr_get, nullptr, PyDoc_STR("member getter"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr},
};

// 比較関数
PyObject*
CondGenMgr_richcmpfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PyCondGenMgr::Check(self) &&
       PyCondGenMgr::Check(other) ) {
    auto& val1 = PyCondGenMgr::Get(self);
    auto& val2 = PyCondGenMgr::Get(other);
    Py_RETURN_RICHCOMPARE(val1, val2, op);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 否定演算(単項演算の例)
PyObject*
CondGenMgr_invert(
  PyObject* self
)
{
  if ( PyCondGenMgr::Check(self) ) {
    auto& val = PyCondGenMgr::Get(self);
    return PyCondGenMgr::ToPyObject(~val);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 乗算(二項演算の例)
PyObject*
CondGenMgr_mul(
  PyObject* self,
  PyObject* other
)
{
  if ( PyCondGenMgr::Check(self) && PyCondGenMgr::Check(other) ) {
    auto& val1 = PyCondGenMgr::Get(self);
    auto& val2 = PyCondGenMgr::Get(other);
    return PyCondGenMgr::ToPyObject(val1 * val2);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 数値演算メソッド定義
PyNumberMethods CondGenMgr_number = {
  .nb_multiply = CondGenMgr_mult,
  .nb_invert = CondGenMgr_invert,
  .nb_inplace_multiply = CondGenMgr_mult
};

// マップオブジェクトのサイズ
Py_ssize_t
CondGenMgr_Size(
  PyObject* self
)
{
  auto& val = PyCondGenMgr::Get(self);
  return val.size();
}

// マップオブジェクトの要素取得関数
PyObject*
CondGenMgr_GetItem(
  PyObject* self,
  PyObject* key
)
{
  return nullptr;
}

// マップオブジェクトの要素設定関数
int
CondGenMgr_SetItem(
  PyObject* self,
  PyObject* key,
  PyObject* v
)
{
  return -1;
}

// マップオブジェクト構造体
PyMappingMethods CondGenMgr_mapping = {
  .mp_length = CondGenMgr_Size,
  .mp_subscript = CondGenMgr_GetItem,
  .mp_ass_subscript = CondGenMgr_SetItem
};

// ハッシュ関数
Py_hash_t
CondGenMgr_hash(
  PyObject* self
)
{
  auto& val = PyCondGenMgr::Get(self);
  return val.hash();
}

END_NONAMESPACE


// @brief 'CondGenMgr' オブジェクトを使用可能にする．
bool
PyCondGenMgr::init(
  PyObject* m
)
{
  CondGenMgrType.tp_name = "CondGenMgr";
  CondGenMgrType.tp_basicsize = sizeof(CondGenMgrObject);
  CondGenMgrType.tp_itemsize = 0;
  CondGenMgrType.tp_dealloc = CondGenMgr_dealloc;
  CondGenMgrType.tp_flags = Py_TPFLAGS_DEFAULT;
  CondGenMgrType.tp_doc = PyDoc_STR("CondGenMgr object");
  CondGenMgrType.tp_richcompare = CondGenMgr_richcmpfunc;
  CondGenMgrType.tp_methods = CondGenMgr_methods;
  CondGenMgrType.tp_getset = CondGenMgr_getset;
  CondGenMgrType.tp_init = CondGenMgr_init;
  CondGenMgrType.tp_new = CondGenMgr_new;
  CondGenMgrType.tp_repr = CondGenMgr_repr;
  CondGenMgrType.tp_as_number = &CondGenMgr_number;
  CondGenMgrType.tp_as_mapping = &CondGenMgr_mapping;
  CondGenMgrType.tp_hash = CondGenMgr_hash;
  if ( PyType_Ready(&CondGenMgrType) < 0 ) {
    return false;
  }

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "CondGenMgr", &CondGenMgrType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief CondGenMgr を PyObject に変換する．
PyObject*
PyCondGenMgr::ToPyObject(
  const CondGenMgr& val
)
{
  auto obj = CondGenMgrType.tp_alloc(&CondGenMgrType, 0);
  auto condgenmgr_obj = reinterpret_cast<CondGenMgrObject*>(obj);
  condgenmgr_obj->mVal = val;
  return obj;
}

// @brief PyObject が CondGenMgr タイプか調べる．
bool
PyCondGenMgr::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief CondGenMgr を表す PyObject から CondGenMgr を取り出す．
CondGenMgr
PyCondGenMgr::Get(
  PyObject* obj
)
{
  auto condgenmgr_obj = reinterpret_cast<CondGenMgrObject*>(obj);
  return condgenmgr_obj->mVal;
}

// @brief CondGenMgr を表す PyObject から CondGenMgr を取り出す．
CondGenMgr&
PyCondGenMgr::Get(
  PyObject* obj
)
{
  auto condgenmgr_obj = reinterpret_cast<CondGenMgrObject*>(obj);
  return condgenmgr_obj->mVal;
}

// @brief CondGenMgr を表すオブジェクトの型定義を返す．
PyTypeObject*
PyCondGenMgr::_typeobject()
{
  return &CondGenMgrType;
}

END_NAMESPACE_DRUID
