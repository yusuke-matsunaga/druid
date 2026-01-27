
/// @File NetworkRep.cc
/// @brief NetworkRep の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "NetworkRep.h"
#include "NodeRep.h"
#include "GateRep.h"
#include "MFFCRep.h"
#include "FFRRep.h"
#include "FaultRep.h"
#include "types/FaultType.h"
#include "types/Fval2.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// immediate dominator リストをマージする．
const NodeRep*
merge(
  const NodeRep* node1,
  const NodeRep* node2
)
{
  for ( ; ; ) {
    if ( node1 == node2 ) {
      return node1;
    }
    if ( node1 == nullptr || node2 == nullptr ) {
      return nullptr;
    }
    SizeType id1 = node1->id();
    SizeType id2 = node2->id();
    if ( id1 < id2 ) {
      node1 = node1->imm_dom();
    }
    else if ( id1 > id2 ) {
      node2 = node2->imm_dom();
    }
  }
}

void
check_network_connection(
  const NetworkRep* network
)
{
  // fanin/fanout の sanity check
  bool error = false;

  for ( auto& node_ptr: network->node_list() ) {
    for ( auto inode: node_ptr->fanin_list() ) {
      bool found = false;
      for ( auto onode: inode->fanout_list() ) {
	if ( onode == node_ptr.get() ) {
	  found = true;
	  break;
	}
      }
      if ( !found ) {
	error = true;
	std::cout << "Error: inode(" << inode->id() << ") is a fanin of "
		  << "node(" << node_ptr->id() << "), but "
		  << "node(" << node_ptr->id() << ") is not a fanout of "
		  << "inode(" << inode->id() << ")"
		  << std::endl;
      }
    }
    for ( auto onode: node_ptr->fanout_list() ) {
      bool found = false;
      for ( auto inode: onode->fanin_list() ) {
	if ( inode == node_ptr.get() ) {
	  found = true;
	  break;
	}
      }
      if ( !found ) {
	error = true;
	std::cout << "Error: onode(" << onode->id() << ") is a fanout of "
		  << "node(" << node_ptr->id() << "), but "
		  << "node(" << node_ptr->id() << ") is not a fanin of "
		  << "onode(" << onode->id() << ")"
		  << std::endl;
      }
    }
  }
  if ( error ) {
    std::cout << "network connectivity check failed"
	      << std::endl;
    abort();
  }
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス NetworkRep
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NetworkRep::NetworkRep(
  FaultType fault_type,
  SizeType input_num,
  SizeType output_num,
  SizeType dff_num,
  SizeType gate_num,
  SizeType extra_node_num
) : mFaultType{fault_type}
{
  mInputGateType = new_gate_type(
    [&](SizeType id) {
      return GateType::new_ppi(id);
    });
  mOutputGateType = new_gate_type(
    [&](SizeType id) {
      return GateType::new_ppo(id);
    });
  set_size(input_num, output_num, dff_num, gate_num, extra_node_num);
}

// @brief set() の後処理
void
NetworkRep::post_op()
{
  { // 検証
    // 接続が正しいかチェックする．
    check_network_connection(this);
  }

  //////////////////////////////////////////////////////////////////////
  // PPO に到達可能でないノードがないか調べる．
  //////////////////////////////////////////////////////////////////////
  auto tfi_list = get_tfi_list(ppo_list());
  if ( tfi_list.size() < node_num() ) {
    std::cerr << "warning: some nodes cannot reach to the outputs"
	      << std::endl;
    // テスト不能故障になるだけ
  }

  //////////////////////////////////////////////////////////////////////
  // TFI のサイズの昇順に並べた出力順を
  // mPPOArray2 に記録する．
  //////////////////////////////////////////////////////////////////////
  SizeType npo = ppo_num();
  std::vector<std::pair<SizeType, SizeType>> tmp_list(npo);
  for ( SizeType i: Range(npo) ) {
    auto onode = ppo(i);
    // onode の TFI のノード数を計算する．
    SizeType n = 0;
    dfs({onode},
	[&](const NodeRep*) { ++ n; },
	[](const NodeRep*) {});
    tmp_list[i] = std::make_pair(n, i);
  }

  // TFI のサイズの昇順にソートする．
  sort(tmp_list.begin(), tmp_list.end(),
       [](const std::pair<SizeType, SizeType>& a,
	  const std::pair<SizeType, SizeType>& b)->bool
       {
	 return a.first < b.first;
       });

  // tmp_list の順に mPPOArray2 にセットする．
  for ( auto& p: tmp_list ) {
    SizeType opos = p.second;
    auto onode = mPPOArray[opos];
    SizeType opos2 = mPPOArray2.size();
    mPPOArray2.push_back(onode);
    const_cast<NodeRep*>(onode)->set_output_id2(opos2);
  }

  // immediate dominator を求める．
  for ( auto i: Range(node_num()) ) {
    auto node = mNodeArray[node_num() - i - 1].get();
    const NodeRep* imm_dom = nullptr;
    if ( !node->is_ppo() ) {
      SizeType nfo = node->fanout_num();
      if ( nfo > 0 ) {
	bool first = true;
	for ( auto onode: node->fanout_list() ) {
	  if ( first ) {
	    imm_dom = onode;
	    first = false;
	  }
	  else {
	    imm_dom = merge(imm_dom, onode);
	  }
	}
      }
    }
    node->set_imm_dom(imm_dom);
  }

  //////////////////////////////////////////////////////////////////////
  // FFR と MFFC の根のノードを求める．
  //////////////////////////////////////////////////////////////////////
  std::vector<const NodeRep*> ffr_root_list;
  std::vector<const NodeRep*> mffc_root_list;
  for ( auto& node_ptr: node_list() ) {
    auto node = node_ptr.get();
    if ( node->ffr_root() == node ) {
      ffr_root_list.push_back(node);

      // MFFC の根は必ず FFR の根でもある．
      if ( node->imm_dom() == nullptr ) {
	mffc_root_list.push_back(node);
      }
    }
  }

  //////////////////////////////////////////////////////////////////////
  // FFR の情報を作る．
  //////////////////////////////////////////////////////////////////////
  SizeType ffr_num = ffr_root_list.size() ;
  mFFRArray.clear();
  mFFRArray.reserve(ffr_num);
  // ノード番号をキーにしてFFR番号を格納する辞書
  // FFRの根のノードだけ設定する．
  std::unordered_map<SizeType, const FFRRep*> ffr_map;
  for ( auto node: ffr_root_list ) {
    auto ffr = new_ffr(node);
    ffr_map.emplace(node->id(), ffr);
  }

  //////////////////////////////////////////////////////////////////////
  // MFFC の情報を作る．
  //////////////////////////////////////////////////////////////////////
  SizeType mffc_num = mffc_root_list.size();
  mMFFCArray.clear();
  mMFFCArray.reserve(mffc_num);
  for ( auto node: mffc_root_list ) {
    new_mffc(node, ffr_map);
  }

  //////////////////////////////////////////////////////////////////////
  // 故障を作る．
  //////////////////////////////////////////////////////////////////////
  // 各ノードの出力の故障を記録する配列
  // 場合によっては対応する故障がないノードもある．
  std::vector<SizeType> fault_map(node_num() * 2, static_cast<SizeType>(-1));
  for ( auto& gate_ptr: gate_list() ) {
    auto gate = gate_ptr.get();
    gen_gate_faults(gate, fault_map);
    {
      for ( SizeType i = 0; i < gate->input_num(); ++ i ) {
	for ( auto fval: {Fval2::zero, Fval2::one} ) {
	  auto f = gate->branch_fault(i, fval);
	  if ( !f->is_branch() ) {
	    abort();
	  }
	}
      }
    }
  }

  //////////////////////////////////////////////////////////////////////
  // 代表故障を求める．
  //////////////////////////////////////////////////////////////////////
  SizeType NF = mFaultArray.size();
  std::vector<SizeType> rep_map(NF, static_cast<SizeType>(-1));
  // この処理は入力からのトポロジカル順で行う必要がある．
  // gate_list() はトポロジカル順になっているはず．
  for ( auto& gate_ptr: gate_list() ) {
    auto gate = gate_ptr.get();
    gen_rep_map(gate, fault_map, rep_map);
  }
  // rep_map の情報を元に代表故障を求める．
  // この処理は出力側からのトポロジカル順だと都合がよい．
  for ( SizeType i = 0; i < NF; ++ i ) {
    SizeType fid = NF - i - 1;
    auto& f_ptr =  mFaultArray[fid];
    auto f = f_ptr.get();
    auto rep_id = rep_map[fid];
    if ( rep_id != static_cast<SizeType>(-1) ) {
      auto& rep_f = mFaultArray[rep_id];
      f->set_rep_fault(rep_f->rep_fault());
    }
    else {
      // それ以外は自身が代表故障
      f->set_rep_fault(f);
    }
  }

  // 求めた代表故障を記録する．
  mRepFidList.clear();
  for ( auto& f_ptr: mFaultArray ) {
    auto f = f_ptr.get();
    if ( f->rep_fault() == f ) {
      mRepFidList.push_back(f->id());
    }
  }
}

END_NAMESPACE_DRUID
