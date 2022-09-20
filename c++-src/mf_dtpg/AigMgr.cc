
/// @file AigMgr.cc
/// @brief AigMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "AigMgr.h"
#include "AigNode.h"
#include "ym/Expr.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_JESTER

BEGIN_NONAMESPACE

// ハッシュ値を計算する．
SizeType
hash_func(const vector<AigHandle>& fanin_array)
{
  SizeType ans = 0U;
  for ( auto h: fanin_array ) {
    ans += h.val();
    ans *= 7;
  }
  return ans;
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス AigMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
AigMgr::AigMgr()
{
  expand_table(1024);
}

// @brief デストラクタ
AigMgr::~AigMgr()
{
  for ( auto node: mNodeList ) {
    delete node;
  }
}

// @brief 定数０のハンドルを返す．
AigHandle
AigMgr::make_zero()
{
  return AigHandle(nullptr, false);
}

// @brief 定数１のハンドルを返す．
AigHandle
AigMgr::make_one()
{
  return AigHandle(nullptr, true);
}

// @brief 入力ノードのハンドルを返す．
AigHandle
AigMgr::make_input()
{
  int id = mNodeList.size();
  AigNode* node = new AigNode(id);
  reg_node(node);
  return AigHandle(node, false);
}

// @brief ANDノードのハンドルを返す．
// @param[in] fanin_array ファンインのハンドルの配列
AigHandle
AigMgr::make_and(const vector<AigHandle>& fanin_array)
{
  return make_and_sub(fanin_array, false);
}

// @brief ORノードのハンドルを返す．
// @param[in] fanin_array ファンインのハンドルの配列
AigHandle
AigMgr::make_or(const vector<AigHandle>& fanin_array)
{
  return ~make_and_sub(fanin_array, true);
}

// @brief XORノードのハンドルを返す．
// @param[in] fanin_array ファンインのハンドルの配列
AigHandle
AigMgr::make_xor(const vector<AigHandle>& fanin_array)
{
  int n = fanin_array.size();
  return make_xor_sub(fanin_array, 0, n);
}

// @brief ANDノードのハンドルを返す．
// @param[in] fanin_array ファンインのハンドルの配列
// @param[in] fanin_inv ファンインを反転させる時 true にするフラグ
AigHandle
AigMgr::make_and_sub(const vector<AigHandle>& fanin_array,
		     bool fanin_inv)
{
  // 内容をチェックしながら fanin_array を tmp_fanin にコピーする．
  int ni = fanin_array.size();
  vector<AigHandle> tmp_fanin;
  HashMap<int, bool> pol_hash;
  tmp_fanin.reserve(ni);
  for ( auto h: fanin_array ) {
    if ( fanin_inv ) {
      h = ~h;
    }
    if ( h.is_zero() ) {
      return make_zero();
    }
    else if ( h.is_one() ) {
      // この入力は無視する．
      ;
    }
    else {
      int node_id = h.node()->id();
      bool pol;
      if ( pol_hash.find(node_id, pol) ) {
	if ( pol != h.inv() ) {
	  // 同じノードに対する相反する極性があった．
	  return make_zero();
	}
      }
      else {
	tmp_fanin.push_back(h);
	pol_hash.add(node_id, h.inv());
      }
    }
  }
  if ( tmp_fanin.empty() ) {
    return make_one();
  }
  if ( tmp_fanin.size() == 1 ) {
    return tmp_fanin[0];
  }

  // tmp_fanin をソートする．
  sort(tmp_fanin.begin(), tmp_fanin.end());

  // ハッシュ値を求める．
  SizeType hash_val = hash_func(tmp_fanin);
  SizeType index = hash_val % mHashTable.size();
  for ( auto node = mHashTable[index]; node; node = node->mLink ) {
    if ( node->check_equal(tmp_fanin) ) {
      // 同じノードを見つけた．
      return AigHandle(node);
    }
  }

  // 新しいノードを作る．
  int id = mNodeList.size();
  AigNode* node = new AigNode(id, std::move(tmp_fanin));
  reg_node(node);

  if ( mNodeList.size() >= mNextLimit ) {
    // ハッシュ表を拡大する．
    int n = mHashTable.size() * 2;
    expand_table(n);
  }

  // 新しいノードをハッシュ表を登録する．
  index = hash_val % mHashTable.size();
  node->mLink = mHashTable[index];
  mHashTable[index] = node;

  return AigHandle(node);
}

// @brief make_xor() の下請け関数
AigHandle
AigMgr::make_xor_sub(const vector<AigHandle>& fanin_array,
		     int start,
		     int end)
{
  int n = end - start;
  ASSERT_COND( n > 0 );

  if ( n == 1 ) {
    return fanin_array[start];
  }
  else if ( n == 2 ) {
    AigHandle h0 = fanin_array[start + 0];
    AigHandle h1 = fanin_array[start + 1];
    AigHandle h2 = make_and_sub(vector<AigHandle>{ h0, ~h1}, false);
    AigHandle h3 = make_and_sub(vector<AigHandle>{~h0,  h1}, false);
    AigHandle h = ~make_and_sub(vector<AigHandle>{h2, h3}, true);
    return h;
  }
  else {
    int nh = n / 2;
    AigHandle h1 = make_xor_sub(fanin_array, start, start + nh);
    AigHandle h2 = make_xor_sub(fanin_array, start + nh, end);
    return make_xor_sub(vector<AigHandle>{h1, h2}, 0, 2);
  }
}

// @brief EXPRノードのハンドルを返す．
// @param[in] expr 論理式
// @param[in] fanin_array ファンインのハンドルの配列
AigHandle
AigMgr::make_expr(const Expr& expr,
		  const vector<AigHandle>& fanin_array)
{
  if ( expr.is_zero() ) {
    return make_zero();
  }
  else if ( expr.is_one() ) {
    return make_one();
  }
  else if ( expr.is_posi_literal() ) {
    VarId var = expr.varid();
    return fanin_array[var.val()];
  }
  else if ( expr.is_nega_literal() ) {
    VarId var = expr.varid();
    return ~fanin_array[var.val()];
  }
  else {
    int nc = expr.child_num();
    vector<AigHandle> tmp_fanin(nc);
    for ( int i: Range(nc) ) {
      tmp_fanin[i] = make_expr(expr.child(i), fanin_array);
    }
    if ( expr.is_and() ) {
      return make_and(tmp_fanin);
    }
    else if ( expr.is_or() ) {
      return make_or(tmp_fanin);
    }
    else if ( expr.is_xor() ) {
      return make_xor(tmp_fanin);
    }
    else {
      ASSERT_NOT_REACHED;
    }
  }
  return make_zero();
}

// @brief ハッシュ表を拡大する．
// @param[in] req_size 要求サイズ
void
AigMgr::expand_table(int req_size)
{
  vector<AigNode*> new_table(req_size);
  mNextLimit = static_cast<int>(req_size * 1.8);
  for ( auto node: mHashTable ) {
    for ( auto node1 = node; node1; ) {
      auto tmp = node1;
      node1 = node1->mLink;
      SizeType hash_val = hash_func(tmp->mFaninArray);
      SizeType index = hash_val % req_size;
      tmp->mLink = new_table[index];
      new_table[index] = tmp;
    }
  }
  mHashTable.swap(new_table);
}

// @brief ノードを登録する．
void
AigMgr::reg_node(AigNode* node)
{
  mNodeList.push_back(node);

}


//////////////////////////////////////////////////////////////////////
// クラス AigNode
//////////////////////////////////////////////////////////////////////

// @brief ファンインが同一か調べる．
// @param[in] fanin_array 比較対象のファンイン配列
bool
AigNode::check_equal(const vector<AigHandle>& fanin_array) const
{
  int n = fanin_num();
  if ( fanin_array.size() != n ) {
    return false;
  }
  for ( int i: Range(n) ) {
    if ( fanin_array[i] != mFaninArray[i] ) {
      return false;
    }
  }
  return true;
}

END_NAMESPACE_JESTER
