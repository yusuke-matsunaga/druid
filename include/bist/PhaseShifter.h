#ifndef PHASESHIFTER_H
#define PHASESHIFTER_H

/// @file PhaseShifter.h
/// @brief PhaseShifter のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BitVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PhaseShifter PhaseShifter.h "PhaseShifter.h"
/// @brief LFSR のビットベクタを PPI に変換する回路を表すクラス
//////////////////////////////////////////////////////////////////////
class PhaseShifter
{
public:

  /// @brief コンストラクタ
  PhaseShifter(
    SizeType lfsr_bitlen,                        ///< [in] LFSR のビット長
    const vector<vector<SizeType>>& input_config ///< [in] 各PPIのタップ位置の配列
  ) : mLFSRBitlen{lfsr_bitlen},
      mInputConfigArray{input_config}
  {
  }

  /// @brief デストラクタ
  ~PhaseShifter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief LFSR のビット列を変換する
  BitVector
  convert(
    const BitVector& bits ///< [in] 元のビットベクタ
  ) const
  {
    SizeType n = mInputConfigArray.size();
    BitVector bv(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      Val3 v3 = Val3::_0;
      for ( auto p: mInputConfigArray[i] ) {
	v3 = v3 ^ bits.val(p);
      }
      bv.set_val(i, v3);
    }
    return bv;
  }

  /// @brief PPI 数を得る．
  SizeType
  input_num() const
  {
    return mInputConfigArray.size();
  }

  /// @brief PPI のタップ位置の配列を得る．
  const vector<SizeType>&
  input_config(
    SizeType ppi_pos ///< [in] PPIの位置番号 ( 0 <= ppi_pos < input_num() )
  ) const
  {
    return mInputConfigArray[ppi_pos];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // LFSRのビット長
  SizeType mLFSRBitlen;

  // 各PPI用のタップ配列
  vector<vector<SizeType>> mInputConfigArray;

};

END_NAMESPACE_DRUID

#endif // PHASESHIFTER_H
