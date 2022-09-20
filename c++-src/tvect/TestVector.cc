
/// @file TestVector.cc
/// @brief TestVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TestVector.h"
#include "TpgNode.h"
#include "NodeValList.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

// @brief 割当リストからTestVectorを作るクラスメソッド
TestVector
TestVector::new_from_assign_list(
  SizeType input_num,
  SizeType dff_num,
  FaultType fault_type,
  const NodeValList& assign_list
)
{
  TestVector tv(input_num, dff_num, fault_type);

  for ( auto nv: assign_list ) {
    const TpgNode* node = nv.node();
    ASSERT_COND( node->is_ppi() );

    Val3 val = nv.val() ? Val3::_1 : Val3::_0;

    if ( fault_type == FaultType::StuckAt ) {
      tv.set_ppi_val(node->input_id(), val);
    }
    else if ( fault_type == FaultType::TransitionDelay ) {
      int time = nv.time();
      if ( time == 0 ) {
	tv.set_ppi_val(node->input_id(), val);
      }
      else {
	ASSERT_COND( node->is_primary_input() );

	tv.set_aux_input_val(node->input_id(), val);
      }
    }
  }

  return tv;
}

// @brief 複数のテストベクタをマージする．
TestVector
merge(
  const vector<TestVector>& tv_list
)
{
  SizeType n = tv_list.size();
  if ( n == 0 ) {
    return TestVector();
  }

  TestVector ans(tv_list[0]);
  for ( int i: Range(1, n) ) {
    ans &= tv_list[i];
  }
  return ans;
}

END_NAMESPACE_DRUID
