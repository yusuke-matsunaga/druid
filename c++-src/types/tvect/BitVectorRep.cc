
/// @file BitVectorRep.cc
/// @brief BitVectorRep の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "BitVectorRep.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

inline
void
_check_len(
  const BitVectorRep& bv1,
  const BitVectorRep& bv2
)
{
  if ( bv1.len() != bv2.len() ) {
    throw std::invalid_argument{"bv1.len() != bv2.len()"};
  }
}

END_NONAMESPACE

// @brief ベクタ長を指定してオブジェクトを作る．
std::shared_ptr<BitVectorRep>
BitVectorRep::new_vector(
  SizeType len
)
{
  auto size = sizeof(BitVectorRep) + sizeof(PackedVal) * block_num(len);
  void* p = new char[size];
  return std::shared_ptr<BitVectorRep>{new (p) BitVectorRep(len)};
}

// @brief 内容をコピーする．
std::shared_ptr<BitVectorRep>
BitVectorRep::new_vector(
  const BitVectorRep& src
)
{
  auto rep = new_vector(src.len());

  auto n = block_num(src.len());
  for ( auto i: Range_<SizeType>(0, n) ) {
    rep->mPat[i] = src.mPat[i];
  }
  return rep;
}

// @brief コンストラクタ
BitVectorRep::BitVectorRep(
  SizeType vlen
) : mLength{vlen}
{
  // X に初期化しておく．
  init();
}

// @brief X の個数を得る．
SizeType
BitVectorRep::x_count() const
{
  auto nb = block_num(len());
  SizeType n = 0;
  for ( auto i: Range_<SizeType, 2>(0, nb) ) {
    auto i0 = i;
    auto i1 = i + 1;
    auto pat0 = mPat[i0];
    auto pat1 = mPat[i1];
    auto v = pat0 & pat1;
    // v 中の1の数を数える．
    n += count_ones(v);
  }
  return n;
}

// @brief 2つのビットベクタの等価比較を行う．
bool
BitVectorRep::is_eq(
  const BitVectorRep& bv1,
  const BitVectorRep& bv2
)
{
  _check_len(bv1, bv2);

  auto nb = block_num(bv1.len());
  for ( auto i: Range_<SizeType>(0, nb) ) {
    if ( bv1.mPat[i] != bv2.mPat[i] ) {
      return false;
    }
  }
  return true;
}

// @brief 2つのビットベクタの包含関係を調べる．
bool
BitVectorRep::is_lt(
  const BitVectorRep& bv1,
  const BitVectorRep& bv2
)
{
  _check_len(bv1, bv2);

  auto nb = block_num(bv1.len());
  bool diff = false;
  for ( auto i: Range_<SizeType, 2>(0, nb) ) {
    auto i0 = i;
    auto i1 = i + 1;
    auto val1_0 = bv1.mPat[i0];
    auto val1_1 = bv1.mPat[i1];
    auto val2_0 = bv2.mPat[i0];
    auto val2_1 = bv2.mPat[i1];
    if ( (val1_0 & ~val2_0) != PV_ALL0 ||
	 (val1_1 & ~val2_1) != PV_ALL0 ) {
      return false;
    }
    if ( val1_0 != val2_0 || val1_1 != val2_1 ) {
      diff = true;
    }
  }
  return diff;
}

// @brief 2つのビットベクタの包含関係を調べる．
bool
BitVectorRep::is_le(
  const BitVectorRep& bv1,
  const BitVectorRep& bv2
)
{
  _check_len(bv1, bv2);

  auto nb = block_num(bv1.len());
  for ( auto i: Range_<SizeType, 2>(0, nb) ) {
    auto i0 = i;
    auto i1 = i + 1;
    auto val1_0 = bv1.mPat[i0];
    auto val1_1 = bv1.mPat[i1];
    auto val2_0 = bv2.mPat[i0];
    auto val2_1 = bv2.mPat[i1];
    if ( (val1_0 & ~val2_0) != PV_ALL0 ||
	 (val1_1 & ~val2_1) != PV_ALL0 ) {
      return false;
    }
  }
  return true;
}

// @brief 2つのベクタが両立するとき true を返す．
bool
BitVectorRep::is_compat(
  const BitVectorRep& bv1,
  const BitVectorRep& bv2
)
{
  _check_len(bv1, bv2);

  auto nb = block_num(bv1.len());
  for ( auto i: Range_<SizeType, 2>(0, nb) ) {
    auto i0 = i;
    auto i1 = i + 1;
    // 0 のビットと 1 のビットの両方が異なっていると
    // コンフリクトしている．
    auto diff0 = (bv1.mPat[i0] ^ bv2.mPat[i0]);
    auto diff1 = (bv1.mPat[i1] ^ bv2.mPat[i1]);
    if ( (diff0 & diff1) != PV_ALL0 ) {
      return false;
    }
  }
  return true;
}

// @brief すべて未定(X) で初期化する．
void
BitVectorRep::init()
{
  auto nb = block_num(len());
  for ( auto i: Range_<SizeType, 2>(0, nb) ) {
    if ( i < nb - 2 ) {
      mPat[i + 0] = PV_ALL1;
      mPat[i + 1] = PV_ALL1;
    }
    else {
      auto mask = get_mask();
      mPat[i + 0] = mask;
      mPat[i + 1] = mask;
    }
  }
}

// @brief 1ビット左シフトを行う．
void
BitVectorRep::lshift(
  Val3 new_val
)
{
  PackedVal v0 = 1UL;
  PackedVal v1 = 1UL;
  switch ( new_val ) {
  case Val3::_0: v1 = 0UL; break;
  case Val3::_1: v0 = 0UL; break;
  default: break;
  }
  auto nb = block_num(len());
  for ( auto i: Range_<SizeType, 2>(0, nb) ) {
    auto& pat0 = mPat[i + 0];
    auto& pat1 = mPat[i + 1];
    if ( i < nb - 2 ) {
      auto new_v0 = pat0 >> (PV_BITLEN - 1);
      auto new_v1 = pat1 >> (PV_BITLEN - 1);
      pat0 = (pat0 << 1) | v0;
      pat1 = (pat1 << 1) | v1;
      v0 = new_v0;
      v1 = new_v1;
    }
    else {
      pat0 = (pat0 << 1) | v0;
      pat1 = (pat1 << 1) | v1;
      auto mask = get_mask();
      pat0 &= mask;
      pat1 &= mask;
    }
  }
}

// @brief 1ビット右シフトを行う．
void
BitVectorRep::rshift(
  Val3 new_val
)
{
  PackedVal v0 = 1UL;
  PackedVal v1 = 1UL;
  switch ( new_val ) {
  case Val3::_0: v1 = 0UL; break;
  case Val3::_1: v0 = 0UL; break;
  default: break;
  }
  auto s = shift_num(len() - 1);
  v0 <<= s;
  v1 <<= s;
  auto nb = block_num(len());
  for ( auto i: Range_<SizeType, 2>(0, nb) ) {
    auto ri = nb - i - 2;
    auto& pat0 = mPat[ri + 0];
    auto& pat1 = mPat[ri + 1];
    auto new_v0 = (pat0 & 1UL) << (PV_BITLEN - 1);
    auto new_v1 = (pat1 & 1UL) << (PV_BITLEN - 1);
    pat0 = (pat0 >> 1) | v0;
    pat1 = (pat1 >> 1) | v1;
    v0 = new_v0;
    v1 = new_v1;
  }
}

// @brief BIN文字列から内容を設定する．
bool
BitVectorRep::set_from_bin(
  const std::string& bin_string
)
{
  // よく問題になるが，ここでは最下位ビット側から入力する．
  auto nl = len();
  SizeType sft = 0;
  SizeType blk = 0;
  auto pat0 = PV_ALL0;
  auto pat1 = PV_ALL0;
  for ( auto i: Range_<SizeType>(0, nl) ) {
    char c = (i < bin_string.size()) ? bin_string[i] : 'X';
    PackedVal b0;
    PackedVal b1;
    switch ( c ) {
    case '0': b0 = 1ULL; b1 = 0ULL; break;
    case '1': b0 = 0ULL; b1 = 1ULL; break;
    case 'x':
    case 'X': b0 = 1ULL; b1 = 1ULL; break;
    default: // エラー
      return false;
    }
    pat0 |= (b0 << sft);
    pat1 |= (b1 << sft);
    ++ sft;
    if ( sft == PV_BITLEN ) {
      mPat[blk + 0] = pat0;
      mPat[blk + 1] = pat1;
      sft = 0;
      blk += 2;
      pat0 = PV_ALL0;
      pat1 = PV_ALL0;
    }
  }
  if ( sft != 0 ) {
    mPat[blk + 0] = pat0;
    mPat[blk + 1] = pat1;
  }

  return true;
}

// @brief HEX文字列から内容を設定する．
bool
BitVectorRep::set_from_hex(
  const std::string& hex_string
)
{
  // よく問題になるが，ここでは最下位ビット側から入力する．
  auto nl = hex_length(len());
  SizeType sft = 0;
  SizeType blk = 0;
  auto pat = PV_ALL0;
  for ( auto i: Range_<SizeType>(0, nl) ) {
    char c = (i < hex_string.size()) ? hex_string[i] : 'X';
    auto pat1 = PV_ALL0;
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
    if ( sft == PV_BITLEN ) {
      mPat[blk + 0] = ~pat;
      mPat[blk + 1] =  pat;
      sft = 0;
      blk += 2;
      pat = PV_ALL0;
    }
  }
  if ( sft != 0 ) {
    mPat[blk + 0] = ~pat;
    mPat[blk + 1] =  pat;
  }

  return true;
}

// @brief ビットベクタをマージする．
bool
BitVectorRep::merge(
  const BitVectorRep& src
)
{
  _check_len(*this, src);

  auto nb = block_num(len());

  // コンフリクトチェック
  for ( auto i: Range_<SizeType, 2>(0, nb) ) {
    auto i0 = i;
    auto i1 = i + 1;
    auto diff0 = (mPat[i0] ^ src.mPat[i0]);
    auto diff1 = (mPat[i1] ^ src.mPat[i1]);
    if ( (diff0 & diff1) != PV_ALL0 ) {
      return false;
    }
  }

  // 実際のマージ
  for ( auto i: Range_<SizeType, 2>(0, nb) ) {
    auto i0 = i;
    auto i1 = i + 1;
    mPat[i0] &= src.mPat[i0];
    mPat[i1] &= src.mPat[i1];
  }
  return true;
}

// @brief 内容を BIN 形式で表す．
std::string
BitVectorRep::bin_str() const
{
  // よく問題になるが，ここでは最下位ビット側から出力する．
  std::ostringstream buf;
  for ( auto i: Range_<SizeType>(0, len()) ) {
    switch ( val(i) ) {
    case Val3::_0: buf << '0'; break;
    case Val3::_1: buf << '1'; break;
    case Val3::_X: buf << 'X'; break;
    default:       buf << '-'; break; // ありえないけどバグで起こりうる．
    }
  }
  return buf.str();
}

// @brief 内容を HEX 形式で出力する．
std::string
BitVectorRep::hex_str() const
{
  // よく問題になるが，ここでは最下位ビット側から出力する．
  std::ostringstream buf;
  int tmp = 0U;
  int bit = 1U;
  for ( SizeType i = 0; ; ++ i ) {
    if ( i < len() ) {
      if ( val(i) == Val3::_1 ) {
	// 面倒くさいので Val3::X は Val3::_0 と同じとみなす．
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
      buf << static_cast<char>('0' + tmp);
    }
    else {
      buf << static_cast<char>('A' + tmp - 10);
    }
    bit = 1U;
    tmp = 0U;
  }
  return buf.str();
}

// @brief ハッシュ値を求める．
SizeType
BitVectorRep::hash() const
{
  auto nb = block_num(len());
  SizeType ans = 0;
  for ( auto i: Range_<SizeType>(0, nb) ) {
    ans ^= mPat[i];
  }
  return ans;
}

END_NAMESPACE_DRUID
