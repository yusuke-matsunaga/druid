
/// @file InputVals.cc
/// @brief InputVals の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "FsimX.h"
#include "InputVals.h"
#include "TestVector.h"
#include "NodeValList.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID_FSIM

BEGIN_NONAMESPACE

// 初期値を求める．
inline
FSIM_VALTYPE
init_val()
{
#if FSIM_VAL2
  // デフォルトで 0 にする．
  return kPvAll0;
#elif FSIM_VAL3
  // デフォルトで X にする．
  return PackedVal3(kPvAll0, kPvAll0);
#endif
}

// 0/1 を PackedVal/PackedVal3 に変換する．
inline
FSIM_VALTYPE
int_to_packedval(int val)
{
#if FSIM_VAL2
  return val ? kPvAll1 : kPvAll0;
#elif FSIM_VAL3
  return val ? PackedVal3(kPvAll1) : PackedVal3(kPvAll0);
#endif
}

// Val3 を PackedVal/PackedVal3 に変換する．
inline
FSIM_VALTYPE
val3_to_packedval(Val3 val)
{
#if FSIM_VAL2
  // Val3::_X は Val3::_0 とみなす．
  return (val == Val3::_1) ? kPvAll1 : kPvAll0;
#elif FSIM_VAL3
  switch ( val ) {
  case Val3::_X: return PackedVal3(kPvAll0, kPvAll0);
  case Val3::_0: return PackedVal3(kPvAll1, kPvAll0);
  case Val3::_1: return PackedVal3(kPvAll0, kPvAll1);
  }
#endif
}

// bit のビットに値を設定する．
inline
void
bit_set(FSIM_VALTYPE& val,
	Val3 ival,
	PackedVal bit)
{
#if FSIM_VAL2
  if ( ival == Val3::_1 ) {
    val |= bit;
  }
#elif FSIM_VAL3
  FSIM_VALTYPE val1 = val3_to_packedval(ival);
  val.set_with_mask(val1, bit);
#endif
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス TvInputVals
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] testvector テストベクタ
TvInputVals::TvInputVals(const TestVector& testvector) :
  mTestVector(testvector)
{
}

// @brief デストラクタ
TvInputVals::~TvInputVals()
{
}

// @brief 値を設定する．(縮退故障用)
// @param[in] fsim 故障シミュレータ
void
TvInputVals::set_val(FSIM_CLASSNAME& fsim) const
{
  int iid = 0;
  for ( auto simnode: fsim.ppi_list() ) {
    Val3 val3 = mTestVector.ppi_val(iid);
    simnode->set_val(val3_to_packedval(val3));
    ++ iid;
  }
}

// @brief 1時刻目の値を設定する．(遷移故障用)
// @param[in] fsim 故障シミュレータ
void
TvInputVals::set_val1(FSIM_CLASSNAME& fsim) const
{
  int iid = 0;
  for ( auto simnode: fsim.ppi_list() ) {
    Val3 val3 = mTestVector.ppi_val(iid);
    simnode->set_val(val3_to_packedval(val3));
    ++ iid;
  }
}

// @brief 2時刻目の値を設定する．(縮退故障用)
// @param[in] fsim 故障シミュレータ
void
TvInputVals::set_val2(FSIM_CLASSNAME& fsim) const
{
  int iid = 0;
  for ( auto simnode: fsim.input_list() ) {
    Val3 val3 = mTestVector.aux_input_val(iid);
    simnode->set_val(val3_to_packedval(val3));
    ++ iid;
  }
}


//////////////////////////////////////////////////////////////////////
// クラス Tv2InputVals
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] pat_map パタンのセットされているビットに1を立てたビットマップ
// @param[in] pat_array パタンの配列(サイズは kPvBitLen の固定長)
Tv2InputVals::Tv2InputVals(PackedVal pat_map,
			   TestVector pat_array[]) :
  mPatMap(pat_map)
{
  // パタンのセットされている最初のビット位置を求めておく．
  mPatFirstBit = kPvBitLen;
  for ( int i = 0; i < kPvBitLen; ++ i ) {
    if ( mPatMap & (1ULL << i) ) {
      mPatArray[i] = pat_array[i];
      if ( mPatFirstBit > i ) {
	mPatFirstBit = i;
      }
    }
  }
}

// @brief デストラクタ
Tv2InputVals::~Tv2InputVals()
{
}

// @brief 値を設定する．(縮退故障用)
// @param[in] fsim 故障シミュレータ
void
Tv2InputVals::set_val(FSIM_CLASSNAME& fsim) const
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  int iid = 0;
  for ( auto simnode: fsim.ppi_list() ) {
    FSIM_VALTYPE val = init_val();
    PackedVal bit = 1ULL;
    for ( int i = 0; i < kPvBitLen; ++ i, bit <<= 1 ) {
      int pos = (mPatMap & bit) ? i : mPatFirstBit;
      Val3 ival = mPatArray[pos].ppi_val(iid);
      bit_set(val, ival, bit);
    }
    simnode->set_val(val);
    ++ iid;
  }
}

// @brief 1時刻目の値を設定する．(遷移故障用)
// @param[in] fsim 故障シミュレータ
void
Tv2InputVals::set_val1(FSIM_CLASSNAME& fsim) const
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  int iid = 0;
  for ( auto simnode: fsim.ppi_list() ) {
    FSIM_VALTYPE val = init_val();
    PackedVal bit = 1ULL;
    for ( int i = 0; i < kPvBitLen; ++ i, bit <<= 1 ) {
      int pos = (mPatMap & bit) ? i : mPatFirstBit;
      Val3 ival = mPatArray[pos].ppi_val(iid);
      bit_set(val, ival, bit);
    }
    simnode->set_val(val);
    ++ iid;
  }
}

// @brief 2時刻目の値を設定する．(遷移故障用)
// @param[in] fsim 故障シミュレータ
void
Tv2InputVals::set_val2(FSIM_CLASSNAME& fsim) const
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  int iid = 0;
  for ( auto simnode: fsim.input_list() ) {
    FSIM_VALTYPE val = init_val();
    PackedVal bit = 1ULL;
    for ( int i = 0; i < kPvBitLen; ++ i, bit <<= 1 ) {
      int pos = (mPatMap & bit) ? i : mPatFirstBit;
      Val3 ival = mPatArray[pos].aux_input_val(iid);
      bit_set(val, ival, bit);
    }
    simnode->set_val(val);
    ++ iid;
  }
}


//////////////////////////////////////////////////////////////////////
// クラス NvlInputVals
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] assign_list 値の割り当てリスト
NvlInputVals::NvlInputVals(const NodeValList& assign_list) :
  mAssignList(assign_list)
{
}

// @brief デストラクタ
NvlInputVals::~NvlInputVals()
{
}

// @brief 値を設定する．(縮退故障用)
// @param[in] fsim 故障シミュレータ
void
NvlInputVals::set_val(FSIM_CLASSNAME& fsim) const
{
  FSIM_VALTYPE val0 = init_val();
  for ( auto simnode: fsim.ppi_list() ) {
    simnode->set_val(val0);
  }

  for ( auto nv: mAssignList ) {
    ASSERT_COND( nv.time() == 1 );
    int iid = nv.node()->input_id();
    SimNode* simnode = fsim.ppi(iid);
    simnode->set_val(int_to_packedval(nv.val()));
  }
}

// @brief 1時刻目の値を設定する．(遷移故障用)
// @param[in] fsim 故障シミュレータ
void
NvlInputVals::set_val1(FSIM_CLASSNAME& fsim) const
{
  FSIM_VALTYPE val0 = init_val();
  for ( auto simnode: fsim.ppi_list() ) {
    simnode->set_val(val0);
  }

  for ( auto nv: mAssignList ) {
    if ( nv.time() == 0 ) {
      int iid = nv.node()->input_id();
      SimNode* simnode = fsim.ppi(iid);
      simnode->set_val(int_to_packedval(nv.val()));
    }
  }
}

// @brief 2時刻目の値を設定する．(遷移故障用)
// @param[in] fsim 故障シミュレータ
void
NvlInputVals::set_val2(FSIM_CLASSNAME& fsim) const
{
  FSIM_VALTYPE val0 = init_val();
  for ( auto simnode: fsim.input_list() ) {
    simnode->set_val(val0);
  }

  for ( auto nv: mAssignList ) {
    if ( nv.time() == 1 ) {
      int iid = nv.node()->input_id();
      SimNode* simnode = fsim.ppi(iid);
      simnode->set_val(int_to_packedval(nv.val()));
    }
  }
}

END_NAMESPACE_DRUID_FSIM
