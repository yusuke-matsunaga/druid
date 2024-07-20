#ifndef SIM_H
#define SIM_H

/// @file Sim.h
/// @brief Sim のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "PackedVal.h"
#include "TpgNode.h"
#include <random>


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Sim Sim.h "Sim.h"
/// @brief 簡単な論理シミュレータ
//////////////////////////////////////////////////////////////////////
class Sim
{
public:

  /// @brief コンストラクタ
  Sim(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief デストラクタ
  ~Sim() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 乱数を用いたシミュレーションを行う．
  void
  sim_random();

  /// @brief 割り当て条件を満たしているか調べる．
  PackedVal
  check(
    const NodeTimeValList& assign
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの値を計算する(組み合わせ回路用)．
  void
  calc_node(
    const TpgNode* node ///< [in] ノード
  );

  /// @brief ノードの値を計算する(順序回路の0時刻用)．
  void
  calc_node_0(
    const TpgNode* node ///< [in] ノード
  );

  /// @brief ノードの値を計算する(順序回路の1時刻用)．
  void
  calc_node_1(
    const TpgNode* node ///< [in] ノード
  );

  /// @brief ノードの値を読み出す．
  bool
  node_val(
    const TpgNode* node, ///< [in] ノード
    int time             ///< [in] 時刻(0 or 1)
  ) const
  {
    return mValArray[node->id() * 2 + time];
  }

  /// @brief ノードの値を設定する．
  void
  set_node_val(
    const TpgNode* node, ///< [in] ノード
    int time,            ///< [in] 時刻(0 or 1)
    bool val             ///< [in] 値
  )
  {
    mValArray[node->id() * 2 + time] = val;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 値の配列
  // キーは ノード番号 * 2 + time(0 or 1)
  vector<PackedVal> mValArray;

  // 乱数生成器
  std::mt19937 mRandGen;

  // 一様乱数分布生成器
  std::uniform_int_distribution<PackedVal> mRandDist;

};

END_NAMESPACE_DRUID

#endif // SIM_H
