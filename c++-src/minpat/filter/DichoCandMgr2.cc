
/// @file DichoCandMgr2.cc
/// @brief DichoCandMgr2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"
#include "DichoCandMgr2.h"
#include "DPatGraph.h"

#define DEBUG 0

BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CandMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<CandMgr>
CandMgr::new_dichotomy_mgr2(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  return std::unique_ptr<CandMgr>{new DichoCandMgr2(fault_list)};
}


//////////////////////////////////////////////////////////////////////
// クラス DichoCandMgr2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DichoCandMgr2::DichoCandMgr2(
  const TpgFaultList& fault_list
) : CandMgr(fault_list)
{
  // 最初は１つのグループ
  auto group = new Group(0, fault_list);
  group->set_succ_list({group});
  mCurGroupList.push_back(std::unique_ptr<Group>{group});
}

// @brief デストラクタ
DichoCandMgr2::~DichoCandMgr2()
{
}

// @brief 更新処理
bool
DichoCandMgr2::update(
  const std::vector<PackedVal>& dpat_array
)
{
  // 細分化したサブグループの情報
  struct SubGroupInfo {
    PackedVal dpat;
    Group* group;
  };

  // パタンをキーしたGroupの辞書
  using GroupDict = std::unordered_map<PackedVal, Group*>;

  // もとのグループ数
  auto ng = mCurGroupList.size();

  // グループごとのパタンをキーにしたサブグループの辞書の配列
  std::vector<GroupDict> group_dict_array(ng);

  // グループごとのSubGroupInfoのリストの配列
  std::vector<std::vector<SubGroupInfo>> sg_list_array(ng);

  // 細分化したグループのリスト
  std::vector<std::unique_ptr<Group>> new_group_list;

  // 細分化したサブグループを作る．
  for ( auto& group: mCurGroupList ) {
    // group の故障をパタンごとに分ける．
    auto& group_dict = group_dict_array[group->id()];
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

  // グループごとの DPatGraph の配列
  std::vector<DPatGraph> dpat_graph_array(ng);
  for ( SizeType i = 0; i < ng; ++ i ) {
    auto& sg_list = sg_list_array[i];
    std::vector<PackedVal> pat_list;
    pat_list.reserve(sg_list.size());
    for ( auto& sg: sg_list ) {
      auto pat = sg.dpat;
      pat_list.push_back(pat);
    }
    dpat_graph_array[i].rebuild(pat_list);
  }

  // グループ間の順序関係を求める．
  for ( auto& group: mCurGroupList ) {
    auto& succ_list = group->succ_list();
    auto& sg_list = sg_list_array[group->id()];
    for ( auto& sg: sg_list ) {
      auto dpat = sg.dpat;
      auto subgroup = sg.group;
      std::vector<Group*> sub_succ_list;
      for ( auto succ_group: succ_list ) {
	auto& group_dict = group_dict_array[succ_group->id()];
	auto& dpat_graph1 = dpat_graph_array[succ_group->id()];
	for ( auto pat1: dpat_graph1.imm_succ_list(dpat) ) {
	  auto subgroup1 = group_dict.at(pat1);
	  sub_succ_list.push_back(subgroup1);
	}
      }
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
    std::swap(mCurGroupList, new_group_list );
    return true;
  }
  return false;
}

// @brief 終了処理
std::unique_ptr<EqDomCand>
DichoCandMgr2::end(
  bool reduce
) const
{
  // 故障リストを持つグループを抜き出す．
  std::vector<Group*> tmp_list;
  tmp_list.reserve(mCurGroupList.size());
  for ( auto& group: mCurGroupList ) {
    if ( group->fault_list().empty() ) {
      continue;
    }
    tmp_list.push_back(group.get());
  }
  // fault_list() の先頭の故障番号の昇順でソートする．
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

  // グループ番号を付け直す．
  std::vector<TpgFaultList> group_list;
  group_list.reserve(ng);
  for ( auto group: tmp_list ) {
    group_list.push_back(group->fault_list());
  }
  std::vector<std::pair<SizeType, SizeType>> dom_list;
  for ( auto group1: tmp_list ) {
    auto id1 = id_map.at(group1->id());
    auto& succ_list = group1->succ_list();
    for ( auto group2: succ_list ) {
      auto id2 = id_map.at(group2->id());
      if ( id2 != id1 ) {
	dom_list.push_back({id1, id2});
      }
    }
  }
  return std::unique_ptr<EqDomCand>{new EqDomCand(group_list, dom_list)};
}

// @brief 内容を出力する．
void
DichoCandMgr2::print(
  std::ostream& s
) const
{
  s << "-------------------------------------------" << std::endl;
  for ( auto& group: mCurGroupList ) {
    if ( group->fault_list().empty() ) {
      continue;
    }
    const char* spc = "";
    for ( auto fault: group->fault_list() ) {
      s << spc << fault.str();
      spc = " ";
    }
    s << std::endl;
    auto succ_list = group->succ_list();
    std::sort(succ_list.begin(), succ_list.end(),
	      [](Group* a, Group* b) -> bool {
		return a->id() < b->id();
	      });
    const char* comma = "";
    s << "  ==>";
    for ( auto group1: succ_list ) {
      s << comma << group1->fault_list()[0].str();
      comma = ", ";
    }
    s << std::endl;
  }
  s << "-------------------------------------------" << std::endl;
}

// @brief 故障グループの情報を出力する．
void
DichoCandMgr2::print_group_list(
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
DichoCandMgr2::print_group(
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
DichoCandMgr2::pat_str(
  PackedVal pat
)
{
  std::ostringstream buf;
  buf << "[" << std::hex << pat << std::dec << "]";
  return buf.str();
}

// @brief パタンのリストを文字列にする．
std::string
DichoCandMgr2::pat_list_str(
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
