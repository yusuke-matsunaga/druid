#ifndef FAULTGROUPGEN_H
#define FAULTGROUPGEN_H

/// @file FaultGroupGen.h
/// @brief FaultGroupGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultInfo.h"
#include "BaseEnc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultGroupGen FaultGroupGen.h "FaultGroupGen.h"
/// @brief 両立故障グループを求めるクラス
///
/// 概念的には互いに両立な故障の極大グループ(極大両立集合)を求める
/// が，実際には故障検出用の拡張テストキューブ単位で両立集合を求める．
/// 結果としては一つの極大集合が一つの拡張テストキューブに対応する．
//////////////////////////////////////////////////////////////////////
class FaultGroupGen
{
  using ExCube = NodeTimeValList;

public:

  /// @brief コンストラクタ
  FaultGroupGen(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FaultGroupGen();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 両立故障グループを求める．
  vector<vector<FaultInfo>>
  generate(
    const vector<FaultInfo>& fault_list, ///< [in] 故障情報のリスト
    SizeType limit                       ///< [in] 求める要素数の上限
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 極大集合を求める．
  void
  greedy_mcset(
    ExCube& signature,
    SizeType count
  );

  /// @brief 両立性のチェック
  bool
  is_compatible(
    const ExCube& assignments1, ///< [in] 割当1
    const ExCube& assignments2  ///< [in] 割当2
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 基本エンコーダ
  BaseEnc mBaseEnc;

  // タブーリスト
  std::unordered_map<NodeTimeVal, SizeType> mTabuList;

};

END_NAMESPACE_DRUID

#endif // FAULTGROUPGEN_H
