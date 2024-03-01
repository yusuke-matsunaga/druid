
/// @file Classifier2.cc
/// @brief Classifier2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Classifier2.h"
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

END_NONAMESPACE

// @brief 故障を分類する．
vector<vector<TpgFault>>
Classifier2::run(
  const TpgNetwork& network,
  const vector<TpgFault>& fault_list,
  FaultType fault_type,
  const vector<TestVector>& tv_list,
  bool ppsfp,
  bool multi
)
{
  Fsim fsim;
  fsim.initialize(network, fault_type, false, multi);
  fsim.set_fault_list(fault_list);
  SizeType max_id = 0;
  for ( auto& f: fault_list ) {
    max_id = std::max(max_id, f.id());
  }
  ++ max_id;

  vector<TpgFault> tmp_fault_list{fault_list};

  Timer timer;
  auto NTV = tv_list.size();
  vector<vector<vector<SizeType>>> fg_list_array(NTV);
  if ( ppsfp ) {
    vector<TestVector> tv_buff;
    tv_buff.reserve(PV_BITLEN);
    SizeType base = 0;
    for ( SizeType t_id = 0; t_id < NTV; ++ t_id ) {
      auto& tv = tv_list[t_id];
      tv_buff.push_back(tv);
      if ( tv_buff.size() == PV_BITLEN || t_id == NTV - 1 ) {
	unordered_map<DiffBits, SizeType> group_dict[PV_BITLEN];
	timer.start();
	fsim.ppsfp(tv_buff,
		   [&](
		     const TpgFault& fault,
		     const DiffBitsArray& dbits_array
		   )
		   {
		     auto fid = fault.id();
		     for ( SizeType b = 0; b < tv_buff.size(); ++ b ) {
		       auto dbits = dbits_array.get_slice(b);
		       if ( dbits.elem_num() == 0 ) {
			 continue;
		       }
		       auto& fg_list = fg_list_array[base + b];
		       SizeType g;
		       if ( group_dict[b].count(dbits) == 0 ) {
			 // 新しいグループ番号を割り当てる．
			 g = fg_list.size();
			 group_dict[b].emplace(dbits, g);
			 fg_list.push_back({});
		       }
		       else {
			 // キーに合致するグループ番号をとってくる．
			 g = group_dict[b].at(dbits);
		       }
		       fg_list[g].push_back(fid);
		     }
		   });
	timer.stop();
	base += tv_buff.size();
	tv_buff.clear();
      }
    }
  }
  else {
    for ( SizeType t_id = 0; t_id < NTV; ++ t_id ) {
      auto& tv = tv_list[t_id];
      // DiffBits をキーとしてグループ番号を格納する辞書
      unordered_map<DiffBits, SizeType> group_dict;
      auto& fg_list = fg_list_array[t_id];
      timer.start();
      fsim.sppfp(tv,
		 [&](
		   const TpgFault& fault,
		   const DiffBits& dbits
		 )
		 {
		   auto fid = fault.id();
		   SizeType g;
		   if ( group_dict.count(dbits) == 0 ) {
		     // 新しいグループ番号を割り当てる．
		     g = fg_list.size();
		     group_dict.emplace(dbits, g);
		     fg_list.push_back({});
		   }
		   else {
		     // キーに合致するグループ番号をとってくる．
		     g = group_dict.at(dbits);
		   }
		   fg_list[g].push_back(fid);
		 });
      timer.stop();
    }
  }

  // 最初はすべての故障が一つのグループとなっている．
  vector<SizeType> fgmap(max_id, 0);
  SizeType next_id = 1;
  for ( auto& fg_list: fg_list_array ) {
    // パタンごとのグループで細分化する．
    for ( auto& fg: fg_list ) {
      unordered_map<SizeType, SizeType> gid_dict;
      for ( auto fid: fg ) {
	SizeType old_id = fgmap[fid];
	SizeType new_id;
	if ( gid_dict.count(old_id) > 0 ) {
	  new_id = gid_dict.at(old_id);
	}
	else {
	  new_id = next_id;
	  gid_dict.emplace(old_id, new_id);
	  ++ next_id;
	}
	fgmap[fid] = new_id;
      }
    }
  }

  // 各グループの要素数を数える．
  unordered_map<SizeType, SizeType> count_dict;
  for ( auto& f: fault_list ) {
    SizeType fid = f.id();
    SizeType gid = fgmap[fid];
    if ( count_dict.count(gid) == 0 ) {
      count_dict.emplace(gid, 1);
    }
    else {
      ++ count_dict[gid];
    }
  }
  // 要素数が2以上のグループを対象に番号を付け替える．
  unordered_map<SizeType, SizeType> gmap;
  SizeType last_id = 0;
  for ( auto& p: count_dict ) {
    SizeType gid = p.first;
    SizeType count = p.second;
    if ( count >= 2 ) {
      SizeType new_id = last_id;
      ++ last_id;
      gmap.emplace(gid, new_id);
    }
  }

  // グループ番号に基づいてリストを作る．
  vector<vector<TpgFault>> fg_list(last_id);
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
