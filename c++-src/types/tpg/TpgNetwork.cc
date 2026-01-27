
/// @File TpgNetwork.cc
/// @brief TpgNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgNetwork.h"
#include "NetworkRep.h"
#include "NetBuilder.h"
#include "types/TpgNode.h"
#include "types/TpgMFFC.h"
#include "types/TpgFFR.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "ym/BnModel.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNetwork
//////////////////////////////////////////////////////////////////////

// @brief ファイルを読み込む
TpgNetwork
TpgNetwork::read_network(
  const std::string& filename,
  const std::string& format,
  FaultType fault_type
)
{
  if ( format == "blif" ) {
    return read_blif(filename, fault_type);
  }
  if ( format == "iscas89" ) {
    return read_iscas89(filename, fault_type);
  }
  std::ostringstream buf;
  buf << format << ": Unknown format";
  throw std::invalid_argument(buf.str());
}

// @brief blif ファイルを読み込む．
TpgNetwork
TpgNetwork::read_blif(
  const std::string& filename,
  FaultType fault_type
)
{
  auto model = BnModel::read_blif(filename);
  return TpgNetwork::from_bn(model, fault_type);
}

// @brief iscas89 形式のファイルを読み込む．
TpgNetwork
TpgNetwork::read_iscas89(
  const std::string& filename,
  FaultType fault_type
)
{
  auto model = BnModel::read_iscas89(filename);
  return TpgNetwork::from_bn(model, fault_type);
}

// @brief BnModel から変換する．
TpgNetwork
TpgNetwork::from_bn(
  const BnModel& model,
  FaultType fault_type
)
{
  return NetBuilder::from_bn(model, fault_type);
}

// @brief ノード数を得る．
SizeType
TpgNetwork::node_num() const
{
  _check_valid();
  return _network()->node_num();
}

// @brief ノードを得る．
TpgNode
TpgNetwork::node(
  SizeType id
) const
{
  _check_valid();
  if ( id >= node_num() ) {
    throw std::out_of_range{"id is out of range"};
  }
  return TpgBase::node(id);
}

// @brief 全ノードのリストを得る．
TpgNodeList
TpgNetwork::node_list() const
{
  _check_valid();
  return TpgBase::node_list(make_id_list(node_num()));
}

// @brief 外部入力数を得る．
SizeType
TpgNetwork::input_num() const
{
  _check_valid();
  return _network()->input_num();
}

// @brief 外部入力ノードを得る．
TpgNode
TpgNetwork::input(
  SizeType pos
) const
{
  _check_valid();
  return TpgBase::node(_network()->input(pos));
}

// @brief 外部入力ノードのリストを得る．
TpgNodeList
TpgNetwork::input_list() const
{
  _check_valid();
  return TpgBase::node_list(_network()->input_list());
}

// @brief 外部出力数を得る．
SizeType
TpgNetwork::output_num() const
{
  _check_valid();
  return _network()->output_num();
}

// @brief 外部出力ノードを得る．
TpgNode
TpgNetwork::output(
  SizeType pos
) const
{
  _check_valid();
  return TpgBase::node(_network()->output(pos));
}

// @brief 外部出力ノードのリストを得る．
TpgNodeList
TpgNetwork::output_list() const
{
  _check_valid();
  return TpgBase::node_list(_network()->output_list());
}

// @brief TFIサイズの降順で整列した順番で外部出力ノードを取り出す．
TpgNode
TpgNetwork::output2(
  SizeType pos
) const
{
  _check_valid();
  return TpgBase::node(_network()->output2(pos));
}

// @brief スキャン方式の擬似外部入力数を得る．
SizeType
TpgNetwork::ppi_num() const
{
  _check_valid();
  return _network()->ppi_num();
}

// @brief スキャン方式の擬似外部入力を得る．
TpgNode
TpgNetwork::ppi(
  SizeType pos
) const
{
  _check_valid();
  return TpgBase::node(_network()->ppi(pos));
}

// @brief PPI の名前を返す．
std::string
TpgNetwork::ppi_name(
  SizeType input_id
) const
{
  _check_valid();
  return _network()->ppi_name(input_id);
}

// @brief 擬似外部入力のリストを得る．
TpgNodeList
TpgNetwork::ppi_list() const
{
  return TpgBase::node_list(_network()->ppi_list());
}

// @brief スキャン方式の擬似外部出力数を得る．
SizeType
TpgNetwork::ppo_num() const
{
  _check_valid();
  return _network()->ppo_num();
}

// @brief スキャン方式の擬似外部出力を得る．
TpgNode
TpgNetwork::ppo(
  SizeType pos
) const
{
  _check_valid();
  return TpgBase::node(_network()->ppo(pos));
}

// @brief PPO の名前を返す．
std::string
TpgNetwork::ppo_name(
  SizeType output_id
) const
{
  _check_valid();
  return _network()->ppo_name(output_id);
}

// @brief 擬似外部出力のリストを得る．
TpgNodeList
TpgNetwork::ppo_list() const
{
  _check_valid();
  return TpgBase::node_list(_network()->ppo_list());
}

// @brief MFFC 数を返す．
SizeType
TpgNetwork::mffc_num() const
{
  _check_valid();
  return _network()->mffc_num();
}

// @brief MFFC を返す．
TpgMFFC
TpgNetwork::mffc(
  SizeType mffc_id
) const
{
  _check_valid();
  return TpgBase::mffc(mffc_id);
}

// @brief node の属している MFFC を返す．
TpgMFFC
TpgNetwork::mffc(
  const TpgNode& node
) const
{
  _check_valid();
  return TpgBase::mffc(_network()->mffc(_node(node.id())));
}

// @brief fault の属している FFR を返す．
TpgMFFC
TpgNetwork::mffc(
  const TpgFault& fault
) const
{
  return mffc(fault.ffr_root());
}

// @brief MFFC のリストを得る．
TpgMFFCList
TpgNetwork::mffc_list() const
{
  _check_valid();
  return TpgBase::mffc_list(make_id_list(mffc_num()));
}

// @brief FFR 数を返す．
SizeType
TpgNetwork::ffr_num() const
{
  _check_valid();
  return _network()->ffr_num();
}

// @brief FFR を返す．
TpgFFR
TpgNetwork::ffr(
  SizeType ffr_id
) const
{
  _check_valid();
  return TpgBase::ffr(ffr_id);
}

// @brief node の属している FFR を返す．
TpgFFR
TpgNetwork::ffr(
  const TpgNode& node
) const
{
  _check_valid();
  return TpgBase::ffr(_network()->ffr(_node(node.id())));
}

// @brief fault の属している FFR を返す．
TpgFFR
TpgNetwork::ffr(
  const TpgFault& fault
) const
{
  return ffr(fault.ffr_root());
}

// @brief FFR のリストを得る．
TpgFFRList
TpgNetwork::ffr_list() const
{
  _check_valid();
  return TpgBase::ffr_list(make_id_list(ffr_num()));
}

// @brief DFF数を得る．
SizeType
TpgNetwork::dff_num() const
{
  _check_valid();
  return _network()->dff_num();
}

// @brief DFFの入力ノードを得る．
TpgNode
TpgNetwork::dff_input(
  SizeType dff_id
) const
{
  _check_valid();
  return TpgBase::node(_network()->dff_input(dff_id));
}

// @brief DFFの出力ノードを得る．
TpgNode
TpgNetwork::dff_output(
  SizeType dff_id
) const
{
  _check_valid();
  return TpgBase::node(_network()->dff_output(dff_id));
}

// @brief TFO のノードを求める．
TpgNodeList
TpgNetwork::get_tfo_list(
  const TpgNode& root,
  const TpgNode& block
) const
{
  auto root_list = TpgBase::node_list({root.id()});
  return get_tfo_list(root_list, block, [](const TpgNode&){});
}

// @brief TFO のノードを求める．
TpgNodeList
TpgNetwork::get_tfo_list(
  const TpgNode& root,
  std::function<void(const TpgNode&)> op
) const
{
  auto root_list = TpgBase::node_list({root.id()});
  return get_tfo_list(root_list, TpgNode{}, op);
}

// @brief TFO のノードを求める．
TpgNodeList
TpgNetwork::get_tfo_list(
  const TpgNodeList& root_list,
  const TpgNode& block,
  std::function<void(const TpgNode&)> op
) const
{
  _check_valid();
  auto node_rep_list = _network()->get_tfo_list(_node_list(root_list.id_list()),
						TpgBase::_node(block),
						[&](const NodeRep* node_rep){
						  op(TpgBase::node(node_rep));
						});
  return TpgBase::node_list(node_rep_list);
}

// @brief TFI のノードを求める．
TpgNodeList
TpgNetwork::get_tfi_list(
  const TpgNodeList& root_list,
  std::function<void(const TpgNode&)> op
) const
{
  _check_valid();
  auto node_rep_list = _network()->get_tfi_list(_node_list(root_list.id_list()),
						[&](const NodeRep* node_rep){
						  op(TpgBase::node(node_rep));
						});
  return TpgBase::node_list(node_rep_list);
}

// @brief 出力からの DFS を行う．
void
TpgNetwork::dfs(
  const TpgNodeList& root_list,
  std::function<void(const TpgNode&)> pre_func,
  std::function<void(const TpgNode&)> post_func
) const
{
  _check_valid();
  _network()->dfs(_node_list(root_list.id_list()),
		  [&](const NodeRep* node_rep){
		    pre_func(TpgBase::node(node_rep));
		  },
		  [&](const NodeRep* node_rep){
		    post_func(TpgBase::node(node_rep));
		  });
}

// @brief ゲート数を返す．
SizeType
TpgNetwork::gate_num() const
{
  _check_valid();
  return _network()->gate_num();
}

// @brief ゲート情報を得る．
TpgGate
TpgNetwork::gate(
  SizeType gid
) const
{
  _check_valid();
  if ( gid >= gate_num() ) {
    throw std::out_of_range{"gid is out of range"};
  }
  return TpgGate(_network(), gid);
}

// @brief 故障の種類を返す．
FaultType
TpgNetwork::fault_type() const
{
  _check_valid();
  return _network()->fault_type();
}

// @brief 1時刻前の状態を持つ時 true を返す．
bool
TpgNetwork::has_prev_state() const
{
  return fault_type() == FaultType::TransitionDelay;
}

// @brief 代表故障の故障番号のリストを得る．
TpgFaultList
TpgNetwork::rep_fault_list() const
{
  _check_valid();
  return TpgFaultList(_network(), _network()->rep_fid_list());
}

// @brief 故障を得る．
TpgFault
TpgNetwork::fault(
  SizeType fault_id
) const
{
  return TpgBase::fault(fault_id);
}

// @brief 故障番号の最大値を返す．
SizeType
TpgNetwork::max_fault_id() const
{
  _check_valid();
  return _network()->max_fault_id();
}

// @brief TpgNetwork の内容を出力する関数
void
TpgNetwork::print(
  std::ostream& s
) const
{
  for ( SizeType id = 0; id < node_num(); ++ id ) {
    auto node = this->node(id);
    s << node.str()
      << ": ";
    if ( node.is_primary_input() ) {
      s << "INPUT#" << node.input_id();
    }
    else if ( node.is_dff_output() ) {
      s << "INPUT#" << node.input_id()
	<< "(DFF#" << node.dff_id() << ".output)";
    }
    else if ( node.is_primary_output() ) {
      s << "OUTPUT#" << node.output_id();
      auto inode = node.fanin(0);
      s << " = " << inode.str();
    }
    else if ( node.is_dff_input() ) {
      s << "OUTPUT#" << node.output_id()
	<< "(DFF#" << node.dff_id() << ".input)";
      auto inode = node.fanin(0);
      s << " = " << inode.str();
    }
    else if ( node.is_logic() ) {
      s << node.gate_type();
      auto ni = node.fanin_num();
      if ( ni > 0 ) {
	s << "(";
	for ( auto inode: node.fanin_list() ) {
	  s << " " << inode.str();
	}
	s << " )";
      }
    }
    else {
      throw std::logic_error{"never be reached"};
    }
    s << std::endl;
  }
  s << std::endl;

  for ( auto ffr: ffr_list() ) {
    s << "FFR#" << ffr.id()
      << std::endl
      << "  ROOT: " << ffr.root().str()
      << std::endl;
    auto ni = ffr.input_num();
    for ( SizeType i = 0; i < ni; ++ i ) {
      s << "  INPUT#" << i << ": "
	<< ffr.input(i).str()
	<< std::endl;
    }
    auto nn = ffr.node_num();
    for ( SizeType i = 0; i < nn; ++ i ) {
      s << "  " << ffr.node(i).str()
	<< std::endl;
    }
  }
  s << std::endl;

  for ( auto mffc: mffc_list() ) {
    s << "MFFC#" << mffc.id()
      << std::endl
      << "  ROOT: " << mffc.root().str()
      << std::endl;
    auto nf = mffc.ffr_num();
    for ( auto ffr: mffc.ffr_list() ) {
      s << "  FFR#" << ffr.id()
	<< std::endl;
    }
  }
  s << std::endl;

  for ( SizeType i = 0; i < ppi_num(); ++ i ) {
    auto name = ppi_name(i);
    s << "PPI#" << i << ": " << name
      << ": " << ppi(i).str()
      << std::endl;
  }
  s << std::endl;

  for ( SizeType i = 0; i < ppo_num(); ++ i ) {
    auto name = ppo_name(i);
    s << "PPO#" << i << ": " << name
      << ": " << ppo(i).str()
      << std::endl;
  }
  s << std::endl;

  for ( SizeType i = 0; i < gate_num(); ++ i ) {
    auto g = gate(i);
    s << "GATE#" << g.id() << ":"
      << std::endl
      << "  Output: " << g.output_node().str()
      << std::endl;
    auto ni = g.input_num();
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto binfo = g.branch_info(i);
      s << "  Input#" << i << ": "
	<< binfo.node.str()
	<< "[" << binfo.ipos << "]"
	<< "(" << binfo.node.fanin(binfo.ipos).str() << ")"
	<< std::endl;
    }
    if ( !g.is_ppo() ) {
      s << "  Stem Faults:";
      for ( auto fval: {Fval2::zero, Fval2::one} ) {
	auto fault = g.stem_fault(fval);
	s << " " << fault;
      }
      s << std::endl;
    }
    if ( !g.is_ppi() ) {
      s << "  Branch Faults:";
      for ( SizeType ipos = 0; ipos < g.input_num(); ++ ipos ) {
	for ( auto fval: {Fval2::zero, Fval2::one} ) {
	  auto fault = g.branch_fault(ipos, fval);
	  s << " " << fault;
	}
      }
      s << std::endl;
    }
    s << std::endl;
  }

  s << "Representative Faults" << std::endl;
  for ( auto fault: rep_fault_list() ) {
    s << "  " << fault << std::endl;
  }
}

END_NAMESPACE_DRUID
