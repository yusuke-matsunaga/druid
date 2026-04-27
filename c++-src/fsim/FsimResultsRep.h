#ifndef FSIMRESULTSREP_H
#define FSIMRESULTSREP_H

/// @file FsimResultsRep.h
/// @brief FsimResultsRep のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FsimResultsRep FsimResultsRep.h "FsimResultsRep.h"
/// @brief FsimResults の実体
///
/// 意味的には
/// - テストベクタ番号
/// - 故障番号
/// - 出力の伝搬状態
/// を要素としたリストだが，
/// テストベクタ番号ごとに操作する時に効率がよくなるように実装する．
//////////////////////////////////////////////////////////////////////
class FsimResultsRep
{
public:

  /// @brief コンストラクタ
  explicit
  FsimResultsRep(
    SizeType fault_size, ///< [in] 故障番号のサイズ
    SizeType tv_num = 1  ///< [in] 確保するベクタ数
  ) : mFaultSize{fault_size},
      mTvNum{tv_num},
      mArray(mTvNum * mFaultSize)
  {
  }

  /// @brief デストラクタ
  ~FsimResultsRep() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素を追加する．
  void
  add(
    SizeType tv_id,          ///< [in] テストベクタ番号
    SizeType fault_id,       ///< [in] 故障番号
    const DiffBits& diffbits ///< [in] 出力の故障伝搬状態
  )
  {
    _check_tv_id(tv_id);
    _check_fault_id(fault_id);
    auto index = _index(tv_id, fault_id);
    mArray[index] = diffbits;
  }

  /// @brief テストベクタ数を返す．
  SizeType
  tv_num() const
  {
    return mTvNum;
  }

  /// @brief 故障番号のサイズを返す．
  SizeType
  fault_size() const
  {
    return mFaultSize;
  }

  /// @brief 指定されたテストベクタ番号で検出された故障番号のリストを返す．
  std::vector<SizeType>
  fault_list(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    _check_tv_id(tv_id);
    std::vector<SizeType> ans_list;
    SizeType base = tv_id * mFaultSize;
    for ( SizeType id = 0; id < mFaultSize; ++ id ) {
      auto& dbits = mArray[base + id];
      if ( dbits.elem_num() > 0 ) {
	ans_list.push_back(id);
      }
    }
    return ans_list;
  }

  /// @brief 出力の故障伝搬状態を返す．
  DiffBits
  diffbits(
    SizeType tv_id,   ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
    SizeType fault_id ///< [in] 故障番号
  ) const
  {
    _check_tv_id(tv_id);
    _check_fault_id(fault_id);
    auto index = _index(tv_id, fault_id);
    return mArray[index];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief tv_id の範囲チェックを行う．
  void
  _check_tv_id(
    SizeType tv_id    ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    if ( tv_id >= tv_num() ) {
      throw std::out_of_range{"tv_id is out of range"};
    }
  }

  /// @brief fault_id の範囲チェックを行う．
  void
  _check_fault_id(
    SizeType fault_id ///< [in] 故障番号 ( 0 <= fault_id < fault_num() )
  ) const
  {
    if ( fault_id >= fault_size() ) {
      throw std::out_of_range{"fault_id is out of range"};
    }
  }

  /// @brief インデックスを返す．
  SizeType
  _index(
    SizeType tv_id,
    SizeType fault_id
  ) const
  {
    return tv_id * mFaultSize + fault_id;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // テストベクタ数
  SizeType mTvNum;

  // 故障番号のサイズ
  SizeType mFaultSize;

  // 本体の配列
  std::vector<DiffBits> mArray;

};

END_NAMESPACE_DRUID

#endif // FSIMRESULTSREP_H
