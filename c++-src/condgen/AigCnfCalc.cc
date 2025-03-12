
/// @file AigCnfCalc.cc
/// @brief AigCnfCalc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "AigCnfCalc.h"


BEGIN_NAMESPACE_DRUID

// @brief AIG を CNF に変換した時のサイズを見積もる．
SizeType
AigCnfCalc::calc_size(
  const AigHandle& aig
)
{
  if ( aig.is_const() ) {
    return 0;
  }
  if ( aig.is_input() ) {
    return 1;
  }
  if ( mAigDict.count(aig) > 0 ) {
    return mAigDict.at(aig);
  }

  auto fanin_list = aig.ex_fanin_list();
  SizeType nlit = 0;
  if ( aig.inv() ) {
    for ( auto& aig1: fanin_list ) {
      auto nlit1 = calc_size(~aig1);
      if ( nlit1 > 1 ) {
	mSize += CnfSize{nlit1, nlit1 * 2};
      }
    }
    mSize += CnfSize{1, fanin_list.size() + 1};
    nlit = 1;
  }
  else {
    for ( auto& aig1: fanin_list ) {
      auto nlit1 = calc_size(aig1);
      nlit += nlit1;
    }
  }
  mAigDict.emplace(aig, nlit);
  return nlit;
}

END_NAMESPACE_DRUID
