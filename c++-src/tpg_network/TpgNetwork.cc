
/// @File TpgNetwork.cc
/// @brief TpgNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014, 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgNetworkImpl.h"
#include "TpgNode.h"
#include "TpgDFF.h"
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

// @brief blif ファイルを読み込む．
TpgNetwork
TpgNetwork::read_blif(
  const string& filename,
  const string& clock_name,
  const string& reset_name
)
{
  return read_blif(filename, ClibCellLibrary{}, clock_name, reset_name);
}

// @brief ファイルを読み込む
TpgNetwork
TpgNetwork::read_network(
  const string& filename,
  const string& format,
  const ClibCellLibrary& cell_library,
  const string& clock_name,
  const string& reset_name
)
{
  if ( format == "blif" ) {
    return read_blif(filename, cell_library, clock_name, reset_name);
  }
  if ( format == "iscas89" ) {
    return read_iscas89(filename, clock_name);
  }
  ostringstream buf;
  buf << format << ": Unknown format";
  throw std::invalid_argument(buf.str());
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

// @brief PPI の名前を返す．
string
TpgNetwork::ppi_name(
  SizeType input_id
) const
{
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

// @brief PPO の名前を返す．
string
TpgNetwork::ppo_name(
  SizeType output_id
) const
{
  return mImpl->ppo_name(output_id);
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
TpgMFFC
TpgNetwork::mffc(
  SizeType pos
) const
{
  return TpgMFFC{mImpl.get(), pos};
}

// @brief FFR 数を返す．
SizeType
TpgNetwork::ffr_num() const
{
  return mImpl->ffr_num();
}

// @brief FFR を返す．
TpgFFR
TpgNetwork::ffr(
  SizeType pos
) const
{
  return TpgFFR{mImpl.get(), pos};
}

// @brief DFF数を得る．
SizeType
TpgNetwork::dff_num() const
{
  return mImpl->dff_num();
}

// @brief DFF を得る．
TpgDFF
TpgNetwork::dff(
  SizeType pos
) const
{
  auto impl = &(mImpl->_dff(pos));
  return TpgDFF{impl};
}

// @brief DFF のリストを得る．
TpgDFFList
TpgNetwork::dff_list() const
{
  return TpgDFFList{mImpl->dff_list()};
}

// @brief ゲート数を返す．
SizeType
TpgNetwork::gate_num() const
{
  return mImpl->gate_num();
}

// @brief ゲート情報を得る．
TpgGate
TpgNetwork::gate(
  SizeType pos
) const
{
  return TpgGate{mImpl.get(), pos};
}

// @brief TpgNetwork の内容を出力する関数
void
TpgNetwork::print(
  ostream& s
)
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
    s << "FFR#" << ffr.id() << endl
      << "  ROOT: " << ffr.root()->str()
      << endl;
    SizeType ni = ffr.input_num();
    for ( SizeType i = 0; i < ni; ++ i ) {
      s << "  INPUT#" << i << ": "
	<< ffr.input(i)->str()
	<< endl;
    }
    SizeType nn = ffr.node_num();
    for ( SizeType i = 0; i < nn; ++ i ) {
      s << "  " << ffr.node(i)->str()
	<< endl;
    }
  }
  s << endl;

  for ( auto mffc: mffc_list() ) {
    s << "MFFC#" << mffc.id()
      << endl
      << "  ROOT: " << mffc.root()->str()
      << endl;
    SizeType nf = mffc.ffr_num();
    for ( auto ffr: mffc.ffr_list() ) {
      s << "  FFR#" << ffr.id() << endl;
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
    s << "GATE#" << gate.id()
      << ": " << gate.name() << endl
      << "  Output: " << gate.output_node()->str()
      << endl;
    SizeType ni = gate.input_num();
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto binfo = gate.branch_info(i);
      s << "  Input#" << i << ": "
	<< binfo.node->str()
	<< "[" << binfo.ipos << "]"
	<< endl;
    }
    s << endl;
  }
}

END_NAMESPACE_DRUID
