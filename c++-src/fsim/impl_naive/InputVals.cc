
/// @file InputVals.cc
/// @brief InputVals の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "FsimX.h"
#include "InputVals.h"
#include "TestVector.h"
#include "NodeTimeValList.h"
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
  FSIM_CLASSNAME& fsim
) const
{
  SizeType iid = 0;
  for ( auto simnode: fsim.ppi_list() ) {
    auto val3 = mTestVector.ppi_val(iid);
    simnode->set_val(val3_to_packedval(val3));
    ++ iid;
  }
}

// @brief 1時刻目の値を設定する．(遷移故障用)
void
TvInputVals::set_val1(
  FSIM_CLASSNAME& fsim
) const
{
  SizeType iid = 0;
  for ( auto simnode: fsim.ppi_list() ) {
    auto val3 = mTestVector.ppi_val(iid);
    simnode->set_val(val3_to_packedval(val3));
    ++ iid;
  }
}

// @brief 2時刻目の値を設定する．(縮退故障用)
void
TvInputVals::set_val2(
  FSIM_CLASSNAME& fsim
) const
{
  SizeType iid = 0;
  for ( auto simnode: fsim.input_list() ) {
    auto val3 = mTestVector.aux_input_val(iid);
    simnode->set_val(val3_to_packedval(val3));
    ++ iid;
  }
}

// @brief 有効なビットを表すビットマスク
PackedVal
TvInputVals::bitmask() const
{
  return PV_ALL1;
}


//////////////////////////////////////////////////////////////////////
// クラス Tv2InputVals
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Tv2InputVals::Tv2InputVals(
  const vector<TestVector>& tv_list
) : mPatNum{tv_list.size()}
{
  for ( SizeType i = 0; i < mPatNum; ++ i ) {
    mPatArray[i] = tv_list[i];
  }
}

// @brief デストラクタ
Tv2InputVals::~Tv2InputVals()
{
}

// @brief 値を設定する．(縮退故障用)
void
Tv2InputVals::set_val(
  FSIM_CLASSNAME& fsim
) const
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  SizeType iid = 0;
  for ( auto simnode: fsim.ppi_list() ) {
    auto val = init_val();
    PackedVal bit = 1UL;
    for ( SizeType pos = 0; pos < PV_BITLEN; ++ pos, bit <<= 1 ) {
      SizeType epos = (pos < mPatNum) ? pos : 0;
      auto ival = mPatArray[epos].ppi_val(iid);
      bit_set(val, ival, bit);
    }
    simnode->set_val(val);
    ++ iid;
  }
}

// @brief 1時刻目の値を設定する．(遷移故障用)
void
Tv2InputVals::set_val1(
  FSIM_CLASSNAME& fsim
) const
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  SizeType iid = 0;
  for ( auto simnode: fsim.ppi_list() ) {
    auto val = init_val();
    PackedVal bit = 1UL;
    for ( SizeType pos = 0; pos < PV_BITLEN; ++ pos, bit <<= 1 ) {
      SizeType epos = (pos < mPatNum) ? pos : 0;
      auto ival = mPatArray[epos].ppi_val(iid);
      bit_set(val, ival, bit);
    }
    simnode->set_val(val);
    ++ iid;
  }
}

// @brief 2時刻目の値を設定する．(遷移故障用)
void
Tv2InputVals::set_val2(
  FSIM_CLASSNAME& fsim
) const
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  SizeType iid = 0;
  for ( auto simnode: fsim.input_list() ) {
    auto val = init_val();
    PackedVal bit = 1UL;
    for ( SizeType pos = 0; pos < PV_BITLEN; ++ pos, bit <<= 1 ) {
      SizeType epos = (pos < mPatNum) ? pos : 0;
      auto ival = mPatArray[epos].aux_input_val(iid);
      bit_set(val, ival, bit);
    }
    simnode->set_val(val);
    ++ iid;
  }
}

// @brief 有効なビットを表すビットマスク
PackedVal
Tv2InputVals::bitmask() const
{
  if ( mPatNum == PV_BITLEN ) {
    return PV_ALL1;
  }
  return (1UL << mPatNum) - 1;
}


//////////////////////////////////////////////////////////////////////
// クラス NvlInputVals
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NvlInputVals::NvlInputVals(
  const NodeTimeValList& assign_list
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
  FSIM_CLASSNAME& fsim
) const
{
  auto val0 = init_val();
  for ( auto simnode: fsim.ppi_list() ) {
    simnode->set_val(val0);
  }

  for ( auto nv: mAssignList ) {
    ASSERT_COND( nv.time() == 1 );
    SizeType iid = nv.node()->input_id();
    auto simnode = fsim.ppi(iid);
    simnode->set_val(int_to_packedval(nv.val()));
  }
}

// @brief 1時刻目の値を設定する．(遷移故障用)
void
NvlInputVals::set_val1(
  FSIM_CLASSNAME& fsim
) const
{
  auto val0 = init_val();
  for ( auto simnode: fsim.ppi_list() ) {
    simnode->set_val(val0);
  }

  for ( auto nv: mAssignList ) {
    if ( nv.time() == 0 ) {
      SizeType iid = nv.node()->input_id();
      auto simnode = fsim.ppi(iid);
      simnode->set_val(int_to_packedval(nv.val()));
    }
  }
}

// @brief 2時刻目の値を設定する．(遷移故障用)
void
NvlInputVals::set_val2(
  FSIM_CLASSNAME& fsim
) const
{
  auto val0 = init_val();
  for ( auto simnode: fsim.input_list() ) {
    simnode->set_val(val0);
  }

  for ( auto nv: mAssignList ) {
    if ( nv.time() == 1 ) {
      SizeType iid = nv.node()->input_id();
      auto simnode = fsim.ppi(iid);
      simnode->set_val(int_to_packedval(nv.val()));
    }
  }
}

// @brief 有効なビットを表すビットマスク
PackedVal
NvlInputVals::bitmask() const
{
  return PV_ALL1;
}

END_NAMESPACE_DRUID_FSIM
