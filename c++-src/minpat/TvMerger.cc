
/// @file TvMerger.cc
/// @brief TvMerger の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TvMerger.h"
#include "TestVector.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

inline
int
calc_nb(const vector<TestVector>& tv_list)
{
  if ( tv_list.empty() ) {
    return 0;
  }
  const TestVector& tv0 = tv_list[0];
  return tv0.vector_size();
}

// リストのユニオンの要素数を数える．
int
count_union(const vector<int>& list1,
	    const vector<int>& list2)
{
  int rpos1 = 0;
  int rpos2 = 0;
  int n1 = list1.size();
  int n2 = list2.size();
  int count = 0;
  while ( rpos1 < n1 && rpos2 < n2 ) {
    int v1 = list1[rpos1];
    int v2 = list2[rpos2];
    if ( v1 < v2 ) {
      ++ count;
      ++ rpos1;
    }
    else if ( v1 > v2 ) {
      ++ count;
      ++ rpos2;
    }
    else { // v1 == v2
      ++ count;
      ++ rpos1;
      ++ rpos2;
    }
  }
  count += (n1 - rpos1);
  count += (n2 - rpos2);

  return count;
}

// リストの差分の要素数を数える．
int
count_diff(const vector<int>& list1,
	   const vector<int>& list2)
{
  int rpos1 = 0;
  int rpos2 = 0;
  int n1 = list1.size();
  int n2 = list2.size();
  int count = 0;
  while ( rpos1 < n1 && rpos2 < n2 ) {
    int v1 = list1[rpos1];
    int v2 = list2[rpos2];
    if ( v1 < v2 ) {
      ++ count;
      ++ rpos1;
    }
    else if ( v1 > v2 ) {
      ++ rpos2;
    }
    else { // v1 == v2
      ++ rpos1;
      ++ rpos2;
    }
  }
  count += (n1 - rpos1);

  return count;
}

// リストをマージする．
void
merge_list(vector<int>& list1,
	   const vector<int>& list2)
{
  vector<int> old_list1(list1);
  int rpos1 = 0;
  int rpos2 = 0;
  int n1 = old_list1.size();
  int n2 = list2.size();
  list1.clear();
  list1.reserve(n1 + n2);
  while ( rpos1 < n1 && rpos2 < n2 ) {
    int v1 = old_list1[rpos1];
    int v2 = list2[rpos2];
    if ( v1 <= v2 ) {
      list1.push_back(v1);
      ++ rpos1;
      if ( v1 == v2 ) {
	++ rpos2;
      }
    }
    else { // v1 > v2
      list1.push_back(v2);
      ++ rpos2;
    }
  }
  for ( ; rpos1 < n1; ++ rpos1 ) {
    int v1 = old_list1[rpos1];
    list1.push_back(v1);
  }
  for ( ; rpos2 < n2; ++ rpos2 ) {
    int v2 = list2[rpos2];
    list1.push_back(v2);
  }
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス TvMerger
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] tv_list 元のテストベクタのリスト
TvMerger::TvMerger(const vector<TestVector>& tv_list) :
  mOrigTvList(tv_list),
  mBitLen(calc_nb(mOrigTvList)),
  mBlockListArray(mBitLen * 2),
  mTabuList(mBitLen, -1)
{
  for ( auto i: Range(mOrigTvList.size()) ) {
    auto tv = mOrigTvList[i];
    for ( auto bit: Range(mBitLen) ) {
      Val3 val = tv.val(bit);
      if ( val == Val3::_0 ) {
	// この位置の値が 1 になるとブロックされる．
	_block_list(bit, 1).push_back(i);
      }
      else if ( val == Val3::_1 ) {
	// この位置の値が 0 になるとブロックされる．
	_block_list(bit, 0).push_back(i);
      }
      // Val3::_X の場合は関係ない．
    }
  }

  // 0/1 の両方のブロックリストが空でないビットを求める．
  mMaxNum = 0;
  mBitList.reserve(mBitLen);
  for ( auto b: Range(mBitLen) ) {
    int n0 = block_list(b, 0).size();
    int n1 = block_list(b, 1).size();
    if ( mMaxNum < n0 ) {
      mMaxNum = n0;
    }
    if ( mMaxNum < n1 ) {
      mMaxNum = n1;
    }
    if ( n0 > 0 && n1 > 0 ) {
      mBitList.push_back(b);
    }
  }
  ++ mMaxNum;
}

// @brief デストラクタ
TvMerger::~TvMerger()
{
}

// @brief 極大両立集合のリストを求める．
// @param[out] new_tv_list マージして生成したテストベクタのリスト
void
TvMerger::gen_mcset(vector<TestVector>& new_tv_list)
{
  new_tv_list.clear();

  // 現在選択されているシグネチャ
  // -1 が未選択，0/1 が選択されている値
  vector<int> signature(mBitLen, -1);

  unordered_set<string> tv_hash;

  int limit = 10000;
  int tenure = 5;
  for ( auto count: Range(limit) ) {
    // 極大集合を求める．
    greedy_mcset(signature, count);
    // 記録する．
    TestVector tv = gen_vector(signature);
    string tv_str = tv.bin_str();
    if ( tv_hash.count(tv_str) == 0 ) {
      tv_hash.emplace(tv_str);
      new_tv_list.push_back(tv);
    }

    // signature から取り除く要素を選ぶ．
    int bit = select_bit(signature);
    signature[bit] = -1;

    // bit をタブーリストに入れる．
    mTabuList[bit] = count + tenure;
  }
}

// @brief 極大集合を1つ求める．
// @param[inout] signature シグネチャ
void
TvMerger::greedy_mcset(vector<int>& signature,
		       int count)
{
  // 現在選択されている集合でブロックされている要素のリスト
  vector<int> cur_block_list;
  cur_block_list.reserve(mOrigTvList.size());

  // cur_block_list を作る．
  for ( auto bit: mBitList ) {
    if ( signature[bit] == 0 ) {
      merge_list(cur_block_list, block_list(bit, 0));
    }
    else if ( signature[bit] == 1 ) {
      merge_list(cur_block_list, block_list(bit, 1));
    }
  }

  // 未選択のビットのうち，新規に追加した時にブロックリストの増加が
  // 最小となるビット位置を求める．
  for ( ; ; ) {
    int min_num = mMaxNum;
    int min_bit = -1;
    int min_val = -1;
    for ( auto bit: mBitList ) {
      if ( signature[bit] != -1 ) {
	continue;
      }
      if ( mTabuList[bit] > count ) {
	continue;
      }
      int n0 = count_diff(block_list(bit, 0), cur_block_list);
      int n1 = count_diff(block_list(bit, 1), cur_block_list);
      if ( n0 == 0 && n1 == 0 ) {
	signature[bit] = 2;
      }
      if ( min_num > n0 ) {
	min_bit = bit;
	min_num = n0;
	min_val = 0;
      }
      if ( min_num > n1 ) {
	min_bit = bit;
	min_num = n1;
	min_val = 1;
      }
    }
    if ( min_val == -1 ) {
      break;
    }

    // シグネチャを更新する．
    signature[min_bit] = min_val;

    // cur_block_list を更新する．
    merge_list(cur_block_list, block_list(min_bit, min_val));
  }
}

// @brief シグネチャから最も価値の低いビットを選ぶ．
int
TvMerger::select_bit(const vector<int>& signature)
{
  // 値を持っているビット位置のリスト
  vector<int> bit_list;
  bit_list.reserve(mBitLen);
  for ( auto bit: Range(mBitLen) ) {
    auto s = signature[bit];
    if ( s == 0 || s == 1 ) {
      bit_list.push_back(bit);
    }
  }
  int nb = bit_list.size();

  // 0 - (i - 1) までのブロックリストのユニオンを計算する．
  vector<vector<int>> accum_block_list_array1(nb);
  vector<int> cur_block_list;
  for ( auto i: Range(nb) ) {
    accum_block_list_array1[i] = cur_block_list;
    int bit = bit_list[i];
    int val = signature[bit];
    merge_list(cur_block_list, block_list(bit, val));
  }

  // (nb - 1) - (i + 1) までのブロックリストのユニオンを計算する．
  vector<vector<int>> accum_block_list_array2(nb);
  cur_block_list.clear();
  for ( int i: Range_<int, -1>(nb - 1, -1) ) {
    accum_block_list_array2[i] = cur_block_list;
    int bit = bit_list[i];
    int val = signature[bit];
    merge_list(cur_block_list, block_list(bit, val));
  }

  // i 番目の要素を取り除いたときのブロックリストは
  // accum_block_list_array1[i] U accum_block_list_array2[i]
  // で与えられる．

  int min_num = cur_block_list.size();
  int min_bit = -1;
  for ( auto i: Range(nb) ) {
    int n = count_union(accum_block_list_array1[i], accum_block_list_array2[i]);
    if ( min_num > n ) {
      min_num = n;
      min_bit = bit_list[i];
    }
  }

  return min_bit;
}

// @brief シグネチャからテストベクタを作る．
TestVector
TvMerger::gen_vector(const vector<int>& signature)
{
  vector<TestVector> tmp_list;
  tmp_list.reserve(mOrigTvList.size());
  for ( auto tv: mOrigTvList ) {
    if ( check_compatible(tv, signature) ) {
      tmp_list.push_back(tv);
    }
  }
  return merge(tmp_list);
}

// @brief テストベクタとシグネチャが両立しているか調べる．
bool
TvMerger::check_compatible(const TestVector& tv,
			   const vector<int>& signature)
{
  for ( auto bit: Range(mBitLen) ) {
    int s = signature[bit];
    Val3 val = tv.val(bit);
    if ( s == 0 && val == Val3::_1 ) {
      return false;
    }
    if ( s == 1 && val == Val3::_0 ) {
      return false;
    }
  }
  return true;
}

END_NAMESPACE_DRUID
