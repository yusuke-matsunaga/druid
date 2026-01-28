
/// @File NetworkRep.cc
/// @brief NetworkRep の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "NetworkRep.h"
#include "types/TpgGate.h"
#include "types/TpgNode.h"
#include "types/TpgFault.h"

#include "GateRep.h"
#include "GateType.h"
#include "NodeRep.h"

#include "types/Val3.h"

#include "ym/Expr.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス NetworkRep
//////////////////////////////////////////////////////////////////////

// @brief サイズを設定する．
void
NetworkRep::set_size(
  SizeType input_num,
  SizeType output_num,
  SizeType dff_num,
  SizeType gate_num,
  SizeType extra_node_num
)
{
  auto node_num = input_num + output_num + dff_num * 2 + gate_num + extra_node_num;
  auto nppi = input_num + dff_num;
  auto nppo = output_num + dff_num;

  mDffList.reserve(dff_num);
  mNodeArray.reserve(node_num);
  mGateArray.reserve(gate_num);
  mPPIArray.reserve(nppi);
  mPPOArray.reserve(nppo);
  mPPOArray2.reserve(nppo);
  mDffList.reserve(dff_num);
}

// @brief 入力ノードを生成する．
NodeRep*
NetworkRep::make_input_node(
  const std::string& name
)
{
  return new_ppi(
    name,
    [&](SizeType id, SizeType input_id) {
      return NodeRep::new_input(id, input_id);
    }
  );
}

// @brief DFFの出力ノードを生成する．
NodeRep*
NetworkRep::make_dff_output_node(
  const std::string& name
)
{
  auto dff_id = mDffList.size();
  auto node = new_ppi(
    name,
    [&](SizeType id, SizeType input_id) {
      return NodeRep::new_dff_output(id, input_id, dff_id);
    }
  );
  NodeRep* dummy = nullptr;
  mDffList.push_back(std::make_pair(dummy, node));
  return node;
}

// @brief 出力ノードを生成する．
NodeRep*
NetworkRep::make_output_node(
  const std::string& name,
  const NodeRep* inode
)
{
  return new_ppo(
    name,
    [&](SizeType id, SizeType oid) {
      return NodeRep::new_output(id, oid, inode);
    }
  );
}

// @brief DFFの入力ノードを生成する．
NodeRep*
NetworkRep::make_dff_input_node(
  SizeType dff_id,
  const std::string& name,
  const NodeRep* inode
)
{
  auto input_node = new_ppo(
    name,
    [&](SizeType id, SizeType output_id) {
      return NodeRep::new_dff_input(id, output_id, dff_id, inode);
    }
  );

  auto& dff_pair = mDffList[dff_id];
  if ( dff_pair.first != nullptr ) {
    throw std::logic_error{"input_node has been already set"};
  }
  dff_pair.first = input_node;
  auto output_node = dff_pair.second;
  input_node->set_alt_node(output_node);
  output_node->set_alt_node(input_node);

  return input_node;
}

// @brief 組み込み型の論理ゲートを生成する．
NodeRep*
NetworkRep::make_prim_node(
  PrimType type,
  const std::vector<const NodeRep*>& fanin_list
)
{
  auto node = new_node(
    [&](SizeType id) {
      return NodeRep::new_logic(id, type, fanin_list);
    }
  );
  return node;
}

// @brief 組込み型の GateType を生成し，登録する．
const GateType*
NetworkRep::make_gate_type(
  SizeType input_num,
  PrimType prim_type
)
{
  auto gate_type = new_gate_type(
    [&](SizeType id) {
      return GateType::new_primitive(id, input_num, prim_type);
    });
  return gate_type;
}

// @brief 複合型の GateType を生成し，登録する．
const GateType*
NetworkRep::make_gate_type(
  SizeType input_num,
  const Expr& expr
)
{
  auto gate_type = new_gate_type(
    [&](SizeType id) {
      return GateType::new_cplx(id, input_num, expr);
    });
  return gate_type;
}

// @brief ゲートを生成する．
GateRep*
NetworkRep::make_gate(
  const GateType* gate_type,
  const NodeRep* node,
  const std::vector<GateRep::BranchInfo>& branch_info
)
{
  if ( gate_type->is_ppi() ||
       gate_type->is_ppo() ||
       gate_type->is_primitive() ) {
    return new_gate(
      [&](SizeType id) {
	return GateRep::new_primitive(id, gate_type, node, mFaultType);
      });
  }
  return new_gate(
    [&](SizeType id) {
      return GateRep::new_cplx(id, gate_type, node, branch_info, mFaultType);
    });
}

// @brief PPI系のノードの生成と登録
NodeRep*
NetworkRep::new_ppi(
  const std::string& name,
  std::function<NodeRep*(SizeType id, SizeType input_id)> new_func
)
{
  auto iid = mPPIArray.size();
  auto node = new_node(
    [&](SizeType id) {
      return new_func(id, iid);
    }
  );
  mPPIArray.push_back(node);
  mPPINameArray.push_back(name);

  make_gate(mInputGateType, node, {});
  return node;
}

// @brief PPO系のノードの生成と登録
NodeRep*
NetworkRep::new_ppo(
  const std::string& name,
  std::function<NodeRep*(SizeType id, SizeType output_id)> new_func
)
{
  auto oid = mPPOArray.size();
  auto node = new_node(
    [&](SizeType id) {
      return new_func(id, oid);
    }
  );
  mPPOArray.push_back(node);
  mPPONameArray.push_back(name);

  make_gate(mOutputGateType, node, {});
  return node;
}

// @brief ノードの生成して登録する．
NodeRep*
NetworkRep::new_node(
  std::function<NodeRep*(SizeType id)> new_func
)
{
  auto id = mNodeArray.size();
  auto node = new_func(id);
  mNodeArray.push_back(std::unique_ptr<NodeRep>{node});
  return node;
}

// @brief ゲートタイプを生成して登録する．
GateType*
NetworkRep::new_gate_type(
  std::function<GateType*(SizeType id)> new_func
)
{
  auto id = mGateTypeArray.size();
  auto gate_type = new_func(id);
  mGateTypeArray.push_back(std::unique_ptr<GateType>{gate_type});
  return gate_type;
}

// @brief ゲートを生成して登録する．
GateRep*
NetworkRep::new_gate(
  std::function<GateRep*(SizeType id)> new_func
)
{
  auto id = mGateArray.size();
  auto gate = new_func(id);
  mGateArray.push_back(std::unique_ptr<GateRep>{gate});
  return gate;
}

// @brief FFR を作る．
const FFRRep*
NetworkRep::new_ffr(
  const NodeRep* root
)
{
  std::vector<const NodeRep*> input_list;
  std::vector<const NodeRep*> node_list;

  // input_list の重複チェック用のハッシュ表のふりをした配列
  std::vector<bool> input_hash(node_num(), false);

  // DFS を行うためのスタック
  std::vector<const NodeRep*> node_stack;
  node_stack.push_back(root);
  node_list.push_back(root);
  while ( !node_stack.empty() ) {
    auto node = node_stack.back();
    node_stack.pop_back();
    for ( auto inode: node->fanin_list() ) {
      if ( inode->ffr_root() == inode ) {
	// inode は他の FFR の根
	if ( !input_hash[inode->id()] ) {
	  input_hash[inode->id()] = true;
	  input_list.push_back(inode);
	}
      }
      else {
	if ( !inode->is_ppi() ) {
	  node_stack.push_back(inode);
	}
	node_list.push_back(inode);
      }
    }
  }
  SizeType id = mFFRArray.size();
  auto ffr = new FFRRep(id, root, input_list, node_list);
  mFFRArray.push_back(std::unique_ptr<FFRRep>{ffr});
  mFFRMap.emplace(root->id(), ffr);
  return ffr;
}

// @brief MFFC を作る．
void
NetworkRep::new_mffc(
  const NodeRep* root,
  const std::unordered_map<SizeType, const FFRRep*>& ffr_map
)
{
  std::vector<const FFRRep*> ffr_list;

  // root を根とする MFFC の情報を得る．
  std::vector<bool> mark(node_num(), false);
  std::vector<const NodeRep*> node_list;

  node_list.push_back(root);
  mark[root->id()] = true;
  while ( !node_list.empty() ) {
    auto node = node_list.back();
    node_list.pop_back();

    if ( node->ffr_root() == node ) {
      if ( ffr_map.count(node->id()) == 0 ) {
	throw std::logic_error{"ffr_map.count(node->id()) == 0"};
      }
      auto ffr = ffr_map.at(node->id());
      ffr_list.push_back(ffr);
    }

    for ( auto inode: node->fanin_list() ) {
      if ( !mark[inode->id()] &&
	   inode->imm_dom() != nullptr ) {
	mark[inode->id()] = true;
	node_list.push_back(inode);
      }
    }
  }
  SizeType id = mMFFCArray.size();
  auto mffc = new MFFCRep(id, root, ffr_list);
  mMFFCArray.push_back(std::unique_ptr<MFFCRep>{mffc});
  mMFFCMap.emplace(root->id(), mffc);
}

END_NAMESPACE_DRUID
