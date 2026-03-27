#ifndef MPCOMP_MINCOV_H
#define MPCOMP_MINCOV_H

/// @file MpComp_MinCov.h
/// @brief MpComp_MinCov のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpCompImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpComp_MinCov MpComp_MinCov.h "MpComp_MinCov.h"
/// @brief 最小被覆問題を用いたパタン圧縮クラス
//////////////////////////////////////////////////////////////////////
class MpComp_MinCov :
  public MpCompImpl
{
public:

  /// @brief コンストラクタ
  MpComp_MinCov();

  /// @brief デストラクタ
  ~MpComp_MinCov();


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

#endif // MPCOMP_MINCOV_H
