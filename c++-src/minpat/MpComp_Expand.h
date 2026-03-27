#ifndef MPCOMP_EXPAND_H
#define MPCOMP_EXPAND_H

/// @file MpComp_Expand.h
/// @brief MpComp_Expand のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpCompImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpComp_Expand MpComp_Expand.h "MpComp_Expand.h"
/// @brief 検出状態を変えずにテストキューブの拡大を行うクラス
//////////////////////////////////////////////////////////////////////
class MpComp_Expand :
  public MpCompImpl
{
public:

  /// @brief コンストラクタ
  MpComp_Expand();

  /// @brief デストラクタ
  ~MpComp_Expand();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief パタン圧縮の本体
  std::vector<TestVector>
  run(
    const std::vector<TestVector>& tv_list, ///< [in] 初期パタンリスト
    const TpgFaultList& fault_list,         ///< [in] 対象の故障リスト
    const ConfigParam& option               ///< [in] オプション
  ) override;

};

END_NAMESPACE_DRUID

#endif // MPCOMP_EXPAND_H
