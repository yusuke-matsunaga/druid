#ifndef DTPGDRIVER_FFRENC_H
#define DTPGDRIVER_FFRENC_H

/// @file DtpgDriver_FFREnc.h
/// @brief DtpgDriver_FFREnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_Enc.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver_FFREnc DtpgDriver_FFREnc.h "DtpgDriver_FFREnc.h"
/// @brief FFR 単位で処理を行う DtpgDriver_Enc
//////////////////////////////////////////////////////////////////////
class DtpgDriver_FFREnc :
  public DtpgDriver_Enc
{
public:

  /// @brief コンストラクタ
  DtpgDriver_FFREnc(
    const TpgFFR& ffr,	       ///< [in] 故障伝搬の起点となる FFR
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DtpgDriver_FFREnc();


private:
  //////////////////////////////////////////////////////////////////////
  // DtpgEnc の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の伝搬条件を得る．
  AssignList
  fault_prop_condition(
    const TpgFault& fault ///< [in] 対象の故障
  ) override;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVER_FFRENC_H
