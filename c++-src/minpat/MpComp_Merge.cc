
/// @file MpComp_Merge.cc
/// @brief MpComp_Merge の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpComp_Merge.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "dtpg/DtpgEngine.h"
#include "fsim/Fsim.h"
#include "ym/Timer.h"
#include "FaultGroup.h"
#include "MpAnalyze.h"
#include "MpComp_MinCov.h"

//#define VERIFY_MAKE_FAULT_GROUP 1
//#define VERIFY_REPLACE 1


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
static int debug = 0;
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス MpComp_Merge
//////////////////////////////////////////////////////////////////////

BEGIN_NONAMESPACE

std::vector<SizeType>
merge(
  const std::vector<SizeType>& list1,
  const std::vector<SizeType>& list2
)
{
  auto n1 = list1.size();
  auto n2 = list2.size();
  SizeType i1 = 0;
  SizeType i2 = 0;
  std::vector<SizeType> dst_list;
  dst_list.reserve(n1 + n2);
  while ( i1 < n1 && i2 < n2 ) {
    auto v1 = list1[i1];
    auto v2 = list2[i2];
    if ( v1 < v2 ) {
      dst_list.push_back(v1);
      ++ i1;
    }
    else if ( v1 > v2 ) {
      dst_list.push_back(v2);
      ++ i2;
    }
    else {
      dst_list.push_back(v1);
      ++ i1;
      ++ i2;
    }
  }
  for ( ; i1 < n1; ++ i1 ) {
    auto v1 = list1[i1];
    dst_list.push_back(v1);
  }
  for ( ; i2 < n2; ++ i2 ) {
    auto v2 = list2[i2];
    dst_list.push_back(v2);
  }
  return dst_list;
}

// 各テストベクタの検出する故障のリストを作る．
std::vector<std::vector<SizeType>>
make_fault_list(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  auto fsim_option = option.get_param("fsim");
  fsim_option.add("has_x", true);
  Fsim fsim(fault_list, fsim_option);
  std::vector<std::vector<SizeType>> fault_list_array;
  auto ntv = tv_list.size();
  fault_list_array.reserve(ntv);
  for ( auto& tv: tv_list ) {
    auto res = fsim.sppfp(tv);
    fault_list_array.push_back(res.fault_list(0));
  }
  return fault_list_array;
}

// 各テストベクタが唯一検出する故障のリストを作る．
std::vector<std::vector<SizeType>>
make_ex_list(
  const TpgFaultList& fault_list,
  const std::vector<std::vector<SizeType>>& fault_list_array
)
{
  auto n = fault_list_array.size();
  std::vector<std::vector<SizeType>> fault_list1(n);
  {
    std::vector<SizeType> acc_fault_list;
    for ( SizeType i = 0; i < n; ++ i ) {
      fault_list1[i] = acc_fault_list;
      acc_fault_list = merge(acc_fault_list, fault_list_array[i]);
    }
  }
  std::vector<std::vector<SizeType>> fault_list2(n);
  {
    std::vector<SizeType> acc_fault_list;
    for ( SizeType i = 0; i < n; ++ i ) {
      auto pos = n - i - 1;
      fault_list1[pos] = acc_fault_list;
      acc_fault_list = merge(acc_fault_list, fault_list_array[pos]);
    }
  }
  auto network = fault_list.network();
  std::vector<std::vector<SizeType>> ex_list_array(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    std::vector<bool> mark(network.max_fault_id(), false);
    for ( auto id: fault_list1[i] ) {
      mark[id] = true;
    }
    for ( auto id: fault_list2[i] ) {
      mark[id] = true;
    }
    for ( auto fid: fault_list_array[i] ) {
      if ( !mark[fid] ) {
	ex_list_array[i].push_back(fid);
      }
    }
  }
  return ex_list_array;
}

// 故障グループを作る．
FaultGroup
make_fault_group(
  const TpgFaultList& fault_list,
  const TestVector& tv,
  const ConfigParam& option
)
{
  auto network = fault_list.network();
  auto tv_assign_list = AssignList(network, tv);
  auto dtpg_option = option.get_param("dtpg");
  AssignList gtc;
  for ( auto fault: fault_list ) {
    auto ffr = network.ffr(fault);
    DtpgEngine engine(ffr, dtpg_option);
    auto lits1 = engine.conv_to_literal_list(tv_assign_list);
    auto lits2 = engine.make_detect_condition(fault);
    auto lits = lits1;
    lits.insert(lits.end(), lits2.begin(), lits2.end());
    auto res = engine.solver().solve(lits);
    if ( res != SatBool3::True ) {
      throw std::logic_error{"something wrong"};
    }
    auto model = engine.solver().model();
    auto cond = engine.extract_sufficient_condition(fault, model, tv_assign_list);
    auto cond1 = cond.main_cond() + cond.aux_cond();
    if ( debug > 1 ) {
      std::cout << "GTC:     " << gtc << std::endl
		<< "cond1:   " << cond1 << std::endl;
    }
    gtc.merge(cond1);
    if ( debug > 1 ) {
      std::cout << "new GTC: " << gtc << std::endl;
    }
  }
#ifdef VERIFY_MAKE_FAULT_GROUP
  { // 検証を行う．
    for ( auto fault: fault_list ) {
      {
	auto node = fault.origin_node();
	BdEngine engine(node, dtpg_option);
	auto pvar = engine.prop_var();
	auto lits = engine.conv_to_literal_list(gtc);
	lits.push_back(~pvar);
	auto res = engine.solver().solve(lits);
	if ( res != SatBool3::False ) {
	  std::cout << "Error gtc is not a sufficient condition" << std::endl;
	  std::cout << fault.str() << std::endl
		    << gtc << std::endl;
	  abort();
	}
      }
      {
	auto node = fault.origin_node();
	DtpgEngine engine(node, dtpg_option);
	auto lits1 = engine.make_detect_condition(fault);
	auto lits2 = engine.conv_to_literal_list(gtc);
	lits1.insert(lits1.end(), lits2.begin(), lits2.end());
	auto res = engine.solver().solve(lits1);
	if ( res != SatBool3::True ) {
	  std::cout << "Error gtc is not a detect condition" << std::endl;
	  std::cout << fault.str() << std::endl
		    << gtc << std::endl;
	  abort();
	}
      }
    }
  }
#endif
  return FaultGroup(fault_list, gtc);
}

// 故障グループのリストを作る．
std::vector<FaultGroup>
make_fg_list(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  MpAnalyze analyze(tv_list, fault_list, option);
  auto network = fault_list.network();
  auto ntv = tv_list.size();

  // 各テストベクタの検出する故障を求める．
  auto fault_list_array = make_fault_list(tv_list, fault_list, option);

  // 各テストベクタが唯一検出する故障を求める．
  auto ex_list_array = make_ex_list(fault_list, fault_list_array);

  // ex_list_array の要素数の降順にソートする．
  std::vector<SizeType> pos_list(ntv);
  for ( SizeType i = 0; i < ntv; ++ i ) {
    pos_list[i] = i;
  }
  std::sort(pos_list.begin(), pos_list.end(),
	    [&](SizeType a, SizeType b) -> bool {
	      return ex_list_array[a].size() < ex_list_array[b].size();
	    });

  // pos_list の順に故障グループを作る．
  std::vector<bool> det_mark(network.max_fault_id(), false);
  std::vector<FaultGroup> fg_list;
  fg_list.reserve(ntv);
  for ( auto pos: pos_list ) {
    TpgFaultList fault_list1;
    for ( auto fid: fault_list_array[pos] ) {
      if ( !det_mark[fid] ) {
	det_mark[fid] = true;
	auto fault = network.fault(fid);
	fault_list1.push_back(fault);
      }
    }
    if ( !fault_list1.empty() ) {
      auto fg = make_fault_group(fault_list1, tv_list[pos], option);
      fg_list.push_back(fg);
    }
  }

  return fg_list;
}

std::vector<FaultGroup>
replace(
  const std::vector<FaultGroup>& fg_list,
  SizeType tgt_pos,
  const ConfigParam& option
)
{
  SizeType limit = option.get_int_elem("limit", 1);
  auto dtpg_option = option.get_param("dtpg");

  auto& tgt_fg = fg_list[tgt_pos];
  auto& fault_list = tgt_fg.fault_list();
  if ( fault_list.size() > limit ) {
    return {};
  }

  auto network = fault_list.network();
  auto nfg = fg_list.size();

  // fault_list に含まれる故障を他の故障グループにマージする．
  std::unordered_map<SizeType, FaultGroup> replace_map;
  {
    std::cout << "Try to replace of ";
    for ( auto fault: fault_list ) {
      std::cout << " " << fault.str();
    }
    std::cout << std::endl;
  }
  for ( auto fault: fault_list ) {
    bool found = false;
    auto ffr = network.ffr(fault);
    DtpgEngine engine(ffr, dtpg_option);
    for ( SizeType i = 0; i < nfg; ++ i ) {
      if ( i == tgt_pos ) {
	continue;
      }
      auto& fg = fg_list[i];
      auto lits = engine.make_detect_condition(fault);
      auto lits1 = engine.conv_to_literal_list(fg.gtc());
      lits.insert(lits.end(), lits1.begin(), lits1.end());
      auto res = engine.solver().solve(lits);
      if ( res == SatBool3::True ) {
	auto model = engine.solver().model();
	auto cond = engine.extract_sufficient_condition(fault, model);
	auto cond1 = cond.main_cond() + cond.aux_cond();
	auto new_fg = fg;
	new_fg.add(fault, cond1);
#ifdef VERIFY_REPLACE
	{ // 検証を行う．
	  std::cout << fault.str()
		    << " is moved to FG#" << i << std::endl;
	  fg.print(std::cout);
	  for ( auto fault: new_fg.fault_list() ) {
	    auto ffr = network.ffr(fault);
	    DtpgEngine engine(ffr, dtpg_option);
	    auto lits = engine.make_detect_condition(fault);
	    auto lits1 = engine.conv_to_literal_list(new_fg.gtc());
	    lits.insert(lits.end(), lits1.begin(), lits1.end());
	    auto res = engine.solver().solve(lits);
	    if ( res != SatBool3::True ) {
	      std::cout << "Error: " << fault.str() << std::endl;
	      new_fg.print(std::cout);
	      abort();
	    }
	  }
	}
#endif
	replace_map.emplace(i, new_fg);
	found = true;
	break;
      }
    }
    if ( !found ) {
      // マージに失敗した．
      if ( debug ) {
	std::cout << "-> Failed" << std::endl;
      }
      return {};
    }
  }
  if ( debug ) {
    std::cout << std::endl
	      << "original fg_list" << std::endl;
    for ( auto& fg: fg_list ) {
      fg.print(std::cout);
      std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "replace_map" << std::endl;
    for ( auto& p: replace_map ) {
      auto pos = p.first;
      auto& fg = p.second;
      std::cout << "#" << pos << std::endl;
      fg.print(std::cout);
      std::cout << std::endl;
    }
  }

  // replace_list に従って結果を作る．
  std::vector<FaultGroup> new_fg_list;
  new_fg_list.reserve(nfg - 1);
  for ( SizeType i = 0; i < nfg; ++ i ) {
    if ( i == tgt_pos ) {
      continue;
    }
    if ( replace_map.count(i) == 0 ) {
      auto& fg = fg_list[i];
      new_fg_list.push_back(fg);
    }
    else {
      auto& fg = replace_map.at(i);
      new_fg_list.push_back(fg);
    }
  }
  std::sort(new_fg_list.begin(), new_fg_list.end(),
	    [](const FaultGroup& a, const FaultGroup& b) -> bool {
	      return a.fault_list().size() > b.fault_list().size();
	    });
  if ( debug ) {
    std::cout << "-> Succeed" << std::endl;
    for ( auto& fg: new_fg_list ) {
      std::cout << std::endl;
      fg.print(std::cout);
      for ( auto fault: fg.fault_list() ) {
	auto ffr = network.ffr(fault);
	DtpgEngine engine(ffr, dtpg_option);
	auto lits = engine.make_detect_condition(fault);
	auto lits1 = engine.conv_to_literal_list(fg.gtc());
	lits.insert(lits.end(), lits1.begin(), lits1.end());
	auto res = engine.solver().solve(lits);
	if ( res != SatBool3::True ) {
	  std::cout << "Error: " << fault.str() << std::endl;
	  fg.print(std::cout);
	  abort();
	}
      }
    }
  }
  return new_fg_list;
}

END_NONAMESPACE

// @brief パタン圧縮の本体
std::vector<TestVector>
MpComp_Merge::run(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  // 最小被覆を求める．
  MpComp_MinCov mincov;
  auto tv_list1 = mincov.run(tv_list, fault_list, option);

  auto fg_list = make_fg_list(tv_list1, fault_list, option);
  if ( debug ) {
    std::cout << std::endl
	      << "make_fg_list" << std::endl;
    auto nfg = fg_list.size();
    for ( SizeType i = 0; i < nfg; ++ i ) {
      auto& fg = fg_list[i];
      std::cout << "Fault Group#" << i << std::endl;
      fg.print(std::cout);
    }
    std::cout << std::endl;
  }

  SizeType limit = option.get_int_elem("limit", 1);
  auto dtpg_option = option.get_param("dtpg");

  auto network = fault_list.network();

  // ex_list の要素数の少ないテストベクタの削除を試みる．
  while ( true ) {
    auto nfg = fg_list.size();
    bool changed = false;
    for ( SizeType i = 0; i < nfg; ++ i ) {
      auto cand_pos = nfg - i - 1;
      auto new_fg_list = replace(fg_list, cand_pos, option);
      if ( !new_fg_list.empty() ) {
	std::swap(fg_list, new_fg_list);
	changed = true;
      }
      if ( changed ) {
	break;
      }
    }
    if ( !changed ) {
      break;
    }
  }

  // fg_list から結果のテストベクタのリストを作る．
  std::vector<TestVector> new_tv_list;
  new_tv_list.reserve(fg_list.size());
  StructEngine engine(network, dtpg_option);
  if ( debug ) {
    std::cout << "=======================" << std::endl;
    std::cout << "final results" << std::endl;
    for ( auto& fg: fg_list ) {
      std::cout << std::endl;
      fg.print(std::cout);
    }
  }
  for ( auto& fg: fg_list ) {
    auto& gtc = fg.gtc();
    auto lits = engine.conv_to_literal_list(gtc);
    auto res = engine.solver().solve(lits);
    if ( res != SatBool3::True ) {
      throw std::logic_error{"something wrong"};
    }
    auto model = engine.solver().model();
    auto cond = SuffCond(gtc, {});
    auto pi_assign_list = engine.justify(cond, model);
    auto tv = TestVector(pi_assign_list);
    new_tv_list.push_back(tv);
  }

  return new_tv_list;
}

END_NAMESPACE_DRUID
