
/// @file MpComp_Simple.cc
/// @brief MpComp_Simple の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpComp_Simple.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "fsim/Fsim.h"
#include "dtpg/DtpgEngine.h"
#include "FaultGroup.h"
#include "ym/MinCov.h"
#include "ym/UdGraph.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MpComp_Simple
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MpComp_Simple::MpComp_Simple()
{
}

// @brief デストラクタ
MpComp_Simple::~MpComp_Simple()
{
}

BEGIN_NONAMESPACE

std::vector<TestVector>
mincov(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  SizeType drop_limit
)
{
  auto network = fault_list.network();

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
    auto nc = tv_list.size();
    auto fsim_option = JsonValue::object();
    fsim_option.add("has_x", true);
    Fsim fsim(fault_list, fsim_option);
    std::unordered_set<SizeType> fault_set;
    for ( SizeType col_pos = 0; col_pos < nc; ++ col_pos ) {
      auto& tv = tv_list[col_pos];
      auto fsim_res = fsim.sppfp(tv);
      auto tmp_fault_list = fsim_res.fault_list(0);
      for ( auto fid: tmp_fault_list ) {
	auto row_pos = fid_map[fid];
	mincov.insert_elem(row_pos, col_pos);
	++ det_count[fid];
	if ( det_count[fid] >= drop_limit ) {
	  auto fault = network.fault(fid);
	  fsim.set_skip(fault);
	}
      }
    }

    timer.stop();
    std::cout << std::left << std::setw(20)
	      << "simulation end: " << timer.get_time() << std::endl;
  }

  std::vector<TestVector> new_list;
  {
    Timer timer;
    timer.start();
    std::vector<SizeType> solution;
    auto nc = mincov.solve(solution);
    new_list.reserve(nc);
    for ( auto i: solution ) {
      new_list.push_back(tv_list[i]);
    }
    timer.stop();
    std::cout << std::left << std::setw(20)
	      << "minimum cover end: " << timer.get_time() << std::endl
	      << std::left << std::setw(20)
	      << "# of pats: " << new_list.size() << std::endl;
  }
  return new_list;
}

TestVector
expand_tv(
  const TestVector& tv,
  const TpgFaultList& fault_list
)
{
  auto network = fault_list.network();
  auto tv_assign_list = network.assign_list(tv);
  AssignList new_assign_list;
  for ( auto fault: fault_list ) {
    auto root = fault.ffr_root();
    auto ffr = network.ffr(root);
    DtpgEngine engine(ffr);
    auto lits = engine.make_detect_condition(fault);
    auto tv_lits = engine.conv_to_literal_list(tv_assign_list);
    lits.insert(lits.end(), tv_lits.begin(), tv_lits.end());
    auto res = engine.solver().solve(lits);
    if ( res != SatBool3::True ) {
      throw std::logic_error{"something wrong"};
    }
    auto& model = engine.solver().model();
    auto cond = engine.extract_sufficient_condition(fault, model);
    auto pi_assign = engine.justify(cond, model);
    new_assign_list.merge(pi_assign);
  }
  auto new_tv = TestVector(new_assign_list);
  return new_tv;
}

std::vector<TestVector>
expand(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list
)
{
  // 各テストベクタの検出する故障リストを求める．
  auto fg_list = FaultGroup::make_list(tv_list, fault_list);

  // 検出故障数の降順に並べる．
  std::sort(fg_list.begin(), fg_list.end(),
	    [](const FaultGroup& a, const FaultGroup& b) -> bool {
	      return a.fault_list().size() > b.fault_list().size();
	    });

  // この順番に故障検出の担当するベクタを決める．
  // -> 担当でない故障を削除する．
  // -> 結果，テストベクタを更新する．
  auto network = fault_list.network();
  std::vector<bool> mark(network.max_fault_id(), false);
  std::vector<TestVector> new_tv_list;
  new_tv_list.reserve(fg_list.size());
  for ( auto& fg: fg_list ) {
    TpgFaultList new_fault_list;
    for ( auto fault: fg.fault_list() ) {
      auto fid = fault.id();
      if ( !mark[fid] ) {
	mark[fid] = true;
	new_fault_list.push_back(fault);
      }
    }
    auto new_tv = expand_tv(fg.testvector(), new_fault_list);
    new_tv_list.push_back(new_tv);
  }
  return new_tv_list;
}

std::vector<TestVector>
packing(
  const std::vector<TestVector>& tv_list
)
{
  Timer timer;
  timer.start();

  auto np = tv_list.size();

  // 衝突グラフを作る．
  UdGraph graph(np);
  for ( SizeType i1 = 0; i1 < np - 1; ++ i1 ) {
    auto& tv1 = tv_list[i1];
    for ( SizeType i2 = i1 + 1; i2 < np; ++ i2 ) {
      auto& tv2 = tv_list[i2];
      if ( !(tv1 && tv2) ) {
	// 衝突している．
	graph.connect(i1, i2);
      }
    }
  }
  // 最小彩色問題を解く．
  std::vector<SizeType> color_map;
  auto nc = graph.coloring(color_map);

  // 結果のテストパタンを作る．
  std::vector<std::vector<SizeType>> group_list(nc);
  for ( SizeType i = 0; i < np; ++ i ) {
    auto c = color_map[i];
    group_list[c - 1].push_back(i);
  }
  std::vector<TestVector> new_tv_list;
  new_tv_list.reserve(nc);
  for ( auto& group: group_list ) {
    auto ng = group.size();
    if ( ng == 0 ) {
      throw std::logic_error{"group.size() == 0"};
    }
    auto tv = tv_list[group[0]];
    for ( SizeType j = 1; j < ng; ++ j ) {
      tv &= tv_list[group[j]];
    }
    new_tv_list.push_back(tv);
  }
  timer.stop();
  std::cout << std::left << std::setw(20)
	    << "packing end: " << timer.get_time() << std::endl
	    << std::left << std::setw(20)
	    << "# of pats: " << new_tv_list.size() << std::endl;

  return new_tv_list;
}

END_NONAMESPACE

// @brief パタン圧縮の本体
std::vector<TestVector>
MpComp_Simple::_run(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const JsonValue& option
)
{
  SizeType drop_limit = get_int(option, "drop_limit", 4000);

  // 最小被覆を求める．
  auto tv_list1 = mincov(tv_list, fault_list, drop_limit);

  // テストキューブの拡大を行う．
  auto tv_list2 = expand(tv_list1, fault_list);

  // 最小彩色を行う．
  auto tv_list3 = packing(tv_list2);

  // 再度最小被覆を行う．
  auto tv_list4 = mincov(tv_list3, fault_list, drop_limit);

  return tv_list4;
}

END_NAMESPACE_DRUID
