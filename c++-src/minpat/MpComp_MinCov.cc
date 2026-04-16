
/// @file MpComp_MinCov.cc
/// @brief MpComp_MinCov の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpComp_MinCov.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "fsim/Fsim.h"
#include "ym/MinCov.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MpComp_MinCov
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MpComp_MinCov::MpComp_MinCov()
{
}

// @brief デストラクタ
MpComp_MinCov::~MpComp_MinCov()
{
}

std::vector<TestVector>
MpComp_MinCov::run(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  SizeType drop_limit = option.get_int_elem("drop_limit", 4000);

  auto network = fault_list.network();

  // 被覆行列を作る．
  MinCov mincov;

  {
    Timer timer;
    timer.start();

    std::vector<SizeType> fid_map(network.max_fault_id(), 0);
    {
      SizeType i = 0;
      for ( auto fault: fault_list ) {
	fid_map[fault.id()] = i;
	++ i;
      }
    }

    std::vector<SizeType> det_count(network.max_fault_id(), 0);
    auto fsim_option = option.get_param("fsim");
    fsim_option.add("has_x", true);
    Fsim fsim(fault_list, fsim_option);
    std::unordered_set<SizeType> fault_set;
    auto nc = tv_list.size();
    for ( SizeType col_pos = 0; col_pos < nc; ++ col_pos ) {
      auto& tv = tv_list[col_pos];
      auto fsim_res = fsim.sppfp(tv);
      auto tmp_fault_list = fsim_res.fault_list(0);
      for ( auto fault: tmp_fault_list ) {
	auto fid = fault.id();
	auto row_pos = fid_map[fid];
	mincov.insert_elem(row_pos, col_pos);
	++ det_count[fid];
	if ( det_count[fid] >= drop_limit ) {
	  fsim.set_skip(fault);
	}
      }
    }

    timer.stop();
    std::cout << std::left << std::setw(20)
	      << "simulation end: " << timer.get_time() << std::endl;
  }

  // 最小被覆問題を解く．
  std::vector<TestVector> new_tv_list;
  {
    Timer timer;
    timer.start();

    std::vector<SizeType> solution;
    auto nc = mincov.solve(solution);
    new_tv_list.reserve(nc);
    for ( auto i: solution ) {
      auto tv = tv_list[i];
      new_tv_list.push_back(tv);
    }

    timer.stop();
    std::cout << std::left << std::setw(20)
	      << "minimum cover end: " << timer.get_time() << std::endl
	      << std::left << std::setw(20)
	      << "# of pats: " << nc << std::endl;
  }

  return new_tv_list;
}

END_NAMESPACE_DRUID
