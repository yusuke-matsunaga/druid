
/// @file DomCandMgr.cc
/// @brief DomCandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DomCandMgr.h"
#include "DomCandGen.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DomCandMgr::DomCandMgr(
  const TpgNetwork& network
) : mNetwork{network},
    mDomCandListArray(network.max_fault_id()),
    mRevCandListArray(network.max_fault_id())
{
}

// @brief 支配関係の候補を作る．
void
DomCandMgr::generate(
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list,
  SizeType limit
)
{
  DomCandGen gen{mNetwork, fault_list, tv_list};

  gen.run(limit, mDomCandListArray);

  // mDomCandListArray の逆向きのリストを作る．
  for ( auto fault1: fault_list ) {
    for ( auto fault2: dom_cand_list(fault1) ) {
      mRevCandListArray[fault2->id()].push_back(fault1);
    }
  }
}

END_NAMESPACE_DRUID
