
/// @file GateEnc.cc
/// @brief GateEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "GateEnc.h"

#include "types/TpgNode.h"
#include "types/TpgNodeList.h"
#include "dtpg/VidMap.h"

#include "ym/SatSolver.h"

#define DEBUG_OUT std::cout


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
static const bool debug_gate_enc = false;
static const bool verify_gate_enc = false;
END_NONAMESPACE

// @brief コンストラクタ
GateEnc::GateEnc(
  SatSolver& solver,
  const VidMap& varmap
) : mSolver{solver},
    mVarMap{varmap}
{
}

// @brief デストラクタ
GateEnc::~GateEnc()
{
}

// @brief ノードの入出力の関係を表すCNF式を作る．
void
GateEnc::make_cnf(
  const TpgNode& node
)
{
  make_cnf(node, mVarMap(node));
}

// @brief ノードの入出力の関係を表すCNF式を作る．
void
GateEnc::make_cnf(
  const TpgNode& node,
  SatLiteral olit
)
{
  auto ni = node.fanin_num();
  auto fanin_array = node.fanin_list();
  if ( verify_gate_enc ) {
    if ( olit == SatLiteral::X ) {
      abort();
    }
    for ( auto inode: fanin_array ) {
      auto ilit = lit(inode);
      if ( ilit == SatLiteral::X ) {
	abort();
      }
    }
  }

  switch ( node.gate_type() ) {
  case PrimType::None:
    break;

  case PrimType::C0:
    mSolver.add_clause(~olit);
    if ( debug_gate_enc ) {
      DEBUG_OUT << "C0: " << olit
		<< std::endl;
    }
    break;

  case PrimType::C1:
    mSolver.add_clause( olit);
    if ( debug_gate_enc ) {
      DEBUG_OUT << "C1: " << olit
		<< std::endl;
    }
    break;

  case PrimType::Buff:
    {
      auto ilit = lit(fanin_array[0]);
      mSolver.add_buffgate(ilit, olit);
      if ( debug_gate_enc ) {
	DEBUG_OUT << "Buff: " << olit << " = " << ilit
		  << std::endl;
      }
    }
    break;

  case PrimType::Not:
    {
      auto ilit = lit(fanin_array[0]);
      mSolver.add_notgate(ilit, olit);
      if ( debug_gate_enc ) {
	DEBUG_OUT << "Not: " << olit << " = ~" << ilit
		  << std::endl;
      }
    }
    break;

  case PrimType::And:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	mSolver.add_andgate(olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	mSolver.add_andgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	auto ilit3 = lit(fanin_array[3]);
	mSolver.add_andgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	std::vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_andgate( olit, ilits);
      }
      break;
    }
    if ( debug_gate_enc ) {
      DEBUG_OUT << "And: " << olit << " = ";
      const char* amp = "";
      for ( auto inode: fanin_array ) {
	auto ilit = lit(inode);
	DEBUG_OUT << amp << ilit;
	amp = " & ";
      }
      DEBUG_OUT << std::endl;
    }
    break;

  case PrimType::Nand:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	mSolver.add_nandgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	mSolver.add_nandgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	auto ilit3 = lit(fanin_array[3]);
	mSolver.add_nandgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	std::vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_nandgate( olit, ilits);
      }
      break;
    }
    if ( debug_gate_enc ) {
      DEBUG_OUT << "Nand: " << olit << " = ~(";
      const char* amp = "";
      for ( auto inode: fanin_array ) {
	auto ilit = lit(inode);
	DEBUG_OUT << amp << ilit;
	amp = " & ";
      }
      DEBUG_OUT << ")"
		<< std::endl;
    }
    break;

  case PrimType::Or:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	mSolver.add_orgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	mSolver.add_orgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	auto ilit3 = lit(fanin_array[3]);
	mSolver.add_orgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	std::vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_orgate( olit, ilits);
      }
      break;
    }
    if ( debug_gate_enc ) {
      DEBUG_OUT << "Or: " << olit << " = ";
      const char* amp = "";
      for ( auto inode: fanin_array ) {
	auto ilit = lit(inode);
	DEBUG_OUT << amp << ilit;
	amp = " | ";
      }
      DEBUG_OUT << std::endl;
    }
    break;

  case PrimType::Nor:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	mSolver.add_norgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	mSolver.add_norgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	auto ilit3 = lit(fanin_array[3]);
	mSolver.add_norgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	std::vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_norgate( olit, ilits);
      }
      break;
    }
    if ( debug_gate_enc ) {
      DEBUG_OUT << "Nor: " << olit << " = ~(";
      const char* amp = "";
      for ( auto inode: fanin_array ) {
	auto ilit = lit(inode);
	DEBUG_OUT << amp << ilit;
	amp = " | ";
      }
      DEBUG_OUT << ")"
		<< std::endl;
    }
    break;

  case PrimType::Xor:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	mSolver.add_xorgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	mSolver.add_xorgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    default:
      ASSERT_COND( ni > 3 );
      {
	std::vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_xorgate( olit, ilits);
      }
      break;
    }
    if ( debug_gate_enc ) {
      DEBUG_OUT << "Xor: " << olit << " = (";
      const char* amp = "";
      for ( auto inode: fanin_array ) {
	auto ilit = lit(inode);
	DEBUG_OUT << amp << ilit;
	amp = " ^ ";
      }
      DEBUG_OUT << ")"
		<< std::endl;
    }
    break;

  case PrimType::Xnor:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	mSolver.add_xnorgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	mSolver.add_xnorgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    default:
      ASSERT_COND( ni > 3 );
      {
	std::vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_xnorgate( olit, ilits);
      }
      break;
    }
    if ( debug_gate_enc ) {
      DEBUG_OUT << "Xnor: " << olit << " = ~(";
      const char* amp = "";
      for ( auto inode: fanin_array ) {
	auto ilit = lit(inode);
	DEBUG_OUT << amp << ilit;
	amp = " ^ ";
      }
      DEBUG_OUT << ")"
		<< std::endl;
    }
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

// @brief ノードの入出力の関係を表すCNF式のサイズを見積もる．
CnfSize
GateEnc::calc_cnf_size(
  const TpgNode& node
)
{
  SizeType ni = node.fanin_num();
  // 入出力の極性違いはサイズには影響しないのでまとめて処理する．
  switch ( node.gate_type() ) {
  case PrimType::None:
    break;

  case PrimType::C0:
  case PrimType::C1:
    return CnfSize{1, 1};

  case PrimType::Buff:
  case PrimType::Not:
    // (ilit | ~olit)(~ilit | olit)
    return CnfSize{2, 4};

  case PrimType::And:
  case PrimType::Nand:
  case PrimType::Or:
  case PrimType::Nor:
    // (ilit_i | ~olit)...(~ilit_i | ... | olit)
    return CnfSize{ni + 1, ni * 2 + ni + 1};

  case PrimType::Xor:
  case PrimType::Xnor:
    // XOR2: (ilit0 | ilit1 | ~olit)(ilit0 | ~ilit1 | olit)
    //       (~ilit0 | ilit1 | olit)(~ilit0 | ~ilit1 | ~olit)
    // XOR-n は XOR2 を (n - 1) 個つなげる．
    // 構造は複数考えられるが CNF のサイズは同一
    return CnfSize{4, 12} * (ni - 1);

  default:
    break;
  }
  ASSERT_NOT_REACHED;
  return CnfSize::zero();
}

// @brief ノードに対応するリテラルを返す．
SatLiteral
GateEnc::lit(
  const TpgNode& node
)
{
  return mVarMap(node);
}

END_NAMESPACE_DRUID
