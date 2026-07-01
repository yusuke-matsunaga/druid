
/// @file DichoMgr2.cc
/// @brief DichoMgr2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqGroupMgr.h"
#include "DichoMgr2.h"
#include "DCM2Helper.h"

#define DEBUG 0

BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス EqGroupMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<EqGroupMgr>
EqGroupMgr::new_dichotomy_mgr2(
  FaultInfo& fault_info,
  Fsim& fsim,
  const ConfigParam& option
)
{
  return std::unique_ptr<EqGroupMgr>{new DichoMgr2(fault_info, fsim, option)};
}


//////////////////////////////////////////////////////////////////////
// クラス DichoMgr2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DichoMgr2::DichoMgr2(
  FaultInfo& fault_info,
  Fsim& fsim,
  const ConfigParam& option
) : EqGroupMgr(fault_info, fsim, option)
{
  // 最初は１つのグループ
  auto group = new DichoGroup(0, fault_info.rep_fault_list());
  mCurGroupList.push_back(DichoGroup::Ptr{group});
}

// @brief デストラクタ
DichoMgr2::~DichoMgr2()
{
}

// @brief 更新処理
bool
DichoMgr2::update(
  const std::vector<PackedVal>& dpat_array
)
{
  DCM2Helper helper;
  auto r = helper.run(mCurGroupList, dpat_array);
  return r;
}

// @brief 終了処理
std::unique_ptr<EqDomCand>
DichoMgr2::end(
  bool reduce
) const
{
  // 故障リストを持つグループを抜き出す．
  std::vector<DichoGroup*> tmp_list;
  tmp_list.reserve(mCurGroupList.size());
  for ( auto& group: mCurGroupList ) {
    if ( group->fault_list().empty() ) {
      continue;
    }
    tmp_list.push_back(group.get());
  }
  // fault_list() の先頭の故障番号の昇順でソートする．
  std::sort(tmp_list.begin(), tmp_list.end(),
	    [](DichoGroup* a,
	       DichoGroup* b) -> bool {
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
  return std::unique_ptr<EqDomCand>{new EqDomCand(group_list, dom_list, reduce)};
}

// @brief 等価故障グループ数を返す．
SizeType
DichoMgr2::group_num() const
{
  return mCurGroupList.size();
}

// @brief 等価故障グループ番号を返す．
SizeType
DichoMgr2::group_id(
  const TpgFault& fault
) const
{
  return 0;
}

// @brief 等価故障グループの故障リストを返す．
TpgFaultList
DichoMgr2::fault_list(
  SizeType group_id
) const
{
  return TpgFaultList();
}

// @brief 後続グループ番号のリスト返す．
std::vector<SizeType>
DichoMgr2::succ_list(
  SizeType group_id
) const
{
  return {};
}

// @brief 先行グループ番号のリスト返す．
std::vector<SizeType>
DichoMgr2::prev_list(
  SizeType group_id
) const
{
  return {};
}

// @brief 順序関係の要素数を返す．
SizeType
DichoMgr2::domcand_num() const
{
  SizeType num = 0;
  for ( auto& group: mCurGroupList ) {
    num += group->succ_list().size();
  }
  return num;
}

// @brief 内容を出力する．
void
DichoMgr2::print(
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
	      [](DichoGroup* a,
		 DichoGroup* b) -> bool {
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
DichoMgr2::print_group_list(
  std::ostream& s,
  const std::vector<DichoGroup::Ptr>& group_list
)
{
  s << "-------------------------------------------" << std::endl;
  for ( auto& group: group_list ) {
    group->print(s);
  }
  s << "-------------------------------------------" << std::endl;
}

// @brief パタンを文字列にする．
std::string
DichoMgr2::pat_str(
  PackedVal pat
)
{
  std::ostringstream buf;
  buf << "[" << std::hex << pat << std::dec << "]";
  return buf.str();
}

// @brief パタンのリストを文字列にする．
std::string
DichoMgr2::pat_list_str(
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
