
/// @file Classifier.cc
/// @brief Classifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Classifier.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
Classifier::Classifier(
  const TpgNetwork& network,
  const vector<TpgFault>& fault_list,
  bool has_prev_state
) : mFaultList{fault_list}
{
  mFsim.initialize(network, has_prev_state, false);
  mFsim.set_fault_list(mFaultList);
  mMaxId = 0;
  for ( auto f: mFaultList ) {
    mMaxId = std::max(mMaxId, f.id());
  }
  ++ mMaxId;
}

// @brief 故障を分類する．
vector<vector<TpgFault>>
Classifier::run(
  const vector<TestVector>& tv_list
)
{
  // 最初はすべての故障が一つのグループとなっている．
  vector<vector<TpgFault>> fg_list{mFaultList};
  for ( auto tv: tv_list ) {
    auto fault_list1 = mFsim.sppfp(tv);
    vector<bool> fault_mark(mMaxId, false);
    for ( auto fault: fault_list1 ) {
      fault_mark[fault.id()] = true;
    }

    // fg_list を今回のシミュレーション結果で細分化する．
    vector<vector<TpgFault>> new_fg_list;
    for ( auto& fg: fg_list ) {
      unordered_map<DiffBits, vector<TpgFault>> fg_dict;
      vector<TpgFault> undet_faults;
      for ( auto& f: fg ) {
	if ( fault_mark[f.id()] ) {
	  auto dbits = mFsim.sppfp_diffbits(f);
	  if ( fg_dict.count(dbits) == 0 ) {
	    fg_dict.emplace(dbits, vector<TpgFault>{});
	  }
	  fg_dict.at(dbits).push_back(f);
	}
	else {
	  undet_faults.push_back(f);
	}
      }
      for ( auto& p: fg_dict ) {
	auto& fg1 = p.second;
	if ( fg1.size() == 1 ) {
	  // singleton は除外する．
	  continue;
	}
	new_fg_list.push_back(fg1);
      }
      if ( undet_faults.size() >= 2 ) {
	new_fg_list.push_back(undet_faults);
      }
    }
    std::swap(fg_list, new_fg_list);
  }

  return fg_list;
}

BEGIN_NONAMESPACE

struct Hash {
  SizeType
  operator()(
    const pair<DiffBits, int>& key
  ) const
  {
    return key.first.hash() ^ key.second;
  }
};

struct Eq {
  bool
  operator()(
    const pair<DiffBits, int>& key1,
    const pair<DiffBits, int>& key2
  ) const
  {
    return key1.first == key2.first && key1.second == key2.second;
  }
};

END_NONAMESPACE

// @brief 故障を分類する．
vector<vector<TpgFault>>
Classifier::run2(
  const vector<TestVector>& tv_list
)
{
  // 最初はすべての故障が一つのグループとなっている．
  vector<int> fgmap(mMaxId, 0);
  for ( auto tv: tv_list ) {
    auto fault_list1 = mFsim.sppfp(tv);
    vector<bool> fault_mark(mMaxId, false);
    for ( auto fault: fault_list1 ) {
      fault_mark[fault.id()] = true;
    }

    // fgmap を今回のシミュレーション結果で細分化する．
    vector<int> new_fgmap(mMaxId, 0);
    vector<SizeType> count;
    unordered_map<pair<DiffBits, int>, int, Hash, Eq> sig_dict;
    for ( auto& f: mFaultList ) {
      if ( fgmap[f.id()] == -1 ) {
	new_fgmap[f.id()] = -1;
	continue;
      }
      DiffBits dbits{0};
      if ( fault_mark[f.id()] ) {
	dbits = mFsim.sppfp_diffbits(f);
      }
      pair<DiffBits, int> key{dbits, fgmap[f.id()]};
      if ( sig_dict.count(key) == 0 ) {
	int g = count.size();
	sig_dict.emplace(key, g);
	new_fgmap[f.id()] = g;
	count.push_back(1);
      }
      else {
	int g = sig_dict.at(key);
	new_fgmap[f.id()] = g;
	++ count[g];
      }
    }
    // singleton を除外する．
    SizeType ng = count.size();
    for ( auto& f: mFaultList ) {
      int g = new_fgmap[f.id()];
      if ( count[g] <= 1 ) {
	new_fgmap[f.id()] = -1;
	mFsim.set_skip(f);
      }
    }
    std::swap(fgmap, new_fgmap);
  }

  unordered_map<int, int> gmap;
  int last_g = 0;
  for ( auto g: fgmap ) {
    if ( g == -1 ) {
      continue;
    }
    gmap.emplace(g, last_g);
    ++ last_g;
  }

  vector<vector<TpgFault>> fg_list(last_g);
  for ( auto& f: mFaultList ) {
    auto g = fgmap[f.id()];
    if ( g == -1 ) {
      continue;
    }
    auto new_g = gmap.at(g);
    fg_list[new_g].push_back(f);
  }
  return fg_list;
}

END_NAMESPACE_DRUID
