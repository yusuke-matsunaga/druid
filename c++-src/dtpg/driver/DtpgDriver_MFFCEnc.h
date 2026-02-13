#ifndef DTPGDRIVER_MFFCENC_H
#define DTPGDRIVER_MFFCENC_H

/// @file DtpgDriver_MFFCEnc.h
/// @brief DtpgDriver_MFFCEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_Enc.h"
#include "dtpg/MFFCEnc.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver_MFFCEnc DtpgDriver_MFFCEnc.h "DtpgDriver_MFFCEnc.h"
/// @brief MFFC 単位で処理をおこなう DtpgDriver_Enc
//////////////////////////////////////////////////////////////////////
class DtpgDriver_MFFCEnc :
  public DtpgDriver_Enc
{
public:

  /// @brief コンストラクタ
  DtpgDriver_MFFCEnc(
    const TpgMFFC& mffc,       ///< [in] 故障伝搬の起点となる MFFC
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DtpgDriver_MFFCEnc() = default;


private:
  //////////////////////////////////////////////////////////////////////
  // DtpgEnc の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の伝搬条件を得る．
  AssignList
  fault_prop_condition(
    const TpgFault& fault ///< [in] 対象の故障
  ) override;

  /// @brief 追加の条件を加える．
  void
  add_extra_assumptions(
    const TpgFault& fault,               ///< [in] 対象の故障
    std::vector<SatLiteral>& assumptions ///< [inout] 追加する対象のリスト
  ) override;

  /// @brief 追加の割り当てを加える．
  void
  add_extra_assignments(
    const TpgFault& fault,  ///< [in] 対象の故障
    AssignList& assign_list ///< [inout] 追加する対象のリスト
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // MFFCEnc
  MFFCEnc* mMFFCEnc;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVER_MFFCENC_H
