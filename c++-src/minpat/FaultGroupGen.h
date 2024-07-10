#ifndef FAULTGROUPGEN_H
#define FAULTGROUPGEN_H

/// @file FaultGroupGen.h
/// @brief FaultGroupGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultInfo.h"
#include "BaseEnc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultGroupGen FaultGroupGen.h "FaultGroupGen.h"
/// @brief 両立故障グループを求めるクラス
//////////////////////////////////////////////////////////////////////
class FaultGroupGen
{
public:

  /// @brief コンストラクタ
  FaultGroupGen(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FaultGroupGen();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 両立故障グループを求める．
  vector<vector<FaultInfo>>
  generate(
    const vector<FaultInfo>& fault_list ///< [in] 故障情報のリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


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

#endif // FAULTGROUPGEN_H
