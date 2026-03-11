#ifndef PATANALYZER_H
#define PATANALYZER_H

/// @file PatAnalyzer.h
/// @brief PatAnalyzer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PatAnalyzer PatAnalyzer.h "PatAnalyzer.h"
/// @brief テストパタンを評価するクラス
//////////////////////////////////////////////////////////////////////
class PatAnalyzer
{
public:

  /// @brief コンストラクタ
  PatAnalyzer(
    const std::vector<TestVector>& tv_list, ///< [in] テストベクタのリスト
    const TpgFaultList& fault_list          ///< [in] 故障のリスト
  );

  /// @brief デストラクタ
  ~PatAnalyzer() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////


  /// @brief テストベクタ数を返す．
  SizeType
  tv_num() const
  {
    return mDetListArray.size();
  }

  /// @brief 各テストベクタが検出する故障番号のリストを得る．
  const std::vector<SizeType>&
  det_list(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    if ( tv_id >= tv_num() ) {
      throw std::out_of_range{"tv_id is out of range"};
    }
    return mDetListArray[tv_id];
  }

  /// @brief 各テストベクタが唯一検出する故障番号のリストを得る．
  const std::vector<SizeType>&
  exclusive_list(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    if ( tv_id >= tv_num() ) {
      throw std::out_of_range{"tv_id is out of range"};
    }
    return mExListArray[tv_id];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 各ベクタが検出する故障番号のリスト
  std::vector<std::vector<SizeType>> mDetListArray;

  // 各テストベクタが唯一検出する故障番号のリスト
  std::vector<std::vector<SizeType>> mExListArray;

};

END_NAMESPACE_DRUID

#endif // PATANALYZER_H
