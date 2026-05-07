#ifndef FSIMRESULTSREP_H
#define FSIMRESULTSREP_H

/// @file FsimResultsRep.h
/// @brief FsimResultsRep のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "fsim/FsimResults.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FsimResultsRep FsimResultsRep.h "FsimResultsRep.h"
/// @brief FsimResults のテストベクタ１つ分の実体
///
/// 意味的には
/// - 故障番号
/// - 出力の伝搬状態
/// を要素としたリストを表す．
//////////////////////////////////////////////////////////////////////
class FsimResultsRep
{
public:

  /// @brief 故障に対する出力ごとの検出結果
  struct Info {
    SizeType fault_id; ///< 故障番号
    DiffBits diffbits; ///< 出力ごとの故障伝搬状況
  };


public:

  /// @brief コンストラクタ
  FsimResultsRep() = default;

  /// @brief デストラクタ
  ~FsimResultsRep() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素を追加する．
  void
  add(
    SizeType fault_id,       ///< [in] 故障番号
    const DiffBits& diffbits ///< [in] 出力の故障伝搬状態
  )
  {
    mDetList.push_back({fault_id, diffbits});
  }

  /// @brief 検出された故障数を返す．
  SizeType
  det_num() const
  {
    return mDetList.size();
  }

  /// @brief 検出された故障を返す．
  SizeType
  fault_id(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < det_num() )
  ) const
  {
    if ( pos >= det_num() ) {
      throw std::out_of_range{"pos is out of range"};
    }
    return mDetList[pos].fault_id;
  }

  /// @brief 出力の故障伝搬状態を返す．
  DiffBits
  diffbits(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < det_num() )
  ) const
  {
    if ( pos >= det_num() ) {
      throw std::out_of_range{"pos is out of range"};
    }
    return mDetList[pos].diffbits;
  }

  /// @brief 故障番号の昇順にソートする．
  void
  sort()
  {
    std::sort(mDetList.begin(), mDetList.end(),
	      [](const Info& a, const Info& b) -> bool {
		return a.fault_id < b.fault_id;
	      });
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 検出結果のリスト
  std::vector<Info> mDetList;

};

END_NAMESPACE_DRUID

#endif // FSIMRESULTSREP_H
