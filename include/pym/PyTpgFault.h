#ifndef PYTPGFAULT_H
#define PYTPGFAULT_H

/// @file PyTpgFault.h
/// @brief PyTpgFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyTpgFault PyTpgFault.h "PyTpgFault.h"
/// @brief Python 用の TpgFault 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyTpgFault
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  /// @return 初期化が成功したら true を返す．
  static
  bool
  init(
    PyObject* m ///< [in] 親のモジュールを表す PyObject
  );

  /// @brief TpgFault を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const TpgFault* val ///< [in] 値
  );

  /// @brief TpgFault のリストを表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyList(
    const vector<const TpgFault*>& val_list ///< [in] 値のリスト
  );

  /// @brief PyObject が TpgFault タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief TpgFault を表す PyObject から TpgFault を取り出す．
  /// @return TpgFault を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  const TpgFault*
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief TpgFault のリストを表す PyObject から TpgFault のリストを取り出す．
  /// @return 成功したら true を返す．
  ///
  /// obj は単一の TpgFault か TpgFault のシーケンスタイプである必要がある．
  static
  bool
  FromPyList(
    PyObject* obj,                      ///< [in] 変換元の PyObject
    vector<const TpgFault*>& fault_list ///< [out] 変換結果を格納する変数
  );

  /// @brief TpgFault を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYTPGFAULT_H
