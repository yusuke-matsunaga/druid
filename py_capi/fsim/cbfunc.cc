
/// @file cbfunc.cc
/// @brief cbfunc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

// Fsim::cbtype1 から Python オブジェクトを呼び出すファンクタクラス
class CbFunc1
{
public:

  // コンストラクタ
  CbFunc1(
    PyObject* cb_obj // 呼び出される Python オブジェクト
  ) : mCbObj{cb_obj}
  {
  }

  // デストラクタ
  ~CbFunc1() = default;

  // コールバック本体
  void
  operator()(
    const TpgFault& fault,
    const DiffBits& dbits
  )
  {
    auto fault_obj = PyTpgFault::ToPyObject(fault);
    auto dbits_obj = PyDiffBits::ToPyObject(dbits);
    auto args = Py_BuildValue("(OO)", fault_obj, dbits_obj);
    auto ret_obj = PyObject_Call(mCbObj, args, nullptr);
    // 結果は無視
    Py_XDECREF(ret_obj);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // Python オブジェクト
  PyObject* mCbObj{nullptr};

};


// Fsim::cbtype2 から Python オブジェクトを呼び出すファンクタクラス
class CbFunc2
{
public:

  // コンストラクタ
  CbFunc2(
    PyObject* cb_obj // 呼び出される Python オブジェクト
  ) : mCbObj{cb_obj}
  {
  }

  // デストラクタ
  ~CbFunc2() = default;

  // コールバック本体
  void
  operator()(
    const TpgFault& fault,
    const DiffBitsArray& dbits_array
  )
  {
    auto fault_obj = PyTpgFault::ToPyObject(fault);
    auto dbits_array_obj = PyDiffBitsArray::ToPyObject(dbits_array);
    auto args = Py_BuildValue("(OO)", fault_obj, dbits_array_obj);
    auto ret_obj = PyObject_Call(mCbObj, args, nullptr);
    // 結果は無視
    Py_XDECREF(ret_obj);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // Python オブジェクト
  PyObject* mCbObj{nullptr};

};
