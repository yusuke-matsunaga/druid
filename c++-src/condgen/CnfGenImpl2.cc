
/// @file CnfGenImpl2.cc
/// @brief CnfGenImpl2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGen.h"
#include "CnfGenImpl2.h"
#include "StructEngine.h"
#include "TpgNetwork.h"
#include "BgMgr.h"
#include "BgNode.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// BDD をサイズの昇順に並べるためのヒープ木
class HeapTree
{
public:

  /// @brief コンストラクタ
  HeapTree() = default;

  /// @brief デストラクタ
  ~HeapTree() = default;


public:

  /// @brief 要素数を返す．
  SizeType
  size() const
  {
    return mArray.size();
  }

  /// @brief BDDを追加する．
  void
  put(
    const Bdd& bdd
  )
  {
    auto pos = mArray.size();
    mArray.push_back(Cell{bdd});
    move_up(pos);
  }

  /// @brief 最小要素を取り出す．
  Bdd
  get_min()
  {
    auto bdd = mArray.front().mBdd;
    mArray[0] = mArray.back();
    mArray.pop_back();
    move_down(0);
    return bdd;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  struct Cell
  {
    Cell() = default;

    Cell(
      const Bdd& bdd
    ) : mBdd{bdd},
	mSize{bdd.size()}
    {
    }

    Bdd mBdd;
    SizeType mSize{0};
  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 上に移動させる．
  void
  move_up(
    SizeType cur_pos ///< [in] 現在の位置
  )
  {
    while ( cur_pos > 0 ) {
      auto cur_cell = mArray[cur_pos];
      auto cur_size = cur_cell.mSize;
      auto parent_pos = (cur_pos + 1) / 2;
      auto parent_cell = mArray[parent_pos];
      auto parent_size = parent_cell.mSize;
      if ( parent_size <= cur_size ) {
	break;
      }
      mArray[parent_pos] = cur_cell;
      mArray[cur_pos] = parent_cell;
      cur_pos = parent_pos;
    }
  }

  /// @brief 下に移動させる．
  void
  move_down(
    SizeType cur_pos ///< [in] 現在の位置
  )
  {
    while ( true ) {
      auto cur_cell = mArray[cur_pos];
      auto cur_size = cur_cell.mSize;
      auto right_pos = (cur_pos + 1) * 2;
      auto left_pos = right_pos - 1;
      if ( mArray.size() <= left_pos ) {
	break;
      }
      // より小さいサイズを持つ子供を見つける．
      auto child_cell = mArray[left_pos];
      auto child_pos = left_pos;
      if ( mArray.size() > right_pos ) {
	auto right_cell = mArray[right_pos];
	if ( child_cell.mSize > right_cell.mSize ) {
	  child_cell = right_cell;
	  child_pos = right_pos;
	}
      }
      // それと比較する．
      if ( cur_size > child_size ) {
	mArray[cur_pos] = child_cell;
	mArray[child_pos] = cur_cell;
	cur_pos = child_pos;
      }
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 配列本体
  vector<Cell> mArray;

};


END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス CnfGenImpl2
//////////////////////////////////////////////////////////////////////

// @brief 式を CNF に変換する．
void
CnfGenImpl2::make_cnf(
  const DetCond& cond,
  vector<SatLiteral>& assumptions
)
{
  // mandatory_condition は直接 assumptions に変換する．

  // cube_list をキューブごとにBDDに変換し，
  // サイズの小さい順にマージしていく
  HeapTree bdd_heap;
  bdd_list.reserve(cond.cube_list().size());
  for ( auto& cube: cond.cube_list() ) {
    auto bdd1 = cube_to_bdd(cube);
    bdd_heap.put(bdd1);
  }
  // サイズオーバーとなった Bdd を入れておくリスト
  vector<Bdd> bdd_list;
  bdd_list.reserve(bdd_heap.size());
  while ( bdd_heap.size() > 1 ) {
    auto bdd1 = bdd_heap.get_min();
    auto bdd2 = bdd_heap.get_min();
    auto bdd3 = bdd1 | bdd2;
    if ( bdd3.size() > size_limit ) {
      bdd_list.push_back(bdd3);
    }
    else {
      bdd_heap.put(bdd3);
    }
  }
  if ( bdd_heap.size() == 1 ) {
    auto bdd1 = bdd_heap.get_min();
    bdd_list.push_back(bdd1);
  }
  // bdd_list の BDD の OR が求めるべき論理式
  auto n = bdd_list.size();
  if ( n == 0 ) {
    abort();
  }
  else if ( n == 1 ) {
    auto bdd = bdd_list.front();
    bdd_to_cnf(bdd, assumptions);
  }
  else {
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(n + 1);
    auto new_lit = mEngine.solver().new_variable(false);
    tmp_lits.push_back(~new_lit);
    for ( auto bdd: bdd_list ) {
      vector<SatLiteral> and_lits;
      bdd_to_cnf(bdd, and_lits);
      auto new_lit = mEngine.solver().new_variable(false);
      tmp_lits.push_back(new_lit);
      for ( auto lit: and_lits ) {
	mEngine.solver().add_clause(~new_lit, lit);
      }
    }
    assumptions.push_back(new_lit);
  }
}

// @brief 式を CNF に変換した際の項数とリテラル数を計算する．
CnfSize
CnfGenImpl2::calc_cnf_size(
  const CondGen& cond
)
{
  auto bdd = conv_to_bdd(expr);
  BddMgr bdd_mgr;
  auto bdd = bdd_mgr.from_expr(expr);
  BgMgr mgr{bdd};
  auto size = CnfSize::zero();
  for ( auto node: mgr.node_list() ) {
    size += node->cnf_size();
  }
  {
    cout << "Expr literal: " << expr.literal_num() << endl
	 << "BDD size:     " << bdd.size() << endl
	 << "BgNode size:  " << mgr.node_list().size() << endl
	 << "CNF size:     " << size.clause_num
	 << ", " << size.literal_num << endl;
  }
  return size;
}

// @brief キューブを BDD に変換する．
Bdd
CnfGenImpl2::conv_to_bdd(
  const AssignList& cube
)
{
  auto ans = mBddMgr.one();
  for ( auto as: cube ) {
    auto node = as.node();
    auto time = as.time();
    auto varid = node->id() * 2 + time;
    auto bdd1 = mBddMgr.variable(varid);
    if ( !as.val() ) {
      bdd1 = ~bdd1;
    }
    ans &= bdd1;
  }
  return ans;
}

// @brief 論理式を BDD に変換する．
Bdd
CnfGenImpl2::conv_to_bdd(
  const DetCond& cond
)
{
  if ( expr.is_zero() ) {
    return mBddMgr.zero();
  }
  if ( expr.is_one() ) {
    return mBddMgr.one();
  }
  if ( expr.is_literal() ) {
    auto lit = expr.literal();
    auto varid = lit.varid();
    auto inv = lit.is_negative();
    auto bdd = static_cast<Bdd>(mBddMgr.variable(varid));
    if ( inv ) {
      bdd = ~bdd;
    }
    return bdd;
  }
  auto n = expr.operand_num();
  if ( expr.is_and() ) {
    auto bdd = mBddMgr.one();
    for ( SizeType i = 0; i < n; ++ i ) {
      bdd &= conv_to_bdd(expr.operand(i));
    }
    return bdd;
  }
  if ( expr.is_or() ) {
    auto bdd = mBddMgr.zero();
    for ( SizeType i = 0; i < n; ++ i ) {
      bdd |= conv_to_bdd(expr.operand(i));
    }
    return bdd;
  }
  if ( expr.is_xor() ) {
    auto bdd = mBddMgr.zero();
    for ( SizeType i = 0; i < n; ++ i ) {
      bdd ^= conv_to_bdd(expr.operand(i));
    }
    return bdd;
  }
  ASSERT_NOT_REACHED;
  return mBddMgr.zero();
}

// @brief BDD を CNF 式に変換する．
void
CnfGenImpl2::bdd_to_cnf(
  const Bdd& bdd,
  vector<SatLiteral>& lit_list
)
{
  if ( mResultDict.count(bdd) > 0 ) {
    lit_list = mResultDict.at(bdd);
    return;
  }
  Bdd bdd0;
  Bdd bdd1;
  auto root_var = bdd.root_decomp(bdd0, bdd1);
  auto lit = conv_to_literal(root_var);
  if ( bdd0.is_zero() ) {
    if ( bdd1.is_one() ) {
      lit_list.push_back(lit);
    }
    else {
      bdd_to_cnf(bdd1, lit_list);
      lit_list.push_back(lit);
    }
  }
  else if ( bdd0.is_one() ) {
    if ( bdd1.is_zero() ) {
      lit_list.push_back(~lit);
    }
    else {
      vector<SatLiteral> lit_list1;
      bdd_to_cnf(bdd1, lit_list1);
      auto new_lit = mEngine.solver().new_variable(true);
      for ( auto lit1: lit_list1 ) {
	mEngine.solver().add_clause(~new_lit, ~lit, lit1);
      }
      lit_list.push_back(new_lit);
    }
  }
  else {
    if ( bdd1.is_zero() ) {
      // bdd0 は定数ではない．
      bdd_to_cnf(bdd0, lit_list);
      lit_list.push_back(~lit);
    }
    else if ( bdd1.is_one() ) {
      vector<SatLiteral> lit_list0;
      bdd_to_cnf(bdd0, lit_list0);
      auto new_lit = mEngine.solver().new_variable(true);
      for ( auto lit1: lit_list0 ) {
	mEngine.solver().add_clause(~new_lit, lit, lit1);
      }
      lit_list.push_back(new_lit);
    }
    else {
      vector<SatLiteral> lit_list0;
      bdd_to_cnf(bdd0, lit_list0);
      vector<SatLiteral> lit_list1;
      bdd_to_cnf(bdd1, lit_list1);
      auto new_lit = mEngine.solver().new_variable(true);
      for ( auto lit1: lit_list0 ) {
	mEngine.solver().add_clause(~new_lit, lit, lit1);
      }
      for ( auto lit1: lit_list1 ) {
	mEngine.solver().add_clause(~new_lit, ~lit, lit1);
      }
      lit_list.push_back(new_lit);
    }
  }
  mResultDict.emplace(bdd, lit_list);
}

// @brief 論理式のリテラルを SAT ソルバのリテラルに変換する．
SatLiteral
CnfGenImpl2::conv_to_literal(
  const BddVar& var
) const
{
  // 回りくどい変換を行う．
  auto varid = var.id();
  auto node_id = varid / 2;
  auto node = mEngine.network().node(node_id);
  auto time = static_cast<int>(varid % 2);
  auto as = Assign{node, time, true};
  return mEngine.conv_to_literal(as);
}

END_NAMESPACE_DRUID
