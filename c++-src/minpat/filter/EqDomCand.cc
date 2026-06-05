
/// @file EqDomCand.cc
/// @brief EqDomCand の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqDomCand.h"
#include "DomGraph.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス EqDomCand
//////////////////////////////////////////////////////////////////////

// @brief 支配故障の候補数を返す．
SizeType
EqDomCand::total_cand_num() const
{
  SizeType total_cand = 0;
  for ( auto fault: mFaultList ) {
    total_cand += domcand(fault).size();
  }
  return total_cand;
}

// @brief 直接支配故障の候補数を返す．
SizeType
EqDomCand::total_imm_cand_num() const
{
#if 0
  DomGraph dg(*this);
  dg.print(std::cout);
  SizeType imm_cand = 0;
  for ( auto fault: mFaultList ) {
    auto rank = dg.rank(fault);
    for ( auto fault1: domcand(fault) ) {
      auto rank1 = dg.rank(fault1);
      if ( rank1 == rank + 1 ) {
	++ imm_cand;
      }
    }
  }
  return imm_cand;
#else
  return 0;
#endif
}

// @brief 内容を出力する．
void
EqDomCand::print(
  std::ostream& s
) const
{
  SizeType id = 0;
  for ( auto& eqgroup: mEqGroupList ) {
    s << "EQ#" << id << ":";
    for ( auto fault: eqgroup ) {
      s << " " << fault.str();
    }
    s << std::endl;
    ++ id;
  }
  s << "--------------------------" << std::endl;
  for ( auto fault: mFaultList ) {
    s << fault.str() << ":";
    for ( auto fault1: mDomCandArray[fault.id()] ) {
      s << " " << fault1.str();
    }
    s << std::endl;
  }
  s << std::endl;
}

// @brief 故障リストを設定する．
void
EqDomCand::init(
  const TpgFaultList& fault_list
)
{
  mFaultList = fault_list;
  SizeType max_id = 0;
  for ( auto fault: mFaultList ) {
    max_id = std::max(max_id, fault.id());
  }
  ++ max_id;
  mDomCandArray.resize(max_id);
}

END_NAMESPACE_DRUID
