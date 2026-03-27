#ifndef MPCOMP_MERGE_H
#define MPCOMP_MERGE_H

/// @file MpComp_Merge.h
/// @brief MpComp_Merge のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpCompImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpComp_Merge MpComp_Merge.h "MpComp_Merge.h"
/// @brief 故障を他のパタンにマージさせることでパタンの削減を行う．
//////////////////////////////////////////////////////////////////////
class MpComp_Merge :
  public MpCompImpl
{
public:

  /// @brief コンストラクタ
  MpComp_Merge() = default;

  /// @brief デストラクタ
  ~MpComp_Merge() = default;


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

#endif // MPCOMP_MERGE_H
