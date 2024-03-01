
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

// @brief 2進文字列からオブジェクトを作る．
TestVector
TestVector::from_bin(
  SizeType input_num,
  SizeType dff_num,
  bool has_prev_state,
  const string& bin_str
)
{
  TestVector tv{input_num, dff_num, has_prev_state};
  tv.mVector.set_from_bin(bin_str);
  return tv;
}

// @brief HEX文字列からオブジェクトを作る．
TestVector
TestVector::from_hex(
  SizeType input_num,
  SizeType dff_num,
  bool has_prev_state,
  const string& hex_str
)
{
  TestVector tv{input_num, dff_num, has_prev_state};
  tv.mVector.set_from_hex(hex_str);
  return tv;
}

// @brief 割当リストから値を設定する．
void
TestVector::set_from_assign_list(
  const NodeValList& assign_list
)
{
  for ( auto nv: assign_list ) {
    auto node = nv.node();
    ASSERT_COND( node->is_ppi() );

    auto val = nv.val() ? Val3::_1 : Val3::_0;

    if ( has_aux_input() ) {
      int time = nv.time();
      if ( time == 0 ) {
	set_ppi_val(node->input_id(), val);
      }
      else {
	ASSERT_COND( node->is_primary_input() );

	set_aux_input_val(node->input_id(), val);
      }
    }
    else {
      set_ppi_val(node->input_id(), val);
    }
  }
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
