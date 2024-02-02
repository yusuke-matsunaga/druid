
/// @file InputVals.cc
/// @brief InputVals の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
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
  return PV_ALL0;
#elif FSIM_VAL3
  // デフォルトで X にする．
  return PackedVal3(PV_ALL0, PV_ALL0);
#endif
}

// 0/1 を PackedVal/PackedVal3 に変換する．
inline
FSIM_VALTYPE
int_to_packedval(
  int val
)
{
#if FSIM_VAL2
  return val ? PV_ALL1 : PV_ALL0;
#elif FSIM_VAL3
  return val ? PackedVal3(PV_ALL1) : PackedVal3(PV_ALL0);
#endif
}

// Val3 を PackedVal/PackedVal3 に変換する．
inline
FSIM_VALTYPE
val3_to_packedval(
  Val3 val
)
{
#if FSIM_VAL2
  // Val3::_X は Val3::_0 とみなす．
  return (val == Val3::_1) ? PV_ALL1 : PV_ALL0;
#elif FSIM_VAL3
  switch ( val ) {
  case Val3::_X: return PackedVal3(PV_ALL0, PV_ALL0);
  case Val3::_0: return PackedVal3(PV_ALL1, PV_ALL0);
  case Val3::_1: return PackedVal3(PV_ALL0, PV_ALL1);
  }
#endif
}

// bit のビットに値を設定する．
inline
void
bit_set(
  FSIM_VALTYPE& val,
  Val3 ival,
  PackedVal bit
)
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
TvInputVals::TvInputVals(
  const TestVector& testvector
) : mTestVector{testvector}
{
}

// @brief デストラクタ
TvInputVals::~TvInputVals()
{
}

// @brief 値を設定する．(縮退故障用)
void
TvInputVals::set_val(
  FSIM_CLASSNAME& fsim,
  vector<FSIM_VALTYPE>& val_array
) const
{
  SizeType iid = 0;
  for ( auto simnode: fsim.ppi_list() ) {
    auto val3 = mTestVector.ppi_val(iid);
    auto val = val3_to_packedval(val3);
    val_array[simnode->id()] = val;
    ++ iid;
  }
}

// @brief 1時刻目の値を設定する．(遷移故障用)
void
TvInputVals::set_val1(
  FSIM_CLASSNAME& fsim,
  vector<FSIM_VALTYPE>& val_array
) const
{
  SizeType iid = 0;
  for ( auto simnode: fsim.ppi_list() ) {
    auto val3 = mTestVector.ppi_val(iid);
    auto val = val3_to_packedval(val3);
    val_array[simnode->id()] = val;
    ++ iid;
  }
}

// @brief 2時刻目の値を設定する．(縮退故障用)
void
TvInputVals::set_val2(
  FSIM_CLASSNAME& fsim,
  vector<FSIM_VALTYPE>& val_array
) const
{
  SizeType iid = 0;
  for ( auto simnode: fsim.input_list() ) {
    auto val3 = mTestVector.aux_input_val(iid);
    auto val = val3_to_packedval(val3);
    val_array[simnode->id()] = val;
    ++ iid;
  }
}


//////////////////////////////////////////////////////////////////////
// クラス Tv2InputVals
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Tv2InputVals::Tv2InputVals(
  PackedVal pat_map,
  TestVector pat_array[]
) : mPatMap{pat_map}
{
  // パタンのセットされている最初のビット位置を求めておく．
  mPatFirstBit = PV_BITLEN;
  for ( SizeType i = 0; i < PV_BITLEN; ++ i ) {
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
void
Tv2InputVals::set_val(
  FSIM_CLASSNAME& fsim,
  vector<FSIM_VALTYPE>& val_array
) const
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  SizeType iid = 0;
  for ( auto simnode: fsim.ppi_list() ) {
    auto val = init_val();
    PackedVal bit = 1ULL;
    for ( int i = 0; i < PV_BITLEN; ++ i, bit <<= 1 ) {
      SizeType pos = (mPatMap & bit) ? i : mPatFirstBit;
      auto ival = mPatArray[pos].ppi_val(iid);
      bit_set(val, ival, bit);
    }
    val_array[simnode->id()] = val;
    ++ iid;
  }
}

// @brief 1時刻目の値を設定する．(遷移故障用)
void
Tv2InputVals::set_val1(
  FSIM_CLASSNAME& fsim,
  vector<FSIM_VALTYPE>& val_array
) const
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  SizeType iid = 0;
  for ( auto simnode: fsim.ppi_list() ) {
    auto val = init_val();
    PackedVal bit = 1ULL;
    for ( int i = 0; i < PV_BITLEN; ++ i, bit <<= 1 ) {
      SizeType pos = (mPatMap & bit) ? i : mPatFirstBit;
      auto ival = mPatArray[pos].ppi_val(iid);
      bit_set(val, ival, bit);
    }
    val_array[simnode->id()] = val;
    ++ iid;
  }
}

// @brief 2時刻目の値を設定する．(遷移故障用)
void
Tv2InputVals::set_val2(
  FSIM_CLASSNAME& fsim,
  vector<FSIM_VALTYPE>& val_array
) const
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  SizeType iid = 0;
  for ( auto simnode: fsim.input_list() ) {
    auto val = init_val();
    PackedVal bit = 1ULL;
    for ( int i = 0; i < PV_BITLEN; ++ i, bit <<= 1 ) {
      SizeType pos = (mPatMap & bit) ? i : mPatFirstBit;
      auto ival = mPatArray[pos].aux_input_val(iid);
      bit_set(val, ival, bit);
    }
    val_array[simnode->id()] = val;
    ++ iid;
  }
}


//////////////////////////////////////////////////////////////////////
// クラス NvlInputVals
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NvlInputVals::NvlInputVals(
  const NodeValList& assign_list
) : mAssignList{assign_list}
{
}

// @brief デストラクタ
NvlInputVals::~NvlInputVals()
{
}

// @brief 値を設定する．(縮退故障用)
void
NvlInputVals::set_val(
  FSIM_CLASSNAME& fsim,
  vector<FSIM_VALTYPE>& val_array
) const
{
  auto val0 = init_val();
  for ( auto simnode: fsim.ppi_list() ) {
    val_array[simnode->id()] = val0;
  }

  for ( auto nv: mAssignList ) {
    ASSERT_COND( nv.time() == 1 );
    SizeType iid = nv.node()->input_id();
    auto simnode = fsim.ppi(iid);
    auto val = int_to_packedval(nv.val());
    val_array[simnode->id()] = val;
  }
}

// @brief 1時刻目の値を設定する．(遷移故障用)
void
NvlInputVals::set_val1(
  FSIM_CLASSNAME& fsim,
  vector<FSIM_VALTYPE>& val_array
) const
{
  auto val0 = init_val();
  for ( auto simnode: fsim.ppi_list() ) {
    val_array[simnode->id()] = val0;
  }

  for ( auto nv: mAssignList ) {
    if ( nv.time() == 0 ) {
      SizeType iid = nv.node()->input_id();
      auto simnode = fsim.ppi(iid);
      auto val = int_to_packedval(nv.val());
      val_array[simnode->id()] = val;
    }
  }
}

// @brief 2時刻目の値を設定する．(遷移故障用)
void
NvlInputVals::set_val2(
  FSIM_CLASSNAME& fsim,
  vector<FSIM_VALTYPE>& val_array
) const
{
  auto val0 = init_val();
  for ( auto simnode: fsim.input_list() ) {
    val_array[simnode->id()] = val0;
  }

  for ( auto nv: mAssignList ) {
    if ( nv.time() == 1 ) {
      SizeType iid = nv.node()->input_id();
      auto simnode = fsim.ppi(iid);
      auto val = int_to_packedval(nv.val());
      val_array[simnode->id()] = val;
    }
  }
}

END_NAMESPACE_DRUID_FSIM
