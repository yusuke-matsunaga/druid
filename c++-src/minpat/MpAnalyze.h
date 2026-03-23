#ifndef MPANALYZE_H
#define MPANALYZE_H

/// @file MpAnalyze.h
/// @brief MpAnalyze のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpAnalyze MpAnalyze.h "MpAnalyze.h"
/// @brief テストパタンを評価するクラス
//////////////////////////////////////////////////////////////////////
class MpAnalyze
{
public:

  /// @brief コンストラクタ
  MpAnalyze(
    const std::vector<TestVector>& tv_list, ///< [in] テストベクタのリスト
    const TpgFaultList& fault_list          ///< [in] 故障のリスト
  );

  /// @brief デストラクタ
  ~MpAnalyze() = default;


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
    _check_tv_id(tv_id);
    return mDetListArray[tv_id];
  }

  /// @brief 各テストベクタが検出する故障数を返す．
  SizeType
  det_num(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    _check_tv_id(tv_id);
    return mDetListArray[tv_id].size();
  }

  /// @brief 各テストベクタが唯一検出する故障番号のリストを得る．
  const std::vector<SizeType>&
  exclusive_list(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    _check_tv_id(tv_id);
    return mExListArray[tv_id];
  }

  /// @brief 各テストベクタが唯一検出する故障数を返す．
  SizeType
  exclusive_num(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    _check_tv_id(tv_id);
    return mExListArray[tv_id].size();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で使用する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief tv_id をチェックする．
  void
  _check_tv_id(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    if ( tv_id >= tv_num() ) {
      throw std::out_of_range{"tv_id is out of range"};
    }
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

#endif // MPANALYZE_H
