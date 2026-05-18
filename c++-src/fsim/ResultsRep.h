#ifndef RESULTSREP_H
#define RESULTSREP_H

/// @file ResultsRep.h
/// @brief ResultsRep のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "fsim/DiffBits.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ResultsRep ResultsRep.h "ResultsRep.h"
/// @brief FsimResults の本体
///
/// 意味的には
/// - 故障番号
/// - 出力の伝搬状態
/// を要素としたリストの配列を表す．
///
/// ただし出力の伝搬状態を持たない場合もある．
//////////////////////////////////////////////////////////////////////
class ResultsRep
{
  friend class Fsim;

public:

  /// @brief コンストラクタ
  ResultsRep(
    SizeType ntv,     ///< [in] テストベクタ数
    bool has_diffbits ///< [in] DiffBits の情報を持つ時 true
  ) : mFidListArray(ntv),
      mDiffBitsDictArray(has_diffbits ? ntv : 0)
  {
  }

  /// @brief デストラクタ
  ~ResultsRep() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テストベクタ数を返す．
  SizeType
  tv_num() const
  {
    return mFidListArray.size();
  }

  /// @brief 検出された故障数を返す．
  SizeType
  fault_num(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    _check_tv_id(tv_id);
    return mFidListArray[tv_id].size();
  }

  /// @brief 検出された故障番号を返す．
  SizeType
  fid(
    SizeType tv_id, ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
    SizeType pos    ///< [in] 位置番号 ( 0 <= pos < det_num() )
  ) const
  {
    _check_tv_id(tv_id);
    _check_pos(tv_id, pos);
    return mFidListArray[tv_id][pos];
  }

  /// @brief 検出された故障番号のリストを返す．
  const std::vector<SizeType>&
  fid_list(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    _check_tv_id(tv_id);
    return mFidListArray[tv_id];
  }

  /// @brief 出力の故障伝搬状態を返す．
  DiffBits
  diffbits(
    SizeType tv_id, ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
    SizeType fid    ///< [in] 故障番号
  ) const
  {
    _check_tv_id(tv_id);
    _check_fid(tv_id, fid);
    return mDiffBitsDictArray[tv_id].at(fid);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で使用する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テストベクタ番号が適正かチェックする．
  void
  _check_tv_id(
    SizeType tv_id ///< [in] テストベクタ番号
  ) const
  {
    if ( tv_id >= tv_num() ) {
      throw std::out_of_range{"tv_id is out of range"};
    }
  }

  /// @brief 位置番号が適正かチェックする．
  void
  _check_pos(
    SizeType tv_id, ///< [in] テストベクタ番号
    SizeType pos    ///< [in] 一番号
  ) const
  {
    if ( pos >= mFidListArray[tv_id].size() ) {
      throw std::out_of_range{"pos is out of range"};
    }
  }

  /// @brief 故障番号が適正かチェックする．
  void
  _check_fid(
    SizeType tv_id,
    SizeType fid
  ) const
  {
    if ( mDiffBitsDictArray[tv_id].count(fid) == 0 ) {
      throw std::out_of_range{"fid is out of range"};
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障番号のリストの配列
  std::vector<std::vector<SizeType>> mFidListArray;

  // 故障番号をキーにして DiffBits を持つ辞書の配列
  std::vector<std::unordered_map<SizeType, DiffBits>> mDiffBitsDictArray;

};

END_NAMESPACE_DRUID

#endif // RESULTSREP_H
