
/// @file Dichotomy.cc
/// @brief Dichotomy の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "Dichotomy.h"
#include "DiGroupMgr.h"
//#include "FFRAnalyze.h"
#include "types/TpgNetwork.h"
#include "types/TestVector.h"
#include "types/PackedVal.h"
#include "fsim/Fsim.h"
#include "dtpg/DtpgMgr.h"
#include <random>


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

void
ffr_analyze(
  const TpgFaultList& fault_list,
  std::vector<TestVector>& tv_list,
  std::unordered_set<SizeType>& del_mark
)
{
#if 0
  auto analyze = FFRAnalyze(fault_list);

  // どちらかの故障が検出できずにもう一方の故障が検出できる条件を探す．
  for ( auto fault: fault_list ) {
    if ( del_mark.count(fault.id()) == 0 ) {
      if ( analyze.fault_status(fault) != FFRAnalyze::DETECTED ) {
	del_mark.insert(fault.id());
      }
    }
  }
#endif
}

void
ffr_analyze2(
  const TpgFaultList& fault_list1,
  const TpgFaultList& fault_list2,
  std::vector<TestVector>& tv_list,
  std::unordered_set<SizeType>& del_mark
)
{
#if 0
  auto analyze = FFRAnalyze2(fault_list1, fault_list2);

  // どちらかの故障が検出できずにもう一方の故障が検出できる条件を探す．
  auto nf1 = fault_list1.size();
  auto nf2 = fault_list2.size();
  for ( SizeType i1 = 0; i1 < nf1; ++ i1 ) {
    auto fault1 = fault_list1[i1];
    if ( del_mark.count(fault1.id()) > 0 ) {
      continue;
    }
    for ( SizeType i2 = 0; i2 < nf2; ++ i2 ) {
      auto fault2 = fault_list2[i2];
      if ( del_mark.count(fault2.id()) > 0 ) {
	continue;
      }
      // fault1 を検出して fault2 を検出しない条件
      bool dom1 = true;
      auto res1 = analyze.dom_check(i1, i2, tv_list);
      if ( res1 == SatBool3::True ) {
	dom1 = false;
      }

      // fault1 を検出せずに fault2 を検出する条件
      bool dom2 = true;
      auto res2 = analyze.dom_check(i2, i1, tv_list);
      if ( res2 == SatBool3::True ) {
	dom2 = false;
      }

      if ( dom1 ) {
	if ( dom2 ) {
	  // fault1 と fault2 は等価故障
	  del_mark.insert(fault2.id());
	  break;
	}
	else {
	  // fault1 は fault2 に支配されている．
	  del_mark.insert(fault2.id());
	  break;
	}
      }
      else if ( dom2 ) {
	// fault2 は fault1 に支配されている．
	del_mark.insert(fault1.id());
	break;
      }
    }
  }
#endif
}

// fault_list の先頭の故障を検出せずに残りの故障を検出するテストベクタをもとめる．
std::vector<TestVector>
differenciate(
  const TpgFaultList& fault_list
)
{
  auto network = fault_list.network();

  // ffr によって分類する．
  std::vector<TpgFaultList> fg_list;
  fg_list.reserve(fault_list.size());
  std::unordered_map<SizeType, SizeType> ffr_map;
  for ( auto fault: fault_list ) {
    auto ffr = network.ffr(fault);
    auto ffr_id = ffr.id();
    SizeType gid = 0;
    if ( ffr_map.count(ffr_id) == 0 ) {
      gid = fg_list.size();
      fg_list.push_back(TpgFaultList());
    }
    else {
      gid = ffr_map.at(ffr_id);
    }
    auto& fg = fg_list[gid];
    fg.push_back(fault);
  }

  // 故障の検出状況を区別するテストベクタのリスト
  std::vector<TestVector> tv_list;

  // 他の故障に支配されている故障番号のマーク
  std::unordered_set<SizeType> del_mark;

  // まず複数の故障を持つ ffr を探す．
  for ( auto& fg: fg_list ) {
    if ( fg.size() >= 2 ) {
      ffr_analyze(fg, tv_list, del_mark);
    }
  }

  // 異なる ffr 間の故障の解析を行う．
  auto ng = fg_list.size();
  for ( SizeType i1 = 0; i1 < ng - 1; ++ i1 ) {
    auto& fg1 = fg_list[i1];
    for ( SizeType i2 = i1 + 1; i2 < ng; ++ i2 ) {
      auto& fg2 = fg_list[i2];
      ffr_analyze2(fg1, fg2, tv_list, del_mark);
    }
  }

  return tv_list;
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Dichotomy
//////////////////////////////////////////////////////////////////////

// @brief 故障グループの細分化を行う．
DiGroupMgr
Dichotomy::run(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  // 対象のネットワーク
  auto network = fault_list.network();

  // 故障シミュレータ
  auto fsim_option = option.get_param("fsim");
  Fsim fsim(fault_list, fsim_option);

  // 初期グループは全ての故障を含んだ一つのグループ．
  auto prev_mgr = DiGroupMgr(fault_list);

  std::mt19937 randgen;

  // シミュレーションの結果を用いて二分法を行う．
  while ( prev_mgr.group_num() < fault_list.size() ) {
    // ランダムパタンを作る．
    std::vector<TestVector> tv_list;
    tv_list.reserve(PV_BITLEN);
    for ( SizeType i = 0; i < PV_BITLEN; ++ i ) {
      auto tv = TestVector(network);
      tv.set_from_random(randgen);
      tv_list.push_back(tv);
    }

    // そのパタンを使って故障シミュレーションを行う．
    auto res = fsim.ppsfp(tv_list);
    bool changed = false;
    for ( SizeType i = 0; i < PV_BITLEN; ++ i ) {
      std::unordered_set<SizeType> fault_set;
      for ( auto fid: res.fault_list(i) ) {
	fault_set.insert(fid);
      }
      auto new_mgr = DiGroupMgr(prev_mgr, fault_set);
      if ( new_mgr != prev_mgr ) {
	// 細分化できたら更新する．
	std::swap(prev_mgr, new_mgr);
	changed = true;
      }
    }
    if ( !changed ) {
      break;
    }
  }

  // 未検出の故障を対象に DTPG を行う．
  auto undet_group = prev_mgr.undet_group();
  if ( undet_group != nullptr ) {
    TpgFaultList untest_list;
    TpgFaultList abort_list;
    auto fault_list = undet_group->fault_list();
    std::vector<TestVector> tv_list;
    tv_list.reserve(fault_list.size());
    auto dtpg_option = option.get_param("dtpg");
    auto res = DtpgMgr::run(fault_list, dtpg_option);
    for ( auto fault: fault_list ) {
      auto status = res.status(fault);
      switch ( status ) {
      case FaultStatus::Detected:
      {
	auto tv = res.testvector(fault);
	tv.fix_x_from_random(randgen);
	tv_list.push_back(tv);
      }
      break;
      case FaultStatus::Untestable:
	untest_list.push_back(fault);
	break;
      case FaultStatus::Undetected:
	abort_list.push_back(fault);
	break;
      }
    }
    Fsim fsim(fault_list, fsim_option);
    for ( auto& tv: tv_list ) {
      auto res = fsim.sppfp(tv);
      std::unordered_set<SizeType> fault_set;
      for ( auto fid: res.fault_list(0) ) {
	fault_set.insert(fid);
      }
      auto new_mgr = DiGroupMgr(prev_mgr, fault_set);
      if ( new_mgr != prev_mgr ) {
	std::swap(prev_mgr, new_mgr);
      }
    }
  }

  for ( ; ; ) {
    // singleton でない故障グループを調べる．
    const DiGroup* group1 = nullptr;
    for ( auto group: prev_mgr.group_list() ) {
      if ( group->fault_list().size() > 1 ) {
	group1 = group;
	break;
      }
    }
    if ( group1 == nullptr ) {
      break;
    }
    auto tv_list = differenciate(group1->fault_list());
    for ( auto& tv: tv_list ) {
      auto res = fsim.sppfp(tv);
      std::unordered_set<SizeType> fault_set;
      for ( auto fid: res.fault_list(0) ) {
	fault_set.insert(fid);
      }
      auto new_mgr = DiGroupMgr(prev_mgr, fault_set);
      if ( new_mgr != prev_mgr ) {
	std::swap(prev_mgr, new_mgr);
      }
    }
  }

  return prev_mgr;
}

END_NAMESPACE_DRUID
