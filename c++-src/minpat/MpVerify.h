#ifndef MPVERIFY_H
#define MPVERIFY_H

/// @file MpVerify.h
/// @brief MpVerify のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpVerify MpVerify.h "MpVerify.h"
/// @brief 結果の検証を行うクラス
//////////////////////////////////////////////////////////////////////
class MpVerify
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief tv_list が fault_list の全ての故障を検出するか調べる．
  /// @return 未検出の故障のリストを返す．
  static
  TpgFaultList
  run(
    const std::vector<TestVector>& tv_list, ///< [in] テストベクタのリスト
    const TpgFaultList& fault_list,         ///< [in] 対象の故障リスト
    const ConfigParam& option = {}          ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // MPVERIFY_H
