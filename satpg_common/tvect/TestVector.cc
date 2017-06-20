﻿
/// @file TestVector.cc
/// @brief TestVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TestVector.h"
#include "NodeValList.h"
#include "TpgDff.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] input_num 入力数
// @param[in] dff_num DFF数
// @param[in] td_mode 遷移故障用の時 true にするフラグ
TestVector::TestVector(ymuint input_num,
		       ymuint dff_num,
		       bool td_mode) :
  mInputNum((input_num << 1) | static_cast<ymuint>(td_mode)),
  mDffNum(dff_num)
{
  // X に初期化しておく．
  ymuint nb = block_num(vect_len());
  for (ymuint i = 0; i < nb; ++ i) {
    mPat[i] = kPvAll0;
  }

  // マスクを設定する．
  ymuint k = vect_len() % kPvBitLen;
  mMask = kPvAll1 << (kPvBitLen - k);
}

// @brief デストラクタ
TestVector::~TestVector()
{
}

// @brief X の個数を得る．
ymuint
TestVector::x_num() const
{
  ymuint nb = block_num(vect_len());
  ymint n = 0;
  for (ymuint i = 0; i < nb; i += 2) {
    PackedVal v = mPat[i] | mPat[i + 1];
    // v 中の1の数を数える．
    n += count_ones(v);
  }
  return vect_len() - n;
}

// @brief 2つのベクタが両立しないとき true を返す．
bool
TestVector::is_conflict(const TestVector& tv1,
			const TestVector& tv2)
{
  ASSERT_COND( tv1.mInputNum == tv2.mInputNum );

  ymuint nb = block_num(tv1.vect_len());
  for (ymuint i = 0; i < nb; i += 2) {
    ymuint i0 = i;
    ymuint i1 = i + 1;
    PackedVal diff0 = (tv1.mPat[i0] ^ tv2.mPat[i0]);
    PackedVal diff1 = (tv1.mPat[i1] ^ tv2.mPat[i1]);
    if ( (diff0 & diff1) != kPvAll0 ) {
      return true;
    }
  }
  return false;
}

// @brief 等価関係の比較を行なう．
// @param[in] right オペランド
// @return 自分自身と right が等しいとき true を返す．
bool
TestVector::operator==(const TestVector& right) const
{
  ASSERT_COND( mInputNum == right.mInputNum );

  ymuint nb = block_num(vect_len());
  for (ymuint i = 0; i < nb; ++ i) {
    if ( mPat[i] != right.mPat[i] ) {
      return false;
    }
  }
  return true;
}

// @brief 包含関係の比較を行なう
// @param[in] right オペランド
// @return minterm の集合として right が自分自身を含んでいたら true を返す．
// @note false だからといって逆に自分自身が right を含むとは限らない．
bool
TestVector::operator<(const TestVector& right) const
{
  ASSERT_COND( mInputNum == right.mInputNum );

  ymuint nb = block_num(vect_len());
  bool diff = false;
  for (ymuint i = 0; i < nb; ++ i) {
    PackedVal val1 = mPat[i];
    PackedVal val2 = right.mPat[i];
    if ( (~val1 & val2) != kPvAll0 ) {
      return false;
    }
    if ( val1 != val2 ) {
      diff = true;
    }
  }
  return diff;
}

// @brief 包含関係の比較を行なう
// @param[in] right オペランド
// @return minterm の集合として right が自分自身を含んでいたら true を返す．
// @note こちらは等しい場合も含む．
// @note false だからといって逆に自分自身が right を含むとは限らない．
bool
TestVector::operator<=(const TestVector& right) const
{
  ASSERT_COND( mInputNum == right.mInputNum );

  ymuint nb = block_num(vect_len());
  for (ymuint i = 0; i < nb; ++ i) {
    PackedVal val1 = mPat[i];
    PackedVal val2 = right.mPat[i];
    if ( (~val1 & val2) != kPvAll0 ) {
      return false;
    }
  }
  return true;
}

// @brief すべて未定(X) で初期化する．
void
TestVector::init()
{
  ymuint nb = block_num(vect_len());
  for (ymuint i = 0; i < nb; i += 2) {
    ymuint i0 = i;
    ymuint i1 = i + 1;
    mPat[i0] = kPvAll0;
    mPat[i1] = kPvAll0;
  }
}

// @brief 割当リストから内容を設定する．
// @param[in] assign_list 割当リスト
//
// assign_list に外部入力以外の割当が含まれている場合無視する．
void
TestVector::set_from_assign_list(const NodeValList& assign_list)
{
  ymuint n = assign_list.size();
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assign_list[i];
    Val3 val = nv.val() ? kVal1 : kVal0;
    const TpgNode* node = nv.node();
    ASSERT_COND( node->is_ppi() );
    if ( is_sa_mode() ) {
      ASSERT_COND( nv.time() == 0 );

      ymuint id = node->input_id();
      set_ppi_val(id, val);
    }
    else {
      if ( node->is_primary_input() ) {
	ymuint id = node->input_id();
	if ( nv.time() == 1 ) {
	  ASSERT_COND( is_td_mode() );

	  set_aux_input_val(id, val);
	}
	else {
	  set_input_val(id, val);
	}
      }
      else if ( node->is_dff_output() ) {
	ASSERT_COND( nv.time() == 0 );

	ymuint id = node->dff()->id();
	set_dff_val(id, val);
      }
    }
  }
}

// @brief HEX文字列から内容を設定する．
// @param[in] hex_string HEX 文字列
// @return hex_string に不適切な文字が含まれていたら false を返す．
// @note hex_string が短い時には残りは0で初期化される．
// @note hex_string が長い時には余りは捨てられる．
bool
TestVector::set_from_hex(const string& hex_string)
{
  // よく問題になるが，ここでは最下位ビット側から入力する．
  ymuint nl = hex_length(vect_len());
  ymuint sft = 0;
  ymuint blk = 0;
  PackedVal pat = kPvAll0;
  for (ymuint i = 0; i < nl; ++ i) {
    char c = (i < hex_string.size()) ? hex_string[i] : '0';
    PackedVal pat1 = kPvAll0;
    if ( '0' <= c && c <= '9' ) {
      pat1 = static_cast<PackedVal>(c - '0');
    }
    else if ( 'a' <= c && c <= 'f' ) {
      pat1 = static_cast<PackedVal>(c - 'a' + 10);
    }
    else if ( 'A' <= c && c <= 'F' ) {
      pat1 = static_cast<PackedVal>(c - 'A' + 10);
    }
    else {
      return false;
    }
    pat |= (pat1 << sft);
    sft += 4;
    if ( sft == kPvBitLen ) {
      mPat[blk] = ~pat;
      mPat[blk + 1] = pat;
      sft = 0;
      blk += 2;
      pat = kPvAll0;
    }
  }
  if ( sft != 0 ) {
    mPat[blk] = ~pat;
    mPat[blk + 1] = pat;
  }

  return true;
}

// @brief 乱数パタンを設定する．
// @param[in] randgen 乱数生成器
void
TestVector::set_from_random(RandGen& randgen)
{
  ymuint nb = block_num(vect_len());
  for (ymuint i = 0; i < nb; i += 2) {
    PackedVal v = randgen.uint64();
    ymuint i0 = i;
    ymuint i1 = i + 1;
    if ( i == nb - 2 ) {
      mPat[i0] = ~v & mMask;
      mPat[i1] =  v & mMask;
    }
    else {
      mPat[i0] = ~v;
      mPat[i1] =  v;
    }
  }
}

// @brief X の部分を乱数で 0/1 に設定する．
// @param[in] randgen 乱数生成器
void
TestVector::fix_x_from_random(RandGen& randgen)
{
  ymuint nb = block_num(vect_len());
  for (ymuint i = 0; i < nb; i += 2) {
    ymuint i0 = i;
    ymuint i1 = i + 1;
    PackedVal mask = ~(mPat[i0] | mPat[i1]);
    if ( i == nb - 2 ) {
      mask &= mMask;
    }
    if ( mask == kPvAll0 ) {
      continue;
    }
    PackedVal v = randgen.uint64();
    mPat[i0] |= ~v & mask;
    mPat[i1] |=  v & mask;
  }
}

// @brief テストベクタをコピーする．
// @param[in] src コピー元のテストベクタ
// @note X の部分はコピーしない．
void
TestVector::copy(const TestVector& src)
{
  ymuint nb = block_num(vect_len());
  for (ymuint i = 0; i < nb; i += 2) {
    ymuint i0 = i;
    ymuint i1 = i + 1;
    PackedVal mask = src.mPat[i0] | src.mPat[i1];
    mPat[i0] &= ~mask;
    mPat[i0] |= src.mPat[i0];
    mPat[i1] &= ~mask;
    mPat[i1] |= src.mPat[i1];
  }
}

// @breif テストベクタをマージする．
// @note X 以外で相異なるビットがあったら false を返す．
bool
TestVector::merge(const TestVector& src)
{
  ymuint nb = block_num(vect_len());

  // コンフリクトチェック
  for (ymuint i = 0; i < nb; i += 2) {
    ymuint i0 = i;
    ymuint i1 = i + 1;
    PackedVal diff0 = (mPat[i0] ^ src.mPat[i0]);
    PackedVal diff1 = (mPat[i1] ^ src.mPat[i1]);
    if ( (diff0 & diff1) != kPvAll0 ) {
      return false;
    }
  }

  // 実際のマージ
  for (ymuint i = 0; i < nb; i += 2) {
    ymuint i0 = i;
    ymuint i1 = i + 1;
    mPat[i0] |= src.mPat[i0];
    mPat[i1] |= src.mPat[i1];
  }
  return true;
}

// @brief 内容を BIN 形式で表す．
string
TestVector::bin_str() const
{
  // よく問題になるが，ここでは最下位ビット側から出力する．
  string ans;
  for (ymuint i = 0; i < vect_len(); ++ i) {
    switch ( _val(i) ) {
    case kVal0: ans += '0'; break;
    case kVal1: ans += '1'; break;
    case kValX: ans += 'X'; break;
    default:    ans += '-'; break; // ありえないけどバグで起こりうる．
    }
  }
  return ans;
}

// @brief 内容を HEX 形式で出力する．
string
TestVector::hex_str() const
{
  // よく問題になるが，ここでは最下位ビット側から出力する．
  ymuint tmp = 0U;
  ymuint bit = 1U;
  string ans;
  for (ymuint i = 0; ; ++ i) {
    if ( i < vect_len() ) {
      if ( _val(i) == kVal1 ) {
	// 面倒くさいので kValX は kVal0 と同じとみなす．
	tmp += bit;
      }
      bit <<= 1;
      if ( bit != 16U ) {
	continue;
      }
    }
    else if ( bit == 1U ) {
      break;
    }

    if ( tmp <= 9 ) {
      ans += static_cast<char>('0' + tmp);
    }
    else {
      ans += static_cast<char>('A' + tmp - 10);
    }
    bit = 1U;
    tmp = 0U;
  }
  return ans;
}

END_NAMESPACE_YM_SATPG
