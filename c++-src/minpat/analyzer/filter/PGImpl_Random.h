#ifndef PGIMPL_RANDOM_H
#define PGIMPL_RANDOM_H

/// @file PGImpl_Random.h
/// @brief PGImpl_Random のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "PGImpl.h"
#include <random>


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PGImpl_Random PGImpl_Random.h "PGImpl_Random.h"
/// @brief ランダムパタンを生成する PGImpl の派生クラス
//////////////////////////////////////////////////////////////////////
class PGImpl_Random :
  public PGImpl
{
public:

  /// @brief コンストラクタ
  PGImpl_Random(
    const FaultInfo& fault_info ///< [in] 故障の情報を持つオブジェクト
  );

  /// @brief デストラクタ
  ~PGImpl_Random() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief パタンを作る．
  /// @return パタンを生成した時 true を返す．
  bool
  get_pat(
    TestVector& tv ///< [in] 生成したパタンを格納するオブジェクト
  ) override;

  /// @brief 検出結果で更新する．
  void
  update(
    const FsimResults& res ///< [in] 故障シミュレーションの結果
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 乱数生成器
  std::mt19937 mRandGen;

};

END_NAMESPACE_DRUID

#endif // PGIMPL_RANDOM_H
