#ifndef FFRFAULTLIST_H
#define FFRFAULTLIST_H

/// @file FFRFaultList.h
/// @brief FFRFaultList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFRFaultList FFRFaultList.h "FFRFaultList.h"
/// @brief FFR ごとの故障リストを持つクラス
//////////////////////////////////////////////////////////////////////
class FFRFaultList
{
public:

  /// @brief コンストラクタ
  FFRFaultList(
    const TpgNetwork& network,                ///< [in] ネットワーク
    const vector<const TpgFault*>& fault_list ///< [in] 故障リスト
  );

  /// @brief デストラクタ
  ~FFRFaultList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を持つ FFR のリスト
  const vector<const TpgFFR*>&
  ffr_list() const
  {
    return mFFRList;
  }

  /// @brief FFR 内の故障リスト
  const vector<const TpgFault*>&
  fault_list(
    const TpgFFR* ffr ///< [in] FFR
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFR のリスト
  vector<const TpgFFR*> mFFRList;

  // FFR ごとの故障リストの辞書
  // キーは FFR 番号
  unordered_map<SizeType, vector<const TpgFault*>> mFaultListMap;

};

END_NAMESPACE_DRUID

#endif // FFRFAULTLIST_H
