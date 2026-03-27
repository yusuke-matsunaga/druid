
/// @file MpComp_Pack.cc
/// @brief MpComp_Pack の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpComp_Pack.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "ym/UdGraph.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MpComp_Pack
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MpComp_Pack::MpComp_Pack()
{
}

// @brief デストラクタ
MpComp_Pack::~MpComp_Pack()
{
}

// @brief パタン圧縮の本体
std::vector<TestVector>
MpComp_Pack::run(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const ConfigParam& option
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

END_NAMESPACE_DRUID
