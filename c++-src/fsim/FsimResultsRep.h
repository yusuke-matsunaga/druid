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
/// @brief FsimResults のテストベクタ１つ分の実体
///
/// 意味的には
/// - 故障番号
/// - 出力の伝搬状態
/// を要素としたリストだが，スレッド実行の効率を考えて故障番号をキーにして
/// 出力の伝搬状態を格納する配列となっている．
//////////////////////////////////////////////////////////////////////
class FsimResultsRep
{
public:

  /// @brief コンストラクタ
  explicit
  FsimResultsRep(
    SizeType fault_size ///< [in] 故障番号のサイズ
  ) : mFaultSize{fault_size},
      mArray{new DiffBits*[mFaultSize]}
  {
    for ( SizeType i = 0; i < mFaultSize; ++ i ) {
      mArray[i] = nullptr;
    }
  }

  /// @brief デストラクタ
  ~FsimResultsRep()
  {
    for ( SizeType i = 0; i < mFaultSize; ++ i ) {
      delete mArray[i];
    }
    delete [] mArray;
  }


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
    _check_fault_id(fault_id);
    if ( mArray[fault_id] != nullptr ) {
      throw std::logic_error{"something wrong"};
    }
    mArray[fault_id] = new DiffBits(diffbits);
  }

  /// @brief 故障番号のサイズを返す．
  SizeType
  fault_size() const
  {
    return mFaultSize;
  }

  /// @brief 検出された故障番号のリストを返す．
  std::vector<SizeType>
  fault_list() const
  {
    std::vector<SizeType> ans_list;
    for ( SizeType id = 0; id < fault_size(); ++ id ) {
      auto dbits = mArray[id];
      if ( dbits != nullptr ) {
	ans_list.push_back(id);
      }
    }
    return ans_list;
  }

  /// @brief 検出されているか調べる．
  bool
  detected(
    SizeType fault_id ///< [in] 故障番号
  ) const
  {
    _check_fault_id(fault_id);
    return mArray[fault_id] != nullptr;
  }

  /// @brief 出力の故障伝搬状態を返す．
  const DiffBits&
  diffbits(
    SizeType fault_id ///< [in] 故障番号
  ) const
  {
    _check_fault_id(fault_id);
    return *mArray[fault_id];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障番号のサイズ
  SizeType mFaultSize{0};

  // 本体の配列
  DiffBits** mArray{nullptr};

};

END_NAMESPACE_DRUID

#endif // FSIMRESULTSREP_H
