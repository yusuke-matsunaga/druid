
/// @file Classifier2.cc
/// @brief Classifier2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Classifier2.h"
#include "TestVector.h"
#include "Fsim.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

using SigKey = pair<DiffBits, SizeType>;

struct Hash {
  SizeType
  operator()(
    const SigKey& key
  ) const
  {
    return key.first.hash() ^ key.second;
  }
};

struct Eq {
  bool
  operator()(
    const SigKey& key1,
    const SigKey& key2
  ) const
  {
    return key1.first == key2.first && key1.second == key2.second;
  }
};

END_NONAMESPACE

// @brief 故障を分類する．
vector<vector<TpgFault>>
Classifier2::run(
  const TpgNetwork& network,
  const vector<TpgFault>& fault_list,
  bool has_prev_state,
  const vector<TestVector>& tv_list,
  bool singleton_drop,
  bool multi
)
{
  Fsim fsim;
  if ( multi ) {
    fsim.initialize(network, has_prev_state, false);
  }
  else {
    fsim.initialize_naive(network, has_prev_state, false);
  }
  fsim.set_fault_list(fault_list);
  Timer timer;
  // 最初はすべての故障が一つのグループとなっている．
  vector<vector<TpgFault>> fg_list{fault_list, };
  for ( auto tv: tv_list ) {
    // fgmap を今回のシミュレーション結果で細分化する．
    // 今回未検出の故障のグループ番号は変わらない．
    unordered_map<SigKey, Si, Hash, Eq> sig_dict;
    timer.start();
    auto fault_list1 = fsim.sppfp(tv);
    timer.stop();
    vector<vector<TpgFault>> new_fg_list;
    for ( auto fgroup: fg_list1 ) {
      std::unordered_map<DiffBits, SizeType> sig_dict;
      for ( auto fault: fgroup ) {
	auto dbits = fsim.sppfp_diffbits(fault);
	SizeType g;
	if ( sig_dict.count(dbits) == 0 ) {
	  // 新しいグループ番号を割り当てる．
	  vector<TpgFault> new_group{fault, };
	  g = new_fg_list.size();
	  new_fg_list.push_back(vector<TpgFault>{});
	  sig_dict.emplace(dbits, new_group);
	}
	else {
	  // キーに合致するグループ番号をとってくる．
	  g = sig_dict.at(dbits);
	}
	auto& new_fgroup = new_fg_list[g];
	new_fgroup.push_back(fault);
      }
      if ( singleton_drop ) {
      for ( auto& p: sig_dict ) {
	auto& group = p.second;
	if ( group.size() > 1 ) {
	  new_fg_list.push_back(group);
	}
      }
      for ( auto fault: fgroup ) {
	auto dbits = fsim.sppfp_diffbits(fault);
	auto& g = sig_dict.at(dbits);
	if ( g.size() == 1 ) {
	  // 故障シミュレーションの対象からも外す．
	  fsim.set_skip(fault);
	}
      }
    }
    std::swap(fg_list, new_fg_list);
  }

  auto time = timer.get_time();
  cout << "Fsim time: "
       << std::fixed << std::setprecision(2)
       << (time / 1000) << std::defaultfloat << endl;
  return fg_list;
}

#if 0
// @brief 故障を分類する．
vector<vector<TpgFault>>
Classifier2::run2(
  const vector<TestVector>& tv_list
)
{
  Timer timer;
  // 最初はすべての故障が一つのグループとなっている．
  vector<vector<TpgFault>> fg_list{fault_list, };
  for ( auto tv: tv_list ) {
    // fgmap を今回のシミュレーション結果で細分化する．
    // 今回未検出の故障のグループ番号は変わらない．
    unordered_map<SigKey, int, Hash, Eq> sig_dict;
    timer.start();
    auto fault_list1 = fsim.sppfp(tv);
    timer.stop();
    vector<vector<TpgFault>> new_fg_list;
    for ( auto fgroup: fg_list ) {
      std::unordered_map<DiffBits, vector<TpgFault>> sig_dict;
      for ( auto fault: fgroup ) {
	auto dbits = fsim.sppfp_diffbits(fault);
	if ( sig_dict.count(dbits) == 0 ) {
	  // 新しいグループを割り当てる．
	  vector<TpgFault> new_group{fault, };
	  sig_dict.emplace(dbits, new_group);
	}
	else {
	  // キーに合致するグループ番号をとってくる．
	  auto& group = sig_dict.at(dbits);
	  group.push_back(fault);
	}
      }
      for ( auto& p: sig_dict ) {
	auto& group = p.second;
	if ( group.size() > 1 ) {
	  new_fg_list.push_back(group);
	}
      }
      for ( auto fault: fgroup ) {
	auto dbits = fsim.sppfp_diffbits(fault);
	auto& g = sig_dict.at(dbits);
	if ( g.size() == 1 ) {
	  // 故障シミュレーションの対象からも外す．
	  fsim.set_skip(fault);
	}
      }
    }
    std::swap(fg_list, new_fg_list);
  }

  auto time = timer.get_time();
  cout << "Fsim time: "
       << std::fixed << std::setprecision(2)
       << (time / 1000) << std::defaultfloat << endl;
  return fg_list;
}
#endif

END_NAMESPACE_DRUID
