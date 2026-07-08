#ifndef RANDPATGEN_H
#define RANDPATGEN_H

/// @file RandPatGen.h
/// @brief RandPatGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "PatGen.h"
#include <random>


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class RandPatGen RandPatGen.h "RandPatGen.h"
/// @brief ランダムパタンを生成する PatGen の派生クラス
//////////////////////////////////////////////////////////////////////
class RandPatGen :
  public PatGen
{
public:

  /// @brief コンストラクタ
  RandPatGen(
    const FaultInfo& fault_info ///< [in] 故障の情報を持つオブジェクト
  );

  /// @brief デストラクタ
  ~RandPatGen() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief パタンを作る．
  void
  gen(
    SizeType size,                   ///< [in] パタンのサイズ
    std::vector<TestVector>& tv_buff ///< [in] 生成したパタンを格納するオブジェクト
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

#endif // RANDPATGEN_H
