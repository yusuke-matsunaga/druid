#ifndef MPCOMP_PACK_H
#define MPCOMP_PACK_H

/// @file MpComp_Pack.h
/// @brief MpComp_Pack のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpCompImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpComp_Pack MpComp_Pack.h "MpComp_Pack.h"
/// @brief 最小彩色を用いたパタン圧縮クラス
//////////////////////////////////////////////////////////////////////
class MpComp_Pack :
  public MpCompImpl
{
public:

  /// @brief コンストラクタ
  MpComp_Pack();

  /// @brief デストラクタ
  ~MpComp_Pack();


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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////


};

END_NAMESPACE_DRUID

#endif // MPCOMP_PACK_H
