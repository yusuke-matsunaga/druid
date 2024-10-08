
/// @file Extractor.cc
/// @brief Extractor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Extractor.h"
#include "ExtSimple.h"
#include "PropData.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
int debug = 0;
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Extractor
//////////////////////////////////////////////////////////////////////

// Extractor の継承クラスを作る．
Extractor*
Extractor::new_impl(
  const JsonValue& option
)
{
  if ( option.is_null() ) {
    // デフォルトフォールバック
    return new ExtSimple;
  }
  if ( option.is_string() ) {
    auto mode = option.get_string();
    if ( mode == "simple" ) {
      return new ExtSimple;
    }
    // 知らない型だった．
    ostringstream buf;
    buf << mode << ": unknown value for 'extractor'";
    throw std::invalid_argument{buf.str()};
  }

  // 文字列型ではなかった．
  ostringstream buf;
  buf << "value for 'extractor' should be a string or null";
  throw std::invalid_argument{buf.str()};
  // ダミー
  return nullptr;
}

// @brief 値割り当てを１つ求める．
AssignList
Extractor::operator()(
  const TpgNode* root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
)
{
  PropData data{root, gvar_map, fvar_map, model};

  AssignList min_assign_list;
  SizeType min_val = std::numeric_limits<SizeType>::max();
  for ( auto& boundary_data: data.boundary_data_list() ) {
    AssignList assign_list;
    for ( auto node: boundary_data.mNodeList ) {
      auto& assign_data = data.assign_data(node);
      if ( assign_data.mAndCond ) {
	for ( auto& assign: assign_data.mAssignList ) {
	  assign_list.add(assign);
	}
      }
      else {
	auto& assign = assign_data.mAssignList.front();
	assign_list.add(assign);
      }
    }
#if 0
    auto cnode_list = select_cnode(choice_list);
    for ( auto cnode: cnode_list ) {
      assign_list.add(data.get_assign(cnode));
    }
#endif
    SizeType val = assign_list.size();
    if ( min_val > val ) {
      min_val = val;
      min_assign_list = assign_list;
    }
  }
  return min_assign_list;
}

END_NAMESPACE_DRUID
