
/// @file Classifier2.cc
/// @brief Classifier2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Classifier2.h"
#include "TestVector.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
Classifier2::Classifier2(
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
Classifier2::run(
  const vector<TestVector>& tv_list
)
{
  Timer timer;
  // 最初はすべての故障が一つのグループとなっている．
  vector<vector<TpgFault>> fg_list{mFaultList, };
  for ( auto tv: tv_list ) {
    // fgmap を今回のシミュレーション結果で細分化する．
    // 今回未検出の故障のグループ番号は変わらない．
    unordered_map<pair<DiffBits, int>, int, Hash, Eq> sig_dict;
    timer.start();
    auto fault_list1 = mFsim.sppfp(tv);
    timer.stop();
    vector<vector<TpgFault>> new_fg_list;
    for ( auto fgroup: fg_list ) {
      std::unordered_map<DiffBits, SizeType> sig_dict;
      for ( auto fault: fgroup ) {
	auto dbits = mFsim.sppfp_diffbits(fault);
	SizeType g;
	if ( sig_dict.count(dbits) == 0 ) {
	  // 新しいグループ番号を割り当てる．
	  g = new_fg_list.size();
	  new_fg_list.push_back(vector<TpgFault>{});
	  sig_dict.emplace(dbits, g);
	}
	else {
	  // キーに合致するグループ番号をとってくる．
	  g = sig_dict.at(dbits);
	}
	auto& new_fgroup = new_fg_list[g];
	new_fgroup.push_back(fault);
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

END_NAMESPACE_DRUID
