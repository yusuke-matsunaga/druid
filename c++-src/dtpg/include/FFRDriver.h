#ifndef FFRDRIVER_H
#define FFRDRIVER_H

/// @file FFRDriver.h
/// @brief FFRDriver のヘッダファイル
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
/// @class FFRDriver FFRDriver.h "FFRDriver.h"
/// @brief FFR 単位で DTPG の基本的な処理を行うクラス
///
/// 各故障のFFR内の故障伝搬条件は single literal の積で表されるので
/// SATソルバの節として生成する必要はない．
/// そのため，FFRの根から外部出力までの故障伝搬条件だけを作っておく．
//////////////////////////////////////////////////////////////////////
class FFRDriver :
  public DtpgDriver
{
public:

  /// @brief コンストラクタ
  FFRDriver(
    DtpgMgr& mgr,           ///< [in] DTPGマネージャ
    const TpgFFR* ffr,	    ///< [in] 故障伝搬の起点となる FFR
    const JsonValue& option ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FFRDriver() = default;

};

END_NAMESPACE_DRUID

#endif // FFRDRIVER_H
