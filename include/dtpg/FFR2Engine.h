#ifndef FFR2ENGINE_H
#define FFR2ENGINE_H

/// @file FFR2Engine.h
/// @brief FFR2Engine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/StructEngine.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

class FFREnc;

//////////////////////////////////////////////////////////////////////
/// @class FFR2Engine FFR2Engine.h "FFR2Engine.h"
/// @brief 2つのFFRの故障伝搬を調べる StructEngine
/// @ingroup DtpgGroup
//////////////////////////////////////////////////////////////////////
class FFR2Engine:
  public StructEngine
{
public:

  /// @brief コンストラクタ
  FFR2Engine(
    const TpgFFR& ffr1,              ///< [in] FFR1
    const TpgFFR& ffr2,              ///< [in] FFR2
    const TpgFaultList& fault_list1, ///< [in] FFR1 の故障のリスト
    const TpgFaultList& fault_list2, ///< [in] FFR2 の故障のリスト
    const ConfigParam& option = {}   ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FFR2Engine() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 2つの故障の検出条件を調べる．
  SatBool3
  solve(
    const TpgFault& fault1, ///< [in] fault_list1 の故障
    const TpgFault& fault2, ///< [in] fault_list2 の故障
    bool det1,              ///< [in] fault1 を検出する時 true にするフラグ
    bool det2               ///< [in] fault2 を検出する時 true にするフラグ
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  FFREnc* mEnc1;
  FFREnc* mEnc2;

};

END_NAMESPACE_DRUID

#endif // FFR2ENGINE_H
