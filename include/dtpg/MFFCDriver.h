#ifndef MFFCDRIVER_H
#define MFFCDRIVER_H

/// @file MFFCDriver.h
/// @brief MFFCDriver のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "DtpgDriver.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MFFCDriver MFFCDriver.h "MFFCDriver.h"
/// @brief MFFC 単位で DTPG の基本的な処理を行うクラス
///
/// 各故障のMFFC内の故障伝搬条件はその故障が属しているFFRの故障伝搬条件
/// とそのFFRの故障伝搬条件の積で表される．
/// そのため，MFFCの根から外部出力までの故障伝搬条件と各FFRの故障伝搬条件
/// のみを SATソルバの条件節として作っておく．
//////////////////////////////////////////////////////////////////////
class MFFCDriver :
  public DtpgDriver
{
public:

  /// @brief コンストラクタ
  MFFCDriver(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgMFFC* ffr,	       ///< [in] 故障伝搬の起点となる MFFC
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~MFFCDriver() = default;

};

END_NAMESPACE_DRUID

#endif // MFFCDRIVER_H
