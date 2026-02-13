#ifndef DTPGDRIVER_NODEENC_H
#define DTPGDRIVER_NODEENC_H

/// @file DtpgDriver_NodeEnc.h
/// @brief DtpgDriver_NodeEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_Enc.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver_NodeEnc DtpgDriver_NodeEnc.h "DtpgDriver_NodeEnc.h"
/// @brief Node 単位で処理を行う DtpgDriver_Enc
//////////////////////////////////////////////////////////////////////
class DtpgDriver_NodeEnc :
  public DtpgDriver_Enc
{
public:

  /// @brief コンストラクタ
  DtpgDriver_NodeEnc(
    const TpgNode& node,       ///< [in] 故障伝搬の起点となるノード
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DtpgDriver_NodeEnc();


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

#endif // DTPGDRIVER_NODEENC_H
