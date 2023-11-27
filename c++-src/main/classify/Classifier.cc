
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
Classifier::run(
  const vector<TestVector>& tv_list
)
{
  // 最初はすべての故障が一つのグループとなっている．
  vector<int> fgmap(mMaxId, 0);
  vector<SizeType> count{mFaultList.size()};
  for ( auto tv: tv_list ) {
    // fgmap を今回のシミュレーション結果で細分化する．
    // 今回未検出の故障のグループ番号は変わらない．
    unordered_map<pair<DiffBits, int>, int, Hash, Eq> sig_dict;
    auto fault_list1 = mFsim.sppfp(tv);
    for ( auto fault: fault_list1 ) {
      auto dbits = mFsim.sppfp_diffbits(fault);
      auto fid = fault.id();
      pair<DiffBits, int> key{dbits, fgmap[fid]};
      int g;
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
      int old_g = fgmap[fid];
      fgmap[fid] = g;
      -- count[old_g];
      ++ count[g];
    }

    // singleton を除外する．
    for ( auto& f: mFaultList ) {
      auto fid = f.id();
      int g = fgmap[fid];
      if ( count[g] <= 1 ) {
	fgmap[fid] = -1;
	// 故障シミュレーションの対象からも外す．
	mFsim.set_skip(f);
      }
    }
  }

  // このままでは singleton になって削除された
  // グループ番号が抜けているので
  // 隙間なしに付け替える．
  unordered_map<int, int> gmap;
  int last_g = 0;
  for ( SizeType g = 0; g < count.size(); ++ g ) {
    if ( count[g] >= 2 ) {
      gmap.emplace(g, last_g);
      ++ last_g;
    }
  }

  // グループ番号に基づいてリストを作る．
  vector<vector<TpgFault>> fg_list(last_g);
  for ( auto& f: mFaultList ) {
    auto g = fgmap[f.id()];
    if ( g >= 0 ) {
      auto new_g = gmap.at(g);
      fg_list[new_g].push_back(f);
    }
  }
  return fg_list;
}

END_NAMESPACE_DRUID
