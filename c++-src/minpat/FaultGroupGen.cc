
/// @file FaultGroupGen.cc
/// @brief FaultGroupGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultGroupGen.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FaultGroupGen::FaultGroupGen(
  const TpgNetwork& network,
  const JsonValue& option
) : mNetwork{network},
    mBaseEnc{network, option}
{
  auto& node_list = network.node_list();
  mBaseEnc.make_cnf(node_list, node_list);
}

// @brief デストラクタ
FaultGroupGen::~FaultGroupGen()
{
}

// @brief 両立故障グループを求める．
vector<vector<FaultInfo>>
FaultGroupGen::generate(
  const vector<FaultInfo>& fault_list,
  SizeType limit
)
{
  // 現在選択されているシグネチャ
  // 初期値は空
  ExCube signature{};

  // 禁止期間
  SizeType tenure = 5;
  for ( SizeType count; count < limit; ++ count ) {
    // 極大集合を求める．
    greedy_mcset(signature, count);
    // 記録する．
    // 場合によっては重複チェックを行う．

    // signature から取り除く要素を選ぶ．
    // auto bit = select_bit(signature);
    // signature.clear(bit);

    // bit をタブーリストに入れる．
    //mTabuList[bit] = count + tenure;
  }
}

// @brief 極大集合を求める．
void
FaultGroupGen::greedy_mcset(
  ExCube& signature,
  SizeType count
)
{
  // 現在ブロックされている故障のリスト

  for ( ; ; ) {
    // 未選択の故障の拡張テストキューブのうち，
    // 新規に追加したときにブロック故障の増加数が
    // 最小となるものを求める．

    // 追加できる故障がなくなったら終わる．

    // シグネチャを更新する．

    // cur_block_list を更新する．
  }
}

// @brief 両立性のチェック
bool
FaultGroupGen::is_compatible(
  const ExCube& assignments1,
  const ExCube& assignments2
)
{
  auto lits1 = mBaseEnc.conv_to_literal_list(assignments1);
  auto lits2 = mBaseEnc.conv_to_literal_list(assignments2);
  lits1.insert(lits1.end(), lits2.begin(), lits2.end());
  return mBaseEnc.solver().solve(lits1) == SatBool3::True;
}

END_NAMESPACE_DRUID
