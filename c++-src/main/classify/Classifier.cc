
/// @file Classifier.cc
/// @brief Classifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Classifier.h"
#include "TestVector.h"
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
Classifier::run(
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
  SizeType max_id = 0;
  for ( auto& f: fault_list ) {
    max_id = std::max(max_id, f.id());
  }
  ++ max_id;

  vector<TpgFault> tmp_fault_list{fault_list};

  Timer timer;
  // 最初はすべての故障が一つのグループとなっている．
  vector<SizeType> fgmap(max_id, 0);
  vector<SizeType> count;
  for ( auto tv: tv_list ) {
    // fgmap を今回のシミュレーション結果で細分化する．
    // 今回未検出の故障のグループ番号は変わらない．
    unordered_map<SigKey, SizeType, Hash, Eq> sig_dict;
    timer.start();
    fsim.sppfp(tv,
	       [&](
		 SizeType _,
		 TpgFault fault,
		 DiffBits dbits
	       )
	       {
		 auto fid = fault.id();
		 SigKey key{dbits, fgmap[fid]};
		 SizeType g;
		 if ( sig_dict.count(key) == 0 ) {
		   // 新しいグループ番号を割り当てる．
		   g = count.size();
		   count.push_back(0);
		   sig_dict.emplace(key, g);
		 }
		 else {
		   // キーに合致するグループ番号をとってくる．
		   g = sig_dict.at(key);
		 }
		 // グループ番号を更新する．
		 auto old_g = fgmap[fid];
		 fgmap[fid] = g;
		 -- count[old_g];
		 ++ count[g];
	       });
    timer.stop();

    // singleton を除外する．
    if ( singleton_drop ) {
      vector<TpgFault> new_list;
      new_list.reserve(tmp_fault_list.size());
      for ( auto& f: tmp_fault_list ) {
	auto fid = f.id();
	SizeType g = fgmap[fid];
	if ( count[g] <= 1 ) {
	  // 故障シミュレーションの対象からも外す．
	  fsim.set_skip(f);
	}
	else {
	  new_list.push_back(f);
	}
      }
      if ( new_list.size() != tmp_fault_list.size() ) {
	std::swap(tmp_fault_list, new_list);
      }
    }
  }

  // このままでは singleton になって削除された
  // グループ番号が抜けているので
  // 隙間なしに付け替える．
  unordered_map<SizeType, SizeType> gmap;
  SizeType last_g = 0;
  for ( SizeType g = 0; g < count.size(); ++ g ) {
    if ( count[g] >= 2 ) {
      gmap.emplace(g, last_g);
      ++ last_g;
    }
  }

  // グループ番号に基づいてリストを作る．
  vector<vector<TpgFault>> fg_list(last_g);
  for ( auto& f: fault_list ) {
    auto g = fgmap[f.id()];
    if ( gmap.count(g) > 0 ) {
      auto new_g = gmap.at(g);
      fg_list[new_g].push_back(f);
    }
  }

  auto time = timer.get_time();
  cout << "Fsim time: "
       << std::fixed << std::setprecision(2)
       << (time / 1000) << std::defaultfloat << endl;
  return fg_list;
}

END_NAMESPACE_DRUID
