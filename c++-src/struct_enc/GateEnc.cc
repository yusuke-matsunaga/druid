
/// @file GateEnc.cc
/// @brief GateEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "GateEnc.h"

#include "TpgNode.h"
#include "VidMap.h"

#include "ym/SatSolver.h"


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
  const TpgNode* node
)
{
  make_cnf(node, mVarMap(node));
}

// @brief ノードの入出力の関係を表すCNF式を作る．
void
GateEnc::make_cnf(
  const TpgNode* node,
  SatLiteral olit
)
{
  SizeType ni = node->fanin_num();
  const auto& fanin_array = node->fanin_list();
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
  switch ( node->gate_type() ) {
  case PrimType::None:
    break;

  case PrimType::C0:
    mSolver.add_clause(~olit);
    if ( debug_gate_enc ) {
      cout << "C0: " << olit << endl;
    }
    break;

  case PrimType::C1:
    mSolver.add_clause( olit);
    if ( debug_gate_enc ) {
      cout << "C1: " << olit << endl;
    }
    break;

  case PrimType::Buff:
    {
      auto ilit = lit(fanin_array[0]);
      mSolver.add_buffgate(ilit, olit);
      if ( debug_gate_enc ) {
	cout << "Buff: " << olit << " = " << ilit  << endl;
      }
    }
    break;

  case PrimType::Not:
    {
      auto ilit = lit(fanin_array[0]);
      mSolver.add_notgate(ilit, olit);
      if ( debug_gate_enc ) {
	cout << "Not: " << olit << " = ~" << ilit  << endl;
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
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_andgate( olit, ilits);
      }
      break;
    }
    if ( debug_gate_enc ) {
      cout << "And: " << olit << " = ";
      const char* amp = "";
      for ( auto inode: fanin_array ) {
	auto ilit = lit(inode);
	cout << amp << ilit;
	amp = " & ";
      }
      cout << endl;
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
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_nandgate( olit, ilits);
      }
      break;
    }
    if ( debug_gate_enc ) {
      cout << "Nand: " << olit << " = ~(";
      const char* amp = "";
      for ( auto inode: fanin_array ) {
	auto ilit = lit(inode);
	cout << amp << ilit;
	amp = " & ";
      }
      cout << ")" << endl;
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
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_orgate( olit, ilits);
      }
      break;
    }
    if ( debug_gate_enc ) {
      cout << "Or: " << olit << " = ";
      const char* amp = "";
      for ( auto inode: fanin_array ) {
	auto ilit = lit(inode);
	cout << amp << ilit;
	amp = " | ";
      }
      cout << endl;
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
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_norgate( olit, ilits);
      }
      break;
    }
    if ( debug_gate_enc ) {
      cout << "Nor: " << olit << " = ~(";
      const char* amp = "";
      for ( auto inode: fanin_array ) {
	auto ilit = lit(inode);
	cout << amp << ilit;
	amp = " | ";
      }
      cout << ")" << endl;
    }
    break;

  case PrimType::Xor:
    ASSERT_COND( ni == 2 );
    {
      auto ilit0 = lit(fanin_array[0]);
      auto ilit1 = lit(fanin_array[1]);
      mSolver.add_xorgate( olit, ilit0, ilit1);
      if ( debug_gate_enc ) {
	cout << "Xor: " << olit << " = " << ilit0
	     << " ^ " << ilit1  << endl;
      }
    }
    break;

  case PrimType::Xnor:
    ASSERT_COND( ni == 2 );
    {
      auto ilit0 = lit(fanin_array[0]);
      auto ilit1 = lit(fanin_array[1]);
      mSolver.add_xnorgate( olit, ilit0, ilit1);
      if ( debug_gate_enc ) {
	cout << "Xnor: " << olit << " = ~(" << ilit0
	     << " ^ " << ilit1 << ")" << endl;
      }
    }
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

// @brief ノードに対応するリテラルを返す．
SatLiteral
GateEnc::lit(
  const TpgNode* node
)
{
  return mVarMap(node);
}

END_NAMESPACE_DRUID
