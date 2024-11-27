#ifndef COVER2BDD_H
#define COVER2BDD_H

/// @file Cover2Bdd.h
/// @brief Cover2Bdd のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TestCover.h"
#include "ym/Bdd.h"
#include "ym/BddVar.h"
#include "ym/BddMgr.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Cover2Bdd Cover2Bdd.h "Cover2Bdd.h"
/// @brief カバーを BDD に変換するクラス
///
/// オプションパラメータ
/// - "debug":          int   デバッグレベル
//////////////////////////////////////////////////////////////////////
class Cover2Bdd
{
public:

  /// @brief コンストラクタ
  Cover2Bdd(
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );

  /// @brief デストラクタ
  ~Cover2Bdd() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief TestCover に対応するBDDを返す．
  Bdd
  make_bdd(
    BddMgr& mgr,
    const TestCover& cover
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // デバッグフラグ
  int mDebug{0};

};

END_NAMESPACE_DRUID

#endif // COVER2BDD_H
