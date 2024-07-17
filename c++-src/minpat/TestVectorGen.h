#ifndef TESTVECTORGEN_H
#define TESTVECTORGEN_H

/// @file TestVectorGen.h
/// @brief TestVectorGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BaseEnc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TestVectorGen TestVectorGen.h "TestVectorGen.h"
/// @brief FaultGroupGen の結果からテストベクタを作るクラス
//////////////////////////////////////////////////////////////////////
class TestVectorGen
{
public:

  /// @brief コンストラクタ
  TestVectorGen(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const JsonValue& option    ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  ~TestVectorGen() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 値割り当てからテストベクタを作る．
  TestVector
  generate(
    const NodeTimeValList& assignments ///< [in] 値割り当て
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 基本エンコーダ
  BaseEnc mBaseEnc;

};

END_NAMESPACE_DRUID

#endif // TESTVECTORGEN_H
