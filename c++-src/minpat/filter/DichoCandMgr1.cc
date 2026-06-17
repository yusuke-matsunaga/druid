
/// @file DichoCandMgr1.cc
/// @brief DichoCandMgr1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DichoCandMgr1.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CandMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<CandMgr>
CandMgr::new_dichotomy_mgr(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  return std::unique_ptr<CandMgr>{new DichoCandMgr1(fault_list)};
}


//////////////////////////////////////////////////////////////////////
// クラス DichoCandMgr1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DichoCandMgr1::DichoCandMgr1(
  const TpgFaultList& fault_list
) : CandMgr(fault_list)
{
  // 最初は１つのグループ
  auto group = new Group(0, fault_list);
  mCurGroupList.push_back(std::unique_ptr<Group>{group});
}

// @brief デストラクタ
DichoCandMgr1::~DichoCandMgr1()
{
}

// @brief 更新処理
bool
DichoCandMgr1::update(
  const std::vector<PackedVal>& dpat_array
)
{
  // パタンをキーしたGroupの辞書
  using GroupDict = std::unordered_map<PackedVal, Group*>;

  // パタン間の順序関係を表すグラフ
  DPatGraph dpat_graph(dpat_array);

  // 重複のないパタンのリスト
  auto all_dpat_list = dpat_graph.pat_list();

  // もとのグループ数
  auto ng = mCurGroupList.size();

  // グループごとのSubGroupInfoのリストの配列
  std::vector<std::vector<SubGroupInfo>> sg_list_array(ng);

  // 細分化したグループのリスト
  std::vector<std::unique_ptr<Group>> new_group_list;

  // 細分化したサブグループを作る．
  for ( auto& group: mCurGroupList ) {
    // group の故障をパタンごとに分ける．
    GroupDict group_dict;
    auto& sg_list = sg_list_array[group->id()];
    for ( auto fault: group->fault_list() ) {
      auto dpat = dpat_array[fault.id()];
      if ( group_dict.count(dpat) == 0 ) {
	// 新しいグループを作る．
	auto id = new_group_list.size();
	auto new_group = new Group(id);
	new_group->add_fault(fault);
	new_group_list.push_back(std::unique_ptr<Group>{new_group});
	group_dict.emplace(dpat, new_group);
	sg_list.push_back({dpat, new_group});
      }
      else {
	auto group = group_dict.at(dpat);
	group->add_fault(fault);
      }
    }
  }

  // 変化があったことを示すフラグ
  bool changed = new_group_list.size() != ng;

  // dominance list を作る．
  for ( auto& group: mCurGroupList ) {
    auto& succ_list = group->succ_list();
    auto& sg_list = sg_list_array[group->id()];
    for ( auto& sg: sg_list ) {
      auto dpat = sg.dpat;
      auto subgroup = sg.group;
      std::vector<Group*> sub_succ_list;
#if 0
      for ( auto succ_group: succ_list ) {
	auto& sg_list1 = sg_list_array[succ_group->id()];
	for ( auto& sg1: sg_list1 ) {
	  auto dpat1 = sg1.dpat;
	  if ( (dpat & dpat1) == dpat ) {
	    auto subgroup1 = sg1.group;
	    sub_succ_list.push_back(subgroup1);
	  }
	}
      }
#else
      {
	std::vector<PackedVal> block_pats;
	block_pats.reserve(sg_list.size() - 1);
	for ( auto sg: sg_list ) {
	  auto pat1 = sg.pat;
	  if ( pat1 != dpat ) {
	    block_pats.push_back(pat1);
	  }
	}
	std::vector<PackedVal> medial_pats;
	std::vector<PackedVal> boundary_pats;
	dpat_graph.traverse(dpat, block_pats, medial_pats, boundary_pats);
	for ( auto pat1: boundary_pats ) {
	  // pat1 に対応するサブグループを sub_succ_list に追加する．
	}
	for ( auto succ_group: succ_list ) {

	}
      }
#endif
      subgroup->set_succ_list(std::move(sub_succ_list));
    }
    if ( !changed && sg_list.size() == 1 ) {
      // 後続リストの要素数が変化しているか調べる．
      auto& subgroup = sg_list.front().group;
      if ( group->succ_list().size() != subgroup->succ_list().size() ) {
	changed = true;
      }
    }
  }

  // 変化があったら更新する．
  if ( changed ) {
    std::swap(mCurGroupList, new_group_list);
    return true;
  }

  return false;
}

// @brief 後続のリストを求める．
std::vector<Group*>
DichoCandMgr1::get_succ_list(
  const SubGroupInfo& sg,
  const std::vector<std::vector<SubGroupInfo>>& sg_list_array,
  const DPatGraph& dpat_graph
)
{
  auto dpat = sg.dpat;
  auto group = sg.group;

}

// @brief 終了処理
std::unique_ptr<EqDomCand>
DichoCandMgr1::end(
  bool reduce
) const
{
  // fault_list() の先頭の故障番号の昇順でソートする．
  // mCurGroupList は変更できないので tmp_list にコピーする．
  std::vector<Group*> tmp_list;
  tmp_list.reserve(mCurGroupList.size());
  for ( auto& group: mCurGroupList ) {
    tmp_list.push_back(group.get());
  }
  std::sort(tmp_list.begin(), tmp_list.end(),
	    [](Group* a,
	       Group* b) -> bool {
	      auto f1 = a->fault_list()[0];
	      auto f2 = b->fault_list()[0];
	      return f1.id() < f2.id();
	    });

  // グループ番号を付け直す．
  std::unordered_map<SizeType, SizeType> id_map;
  auto ng = tmp_list.size();
  for ( SizeType id = 0; id < ng; ++ id ) {
    auto group = tmp_list[id];
    id_map.emplace(group->id(), id);
  }

  // 故障グループを作る．
  std::vector<TpgFaultList> group_list;
  group_list.reserve(ng);
  for ( auto group: tmp_list ) {
    group_list.push_back(group->fault_list());
  }

  // 順序関係を表すペアのリストを作る．
  std::vector<std::pair<SizeType, SizeType>> succ_pair_list;
  for ( auto group1: tmp_list ) {
    auto id1 = id_map.at(group1->id());
    auto& succ_list = group1->succ_list();
    for ( auto group2: succ_list ) {
      auto id2 = id_map.at(group2->id());
      if ( id2 != id1 ) {
	succ_pair_list.push_back({id1, id2});
      }
    }
  }
  return std::unique_ptr<EqDomCand>{new EqDomCand(group_list, succ_pair_list, reduce)};
}

// @brief 故障グループの情報を出力する．
void
DichoCandMgr1::print_group_list(
  std::ostream& s,
  const std::vector<std::unique_ptr<Group>>& group_list
)
{
  s << "-------------------------------------------" << std::endl;
  for ( auto& group: group_list ) {
    print_group(s, group.get());
  }
  s << "-------------------------------------------" << std::endl;
}

// @brief 故障グループの情報を出力する．
void
DichoCandMgr1::print_group(
  std::ostream& s,
  Group* group
)
{
  s << "[G#" << group->id() << "]:";
  if ( group->fault_list().empty() ) {
    s << "***";
  }
  else {
    for ( auto fault: group->fault_list() ) {
      s << " " << fault.str();
    }
  }
  s << std::endl
    << "  ==> ";
  auto& succ_list = group->succ_list();
  const char* comma = "";
  for ( auto group: succ_list ) {
    s << comma << "[G#" << group->id() << "]";
    comma = ", ";
    if ( !group->fault_list().empty() ) {
      s << "(" << group->fault_list()[0].str() << ")";
    };
  }
  s << std::endl;
}

// @brief パタンを文字列にする．
std::string
DichoCandMgr1::pat_str(
  PackedVal pat
)
{
  std::ostringstream buf;
  buf << "[" << std::hex << pat << std::dec << "]";
  return buf.str();
}

// @brief パタンのリストを文字列にする．
std::string
DichoCandMgr1::pat_list_str(
  const std::vector<PackedVal>& pat_list
)
{
  std::ostringstream buf;
  buf << "[" << std::hex;
  for ( auto pat: pat_list ) {
    buf << pat;
  }
  buf << std::dec << "]";
  return buf.str();
}

END_NAMESPACE_DRUID
