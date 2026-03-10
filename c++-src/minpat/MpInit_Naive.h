#ifndef MPINIT_NAIVE_H
#define MPINIT_NAIVE_H

/// @file MpInit_Naive.h
/// @brief MpInit_Naive のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpInit.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpInit_Naive MpInit_Naive.h "MpInit_Naive.h"
/// @brief 単純に初期解を作る MpInit
//////////////////////////////////////////////////////////////////////
class MpInit_Naive :
  public MpInit
{
public:

  /// @brief コンストラクタ
  MpInit_Naive(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief デストラクタ
  ~MpInit_Naive() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象の故障リストとテストパタンのリストを求める．
  std::pair<TpgFaultList, std::vector<TestVector>>
  run(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const JsonValue& option = {}    ///< [in] オプション
  ) override;

};

END_NAMESPACE_DRUID

#endif // MPINIT_NAIVE_H
