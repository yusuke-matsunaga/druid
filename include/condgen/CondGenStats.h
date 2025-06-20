#ifndef CONDGENSTATS_H
#define CONDGENSTATS_H

/// @file CondGenStats.h
/// @brief CondGenStats のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/CnfSize.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CondGenStats CondGenStats.h "CondGenStats.h"
/// @brief CondGenMgr::make_cnf() の実行時の統計情報を表すクラス
//////////////////////////////////////////////////////////////////////
class CondGenStats
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 内容は不定
  CondGenStats() = default;

  /// @brief 値を指定したコンストラクタ
  CondGenStats(
    const CnfSize& base,
    const CnfSize& bd,
    const CnfSize& cond
  ) : mBaseSize{base},
      mBdSize{bd},
      mCondSize{cond}
  {
  }

  /// @brief デストラクタ
  ~CondGenStats() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 正常回路を表すCNFのサイズを返す．
  CnfSize
  base_size() const
  {
    return mBaseSize;
  }

  /// @brief ブール微分用のCNFのサイズを返す．
  CnfSize
  bd_size() const
  {
    return mBdSize;
  }

  /// @brief 検出条件から作られたCNFのサイズを返す．
  CnfSize
  cond_size() const
  {
    return mCondSize;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  CnfSize mBaseSize;

  CnfSize mBdSize;

  CnfSize mCondSize;

};

END_NAMESPACE_DRUID

#endif // CONDGENSTATS_H
