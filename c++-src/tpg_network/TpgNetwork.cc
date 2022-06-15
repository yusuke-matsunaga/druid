
/// @File TpgNetwork.cc
/// @brief TpgNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014, 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNetwork.h"
#include "TpgNetworkImpl.h"
#include "TpgNode.h"
#include "TpgDff.h"
#include "GateType.h"

#include "ym/BnNetwork.h"
#include "ym/ClibCellLibrary.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNetwork
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgNetwork::TpgNetwork() :
  mImpl(new TpgNetworkImpl)
{
}

// @brief デストラクタ
TpgNetwork::~TpgNetwork()
{
}

// @brief ノード数を得る．
SizeType
TpgNetwork::node_num() const
{
  return mImpl->node_num();
}

// @brief ノードを得る．
const TpgNode*
TpgNetwork::node(
  SizeType id
) const
{
  return mImpl->node(id);
}

// @brief 全ノードのリストを得る．
const vector<const TpgNode*>&
TpgNetwork::node_list() const
{
  return mImpl->node_list();
}

// @brief ノード名を得る．
const string&
TpgNetwork::node_name(
  int id
) const
{
  return mImpl->node_name(id);
}

// @brief 外部入力数を得る．
SizeType
TpgNetwork::input_num() const
{
  return mImpl->input_num();
}

// @brief 外部入力ノードを得る．
const TpgNode*
TpgNetwork::input(
  SizeType pos
) const
{
  return mImpl->input(pos);
}

// @brief 外部入力ノードのリストを得る．
const vector<const TpgNode*>&
TpgNetwork::input_list() const
{
  return mImpl->input_list();
}

// @brief 外部出力数を得る．
SizeType
TpgNetwork::output_num() const
{
  return mImpl->output_num();
}

// @brief 外部出力ノードを得る．
const TpgNode*
TpgNetwork::output(
  SizeType pos
) const
{
  return mImpl->output(pos);
}

// @brief 外部出力ノードのリストを得る．
const vector<const TpgNode*>&
TpgNetwork::output_list() const
{
  return mImpl->output_list();
}

// @brief TFIサイズの降順で整列した順番で外部出力ノードを取り出す．
const TpgNode*
TpgNetwork::output2(
  SizeType pos
) const
{
  return mImpl->output2(pos);
}

// @brief スキャン方式の擬似外部入力数を得る．
SizeType
TpgNetwork::ppi_num() const
{
  return mImpl->ppi_num();
}

// @brief スキャン方式の擬似外部入力を得る．
const TpgNode*
TpgNetwork::ppi(
  SizeType pos
) const
{
  return mImpl->ppi(pos);
}

// @brief 擬似外部入力のリストを得る．
const vector<const TpgNode*>&
TpgNetwork::ppi_list() const
{
  return mImpl->ppi_list();
}

// @brief スキャン方式の擬似外部出力数を得る．
SizeType
TpgNetwork::ppo_num() const
{
  return mImpl->ppo_num();
}

// @brief スキャン方式の擬似外部出力を得る．
const TpgNode*
TpgNetwork::ppo(
  SizeType pos
) const
{
  return mImpl->ppo(pos);
}

// @brief 擬似外部出力のリストを得る．
const vector<const TpgNode*>&
TpgNetwork::ppo_list() const
{
  return mImpl->ppo_list();
}

// @brief MFFC 数を返す．
SizeType
TpgNetwork::mffc_num() const
{
  return mImpl->mffc_num();
}

// @brief MFFC を返す．
// @param[in] pos 位置番号 ( 0 <= pos < mffc_num() )
const TpgMFFC&
TpgNetwork::mffc(
  SizeType pos
) const
{
  return mImpl->mffc(pos);
}

// @brief MFFC のリストを得る．
const vector<TpgMFFC>&
TpgNetwork::mffc_list() const
{
  return mImpl->mffc_list();
}

// @brief FFR 数を返す．
SizeType
TpgNetwork::ffr_num() const
{
  return mImpl->ffr_num();
}

// @brief FFR を返す．
const TpgFFR&
TpgNetwork::ffr(
  SizeType pos
) const
{
  return mImpl->ffr(pos);
}

// @brief FFR のリストを得る．
const vector<TpgFFR>&
TpgNetwork::ffr_list() const
{
  return mImpl->ffr_list();
}

// @brief DFF数を得る．
SizeType
TpgNetwork::dff_num() const
{
  return mImpl->dff_num();
}

// @brief DFF を得る．
const TpgDff&
TpgNetwork::dff(
  SizeType pos
) const
{
  return mImpl->dff(pos);
}

// @brief DFF のリストを得る．
const vector<TpgDff>&
TpgNetwork::dff_list() const
{
  return mImpl->dff_list();
}

// @brief 故障IDの最大値+1を返す．
SizeType
TpgNetwork::max_fault_id() const
{
  return mImpl->max_fault_id();
}

// @brief 全代表故障数を返す．
SizeType
TpgNetwork::rep_fault_num() const
{
  return mImpl->rep_fault_num();
}

// @brief 代表故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < rep_fault_num() )
const TpgFault*
TpgNetwork::rep_fault(
  SizeType pos
) const
{
  return mImpl->rep_fault(pos);
}

// @brief 代表故障のリストを返す．
const vector<const TpgFault*>&
TpgNetwork::rep_fault_list() const
{
  return mImpl->rep_fault_list();
}

// @brief ノードに関係した代表故障数を返す．
SizeType
TpgNetwork::node_rep_fault_num(
  SizeType id
) const
{
  return mImpl->node_rep_fault_num(id);
}

// @brief ノードに関係した代表故障を返す．
const TpgFault*
TpgNetwork::node_rep_fault(
  SizeType id,
  SizeType pos
) const
{
  return mImpl->node_rep_fault(id, pos);
}

// @brief BnNetwork から内容を設定する．
void
TpgNetwork::set(
  const BnNetwork& network
)
{
  mImpl->set(network);
}

// @brief blif ファイルを読み込む．
bool
TpgNetwork::read_blif(
  const string& filename
)
{
  return read_blif(filename, ClibCellLibrary());
}

// @brief blif ファイルを読み込む．
bool
TpgNetwork::read_blif(
  const string& filename,
  const ClibCellLibrary& cell_library
)
{
  BnNetwork network = BnNetwork::read_blif(filename, cell_library);
  bool stat = network.node_num() != 0;
  if ( stat ) {
    set(network);
  }

  return stat;
}

// @brief iscas89 形式のファイルを読み込む．
bool
TpgNetwork::read_iscas89(
  const string& filename
)
{
  BnNetwork network = BnNetwork::read_iscas89(filename);
  bool stat = network.node_num() != 0;
  if ( stat ) {
    set(network);
  }

  return stat;
}

// @brief TpgNetwork の内容を出力する関数
void
print_network(
  ostream& s,
  const TpgNetwork& network
)
{
  int n = network.node_num();
  for ( auto node: network.node_list() ) {
    print_node(s, network, node);
    s << ": ";
    if ( node->is_primary_input() ) {
      s << "INPUT#" << node->input_id();
    }
    else if ( node->is_dff_output() ) {
      s << "INPUT#" << node->input_id()
	<< "(DFF#" << node->dff()->id() << ".output)";
    }
    else if ( node->is_primary_output() ) {
      s << "OUTPUT#" << node->output_id();
      const TpgNode* inode = node->fanin(0);
      s << " = ";
      print_node(s, network, inode);
    }
    else if ( node->is_dff_input() ) {
      s << "OUTPUT#" << node->output_id()
	<< "(DFF#" << node->dff()->id() << ".input)";
      const TpgNode* inode = node->fanin(0);
      s << " = ";
      print_node(s, network, inode);
    }
    else if ( node->is_dff_clock() ) {
      s << "DFF#" << node->dff()->id() << ".clock";
    }
    else if ( node->is_dff_clear() ) {
      s << "DFF#" << node->dff()->id() << ".clear";
    }
    else if ( node->is_dff_preset() ) {
      s << "DFF#" << node->dff()->id() << ".preset";
    }
    else if ( node->is_logic() ) {
      s << node->gate_type();
      int ni = node->fanin_num();
      if ( ni > 0 ) {
	s << "(";
	for ( auto inode: node->fanin_list() ) {
	  s << " ";
	  print_node(s, network, inode);
	}
	s << " )";
      }
    }
    else {
      ASSERT_NOT_REACHED;
    }
    s << endl;
  }
  s << endl;
}

// @brief ノード名を出力する
// @param[in] s 出力先のストリーム
// @param[in] node 対象のノード
void
print_node(ostream& s,
	   const TpgNetwork& network,
	   const TpgNode* node)
{
  s << "NODE#" << node->id() << ": " << network.node_name(node->id());
}

END_NAMESPACE_DRUID
