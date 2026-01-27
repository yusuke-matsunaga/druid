
/// @file FaultRep.cc
/// @brief FaultRep とその継承クラスの実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "FaultRep.h"
#include "GateRep.h"
#include "NodeRep.h"
#include "types/FaultType.h"
#include "types/TpgNode.h"
#include "types/Assign.h"
#include "types/Fval2.h"
#include "types/Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FaultRep
//////////////////////////////////////////////////////////////////////

// @brief ステムの故障の時 true を返す．
bool
FaultRep::is_stem() const
{
  return true;
}

// @brief ブランチの故障の時の入力位置を返す．
SizeType
FaultRep::branch_pos() const
{
  throw std::logic_error{"Not a branch fault"};
}

// @brief 網羅故障の場合の入力値のベクトルを返す．
std::vector<bool>
FaultRep::input_vals() const
{
  throw std::logic_error{"Not a exhausitve fault"};
}

// @brief origin_node を含む FFR の根のノードを返す．
const NodeRep*
FaultRep::ffr_root() const
{
  return origin_node()->ffr_root();
}

// @brief 故障が励起してFFRの根まで伝搬する条件を求める．
std::vector<SizeType>
FaultRep::ffr_propagate_condition() const
{
  // ノードの出力に故障が現れる励起条件
  auto assign_list = excitation_condition();

  // FFR の根までの伝搬条件を作る．
  for ( auto node = origin_node(); node->fanout_num() == 1; ) {
    auto fonode = node->fanout(0);
    auto val = fonode->nval();
    if ( val != Val3::_X ) {
      bool bval = val == Val3::_1 ? true : false;
      for ( auto inode: fonode->fanin_list() ) {
	if ( inode != node ) {
	  assign_list.push_back(Assign::encode(inode->id(), 1, bval));
	}
      }
    }
    node = fonode;
  }
  return assign_list;
}

// @brief ハッシュ用の値を返す．
SizeType
FaultRep::hash() const
{
  return id();
}

END_NAMESPACE_DRUID
