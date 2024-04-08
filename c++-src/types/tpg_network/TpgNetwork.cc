
/// @File TpgNetwork.cc
/// @brief TpgNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014, 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgNetworkImpl.h"
#include "TpgNode.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "ym/ClibCellLibrary.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNetwork
//////////////////////////////////////////////////////////////////////

// @brief 空のコンストラクタ
TpgNetwork::TpgNetwork(
)
{
  // mImpl は nullptr を指すはず
}

// @brief ムーブコンストラクタ
TpgNetwork::TpgNetwork(
  TpgNetwork&& src
) : mImpl{std::move(src.mImpl)}
{
}

// @brief ムーブ代入演算子
TpgNetwork&
TpgNetwork::operator=(
  TpgNetwork&& src
)
{
  std::swap(mImpl, src.mImpl);
  return *this;
}

// @brief デストラクタ
TpgNetwork::~TpgNetwork()
{
}

// @brief ノード数を得る．
SizeType
TpgNetwork::node_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->node_num();
}

// @brief ノードを得る．
const TpgNode*
TpgNetwork::node(
  SizeType id
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->node(id);
}

// @brief 全ノードのリストを得る．
const vector<const TpgNode*>&
TpgNetwork::node_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->node_list();
}

// @brief 外部入力数を得る．
SizeType
TpgNetwork::input_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->input_num();
}

// @brief 外部入力ノードを得る．
const TpgNode*
TpgNetwork::input(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->input(pos);
}

// @brief 外部入力ノードのリストを得る．
const vector<const TpgNode*>&
TpgNetwork::input_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->input_list();
}

// @brief 外部出力数を得る．
SizeType
TpgNetwork::output_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->output_num();
}

// @brief 外部出力ノードを得る．
const TpgNode*
TpgNetwork::output(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->output(pos);
}

// @brief 外部出力ノードのリストを得る．
const vector<const TpgNode*>&
TpgNetwork::output_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->output_list();
}

// @brief TFIサイズの降順で整列した順番で外部出力ノードを取り出す．
const TpgNode*
TpgNetwork::output2(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->output2(pos);
}

// @brief スキャン方式の擬似外部入力数を得る．
SizeType
TpgNetwork::ppi_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->ppi_num();
}

// @brief スキャン方式の擬似外部入力を得る．
const TpgNode*
TpgNetwork::ppi(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->ppi(pos);
}

// @brief PPI の名前を返す．
string
TpgNetwork::ppi_name(
  SizeType input_id
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->ppi_name(input_id);
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
  ASSERT_COND( mImpl != nullptr );

  return mImpl->ppo_num();
}

// @brief スキャン方式の擬似外部出力を得る．
const TpgNode*
TpgNetwork::ppo(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->ppo(pos);
}

// @brief PPO の名前を返す．
string
TpgNetwork::ppo_name(
  SizeType output_id
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->ppo_name(output_id);
}

// @brief 擬似外部出力のリストを得る．
const vector<const TpgNode*>&
TpgNetwork::ppo_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->ppo_list();
}

// @brief MFFC 数を返す．
SizeType
TpgNetwork::mffc_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->mffc_num();
}

// @brief MFFC を返す．
const TpgMFFC*
TpgNetwork::mffc(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->mffc(pos);
}

// @brief MFFC のリストを得る．
const vector<const TpgMFFC*>&
TpgNetwork::mffc_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->mffc_list();
}

// @brief FFR 数を返す．
SizeType
TpgNetwork::ffr_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->ffr_num();
}

// @brief FFR を返す．
const TpgFFR*
TpgNetwork::ffr(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->ffr(pos);
}

// @brief FFR のリストを得る．
const vector<const TpgFFR*>&
TpgNetwork::ffr_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->ffr_list();
}

// @brief DFF数を得る．
SizeType
TpgNetwork::dff_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->dff_num();
}

// @brief DFFの入力ノードを得る．
const TpgNode*
TpgNetwork::dff_input(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->dff_input(pos);
}

// @brief DFFの出力ノードを得る．
const TpgNode*
TpgNetwork::dff_output(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->dff_output(pos);
}

// @brief ゲート数を返す．
SizeType
TpgNetwork::gate_num() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->gate_num();
}

// @brief ゲート情報を得る．
const TpgGate*
TpgNetwork::gate(
  SizeType pos
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->gate(pos);
}

// @brief ゲート情報のリストを得る．
const vector<const TpgGate*>&
TpgNetwork::gate_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->gate_list();
}

// @brief 故障の種類を返す．
FaultType
TpgNetwork::fault_type() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->fault_type();
}

// @brief 代表故障のリストを得る．
const vector<const TpgFault*>&
TpgNetwork::rep_fault_list() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->rep_fault_list();
}

// @brief ステムの故障を得る．
const TpgFault*
TpgNetwork::find_fault(
  const TpgGate* gate,
  Fval2 fval
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->find_fault(gate, fval);
}

// @brief ブランチの故障を得る．
const TpgFault*
TpgNetwork::find_fault(
  const TpgGate* gate,
  SizeType ipos,
  Fval2 fval
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->find_fault(gate, ipos, fval);
}

// @brief 網羅故障を得る．
const TpgFault*
TpgNetwork::find_fault(
  const TpgGate* gate,
  const vector<bool>& ivals
) const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->find_fault(gate, ivals);
}

// @brief TpgNetwork の内容を出力する関数
void
TpgNetwork::print(
  ostream& s
) const
{
  for ( auto node: node_list() ) {
    s << node->str()
      << ": ";
    if ( node->is_primary_input() ) {
      s << "INPUT#" << node->input_id();
    }
    else if ( node->is_dff_output() ) {
      s << "INPUT#" << node->input_id()
	<< "(DFF#" << node->dff_id() << ".output)";
    }
    else if ( node->is_primary_output() ) {
      s << "OUTPUT#" << node->output_id();
      auto inode = node->fanin(0);
      s << " = " << inode->str();
    }
    else if ( node->is_dff_input() ) {
      s << "OUTPUT#" << node->output_id()
	<< "(DFF#" << node->dff_id() << ".input)";
      auto inode = node->fanin(0);
      s << " = " << inode->str();
    }
    else if ( node->is_logic() ) {
      s << node->gate_type();
      SizeType ni = node->fanin_num();
      if ( ni > 0 ) {
	s << "(";
	for ( auto inode: node->fanin_list() ) {
	  s << " " << inode->str();
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

  for ( auto ffr: ffr_list() ) {
    s << "FFR#" << ffr->id() << endl
      << "  ROOT: " << ffr->root()->str()
      << endl;
    SizeType ni = ffr->input_num();
    for ( SizeType i = 0; i < ni; ++ i ) {
      s << "  INPUT#" << i << ": "
	<< ffr->input(i)->str()
	<< endl;
    }
    SizeType nn = ffr->node_num();
    for ( SizeType i = 0; i < nn; ++ i ) {
      s << "  " << ffr->node(i)->str()
	<< endl;
    }
  }
  s << endl;

  for ( auto mffc: mffc_list() ) {
    s << "MFFC#" << mffc->id()
      << endl
      << "  ROOT: " << mffc->root()->str()
      << endl;
    SizeType nf = mffc->ffr_num();
    for ( auto ffr: mffc->ffr_list() ) {
      s << "  FFR#" << ffr->id() << endl;
    }
  }
  s << endl;

  for ( SizeType i = 0; i < ppi_num(); ++ i ) {
    auto name = ppi_name(i);
    s << "PPI#" << i << ": " << name
      << ": " << ppi(i)->str()
      << endl;
  }
  s << endl;

  for ( SizeType i = 0; i < ppo_num(); ++ i ) {
    auto name = ppo_name(i);
    s << "PPO#" << i << ": " << name
      << ": " << ppo(i)->str()
      << endl;
  }
  s << endl;

  for ( auto gate: gate_list() ) {
    s << "GATE#" << gate->id() << ":" << endl
      << "  Output: " << gate->output_node()->str()
      << endl;
    SizeType ni = gate->input_num();
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto binfo = gate->branch_info(i);
      s << "  Input#" << i << ": "
	<< binfo.node->str()
	<< "[" << binfo.ipos << "]"
	<< "(" << binfo.node->fanin(binfo.ipos)->str() << ")"
	<< endl;
    }
    s << endl;
  }
}

END_NAMESPACE_DRUID
