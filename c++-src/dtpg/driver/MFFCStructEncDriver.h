#ifndef MFFCSTRUCTENCDRIVER_H
#define MFFCSTRUCTENCDRIVER_H

/// @file MFFCStructEncDriver.h
/// @brief MFFCStructEncDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriverImpl.h"
#include "StructEnc.h"
#include "Justifier.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MFFCStructEncDriver MFFCStructEncDriver.h "MFFCStructEncDriver.h"
/// @brief MFFCStructEnc を用いたドライバ
//////////////////////////////////////////////////////////////////////
class MFFCStructEncDriver :
  public DtpgDriverImpl
{
public:

  /// @brief コンストラクタ
  MFFCStructEncDriver(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgMFFC* mffc,       ///< [in] 故障伝搬の起点となる MFFC
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~MFFCStructEncDriver();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を検出する条件を求める．
  ///
  /// - f はコンストラクタで指定した MFFC 内の故障でなければならない．
  SatBool3
  solve(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

  /// @brief テストパタン生成を行う．
  TestVector
  gen_pattern(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // エンジン本体
  StructEnc mStructEnc;

  // MFFC の根のノード
  const TpgNode* mRoot;

  // justifier
  Justifier mJustifier;

};

END_NAMESPACE_DRUID

#endif // MFFCSTRUCTENCDRIVER_H
