#ifndef PYCLASSIFIER_H
#define PYCLASSIFIER_H

/// @file PyClassifier.h
/// @brief PyClassifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "Classifier.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PyClassifier PyClassifier.h "PyClassifier.h"
/// @brief Python 用の Classifier 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyClassifier
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

  /// @brief PyObject が Classifier タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief Classifier を表す PyObject から Classifier を取り出す．
  /// @return Classifier を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  Classifier*
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief Classifier を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_DRUID

#endif // PYCLASSIFIER_H
