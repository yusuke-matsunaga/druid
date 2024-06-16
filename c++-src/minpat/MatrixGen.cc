
/// @file MatrixGen.cc
/// @brief MatrixGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "MatrixGen.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "Fsim.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

// @brief 被覆行列を作る．
McMatrix
MatrixGen::generate(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list
)
{
  Fsim fsim{network, fault_list, 3, true};
  fsim.clear_patterns();
  fsim.set_skip_all();
  vector<SizeType> rowid_map(network.max_fault_id());
  for ( auto row_id: Range(fault_list.size()) ) {
    auto fault = fault_list[row_id];
    rowid_map[fault->id()] = row_id;
    fsim.clear_skip(fault);
  }

  McMatrix matrix{fault_list.size(), tv_list.size()};

  SizeType wpos = 0;
  SizeType tv_base = 0;
  for ( auto tv: tv_list ) {
    fsim.set_pattern(wpos, tv);
    ++ wpos;
    if ( wpos == PV_BITLEN || tv_base + wpos == tv_list.size() ) {
      SizeType ndet = fsim.ppsfp();
      vector<int> det_list[PV_BITLEN];
      for ( auto i: Range(ndet) ) {
	auto fault = mFsim.det_fault(i);
	auto dbits = mFsim.det_fault_pat(i);
	auto row_id = rowid_map[fault->id()];
	for ( auto bit: Range(wpos) ) {
	  if ( dbits & (1UL << bit) ) {
	    SizeType tvid = tv_base + bit;
	    matrix.insert_elem(row_id, tv_base + bit);
	  }
	}
      }
      fsim.clear_patterns();
      wpos = 0;
      tv_base += PV_BITLEN;
    }
  }

  return std::move(matrix);
}

END_NAMESPACE_DRUID
