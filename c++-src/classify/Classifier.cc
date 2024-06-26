
/// @file Classifier.cc
/// @brief Classifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Classifier.h"
#include "TestVector.h"
#include "TpgFault.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

bool verbose = false;

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

using SigKey2 = pair<DiffBitsArray, SizeType>;

struct Hash2 {
  SizeType
  operator()(
    const SigKey2& key
  ) const
  {
    return key.first.hash() ^ key.second;
  }
};

struct Eq2 {
  bool
  operator()(
    const SigKey2& key1,
    const SigKey2& key2
  ) const
  {
    return key1.first == key2.first && key1.second == key2.second;
  }
};

// @brief 故障を分類する．
vector<vector<const TpgFault*>>
run_sppfp(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list,
  bool singleton_drop,
  bool multi
)
{
  Fsim fsim{network, fault_list, false, multi};
  SizeType max_id = 0;
  for ( auto& f: fault_list ) {
    max_id = std::max(max_id, f->id());
  }
  ++ max_id;

  vector<const TpgFault*> tmp_fault_list{fault_list};

  Timer timer;
  Timer fsim_timer;

  timer.start();
  // 最初はすべての故障が一つのグループとなっている．
  vector<SizeType> fgmap(max_id, 0);
  vector<SizeType> count{fault_list.size()};
  for ( auto tv: tv_list ) {
    // fgmap を今回のシミュレーション結果で細分化する．
    // 今回未検出の故障のグループ番号は変わらない．
    unordered_map<SigKey, SizeType, Hash, Eq> sig_dict;
    fsim_timer.start();
    fsim.sppfp(tv,
	       [&](
		 const TpgFault* fault,
		 const DiffBits& dbits
	       )
	       {
		 auto fid = fault->id();
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
    fsim_timer.stop();

    // singleton を除外する．
    if ( singleton_drop ) {
      vector<const TpgFault*> new_list;
      new_list.reserve(tmp_fault_list.size());
      for ( auto f: tmp_fault_list ) {
	auto fid = f->id();
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
  vector<vector<const TpgFault*>> fg_list(last_g);
  for ( auto f: fault_list ) {
    auto g = fgmap[f->id()];
    if ( gmap.count(g) > 0 ) {
      auto new_g = gmap.at(g);
      fg_list[new_g].push_back(f);
    }
  }
  timer.stop();

  if ( verbose ) {
    auto time = timer.get_time();
    cout << "total time: "
	 << std::fixed << std::setprecision(2)
	 << (time / 1000) << std::defaultfloat << endl;
    auto fsim_time = timer.get_time();
    cout << "Fsim time: "
	 << std::fixed << std::setprecision(2)
	 << (fsim_time / 1000) << std::defaultfloat << endl;
  }
  return fg_list;
}

// @brief 故障を分類する．
vector<vector<const TpgFault*>>
run_ppsfp(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list,
  bool singleton_drop,
  bool multi
)
{
  Fsim fsim{network, fault_list, false, multi};
  SizeType max_id = 0;
  for ( auto& f: fault_list ) {
    max_id = std::max(max_id, f->id());
  }
  ++ max_id;

  vector<const TpgFault*> tmp_fault_list{fault_list};

  Timer timer;
  Timer fsim_timer;

  timer.start();
  vector<TestVector> tv_buff;
  tv_buff.reserve(PV_BITLEN);
  // 最初はすべての故障が一つのグループとなっている．
  vector<SizeType> fgmap(max_id, 0);
  vector<SizeType> count{fault_list.size()};
  auto NTV = tv_list.size();
  SizeType base = 0;
  for ( auto tv: tv_list ) {
    tv_buff.push_back(tv);
    SizeType buff_size = tv_buff.size();
    if ( buff_size == PV_BITLEN || base + buff_size == NTV ) {
      // fgmap を今回のシミュレーション結果で細分化する．
      // 今回未検出の故障のグループ番号は変わらない．
      unordered_map<SigKey2, SizeType, Hash2, Eq2> sig_dict;
      fsim_timer.start();
      fsim.ppsfp(tv_buff,
		 [&](
		   const TpgFault* fault,
		   const DiffBitsArray& dbits_array
		 )
		 {
		   auto fid = fault->id();
		   SigKey2 key{dbits_array, fgmap[fid]};
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
      fsim_timer.stop();
      base += buff_size;
      tv_buff.clear();

      // singleton を除外する．
      if ( singleton_drop ) {
	vector<const TpgFault*> new_list;
	new_list.reserve(tmp_fault_list.size());
	for ( auto f: tmp_fault_list ) {
	  auto fid = f->id();
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
  vector<vector<const TpgFault*>> fg_list(last_g);
  for ( auto f: fault_list ) {
    auto g = fgmap[f->id()];
    if ( gmap.count(g) > 0 ) {
      auto new_g = gmap.at(g);
      fg_list[new_g].push_back(f);
    }
  }

  timer.stop();
  if ( verbose ) {
    auto time = timer.get_time();
    auto fsim_time = fsim_timer.get_time();
    cout << "Total time: "
	 << std::fixed << std::setprecision(2)
	 << (time / 1000) << std::defaultfloat << endl;
    cout << "Fsim time: "
	 << std::fixed << std::setprecision(2)
	 << (fsim_time / 1000) << std::defaultfloat << endl;
  }
  return fg_list;
}

END_NONAMESPACE


// @brief 故障を分類する．
vector<vector<const TpgFault*>>
Classifier::run(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list,
  bool singleton_drop,
  bool ppsfp,
  bool multi
)
{
  if ( ppsfp ) {
    return run_ppsfp(network, fault_list, tv_list, singleton_drop, multi);
  }
  else {
    return run_sppfp(network, fault_list, tv_list, singleton_drop, multi);
  }
}

void
Classifier::set_verbose(
  bool flag
)
{
  verbose = flag;
}

END_NAMESPACE_DRUID
