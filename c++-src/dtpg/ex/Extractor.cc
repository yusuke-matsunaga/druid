
/// @file Extractor.cc
/// @brief Extractor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Extractor.h"
#include "ExtSimple.h"
#include "ExData.h"


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
    std::ostringstream buf;
    buf << mode << ": unknown value for 'extractor'";
    throw std::invalid_argument{buf.str()};
  }

  // 文字列型ではなかった．
  std::ostringstream buf;
  buf << "value for 'extractor' should be a string or null";
  throw std::invalid_argument{buf.str()};
}

// @brief 値割り当てを１つ求める．
AssignList
Extractor::operator()(
  const TpgNode& root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
)
{
  ExData data{root, gvar_map, fvar_map, model};

  // 故障差の伝搬している経路を探す．
  ASSERT_COND( !data.sensitized_output_list().empty() );

  AssignList min_assign_list;
  SizeType min_val = std::numeric_limits<SizeType>::max();
  for ( auto po: data.sensitized_output_list() ) {
    std::vector<std::vector<TpgNode>> choice_list;
    auto node_list = data.backtrace(po, choice_list);
    auto cnode_list = select_cnode(choice_list);
    node_list.insert(node_list.end(), cnode_list.begin(), cnode_list.end());
    // AssignList に変換する．
    AssignList assign_list;
    for ( auto& node: node_list ) {
      auto bval = (data.gval(node) == Val3::_1);
      assign_list.add(node, 1, bval);
    }
    SizeType val = assign_list.size();
    if ( min_val > val ) {
      min_val = val;
      min_assign_list = assign_list;
    }
  }
  return min_assign_list;
}

// @brief 値割り当てを１つ求める．
AssignList
Extractor::operator()(
  const TpgNode& root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const TpgNode& output,
  const SatModel& model
)
{
  ExData data{root, gvar_map, fvar_map, model};

  // 故障差の伝搬している経路を探す．
  if ( data.sensitized_output_list().empty() ) {
    throw std::logic_error{"sensitized_output_list is empty"};
  }

  AssignList min_assign_list;
  SizeType min_val = std::numeric_limits<SizeType>::max();
  {
    std::vector<std::vector<TpgNode>> choice_list;
    auto node_list = data.backtrace(output, choice_list);
    auto cnode_list = select_cnode(choice_list);
    node_list.insert(node_list.end(), cnode_list.begin(), cnode_list.end());
    // AssignList に変換する．
    AssignList assign_list;
    for ( auto& node: node_list ) {
      auto bval = (data.gval(node) == Val3::_1);
      assign_list.add(node, 1, bval);
    }
    SizeType val = assign_list.size();
    if ( min_val > val ) {
      min_val = val;
      min_assign_list = assign_list;
    }
  }
  return min_assign_list;
}

END_NAMESPACE_DRUID
