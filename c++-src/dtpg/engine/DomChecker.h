#ifndef DOMCHECKER_H
#define DOMCHECKER_H

/// @file DomChecker.h
/// @brief DomChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DomChecker DomChecker.h "DomChecker.h"
/// @brief ２つの FFR の故障の間の支配関係を調べるクラス
///
/// 処理はコンストラクタ中で行われる．
/// 外部インターフェイスは結果を取り出すためのもの
//////////////////////////////////////////////////////////////////////
class DomChecker
{
public:

  /// @brief コンストラクタ
  DomChecker(
    const TpgFFR& ffr1,              ///< [in] FFR1
    const TpgFFR& ffr2,              ///< [in] FFR2
    const TpgFaultList& fault_list1, ///< [in] ffr1 の故障のリスト
    const TpgFaultList& fault_list2, ///< [in] ffr2 の故障のリスト
    const ConfigParam& option = {}   ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DomChecker() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief fault_list1 の故障の情報を fault_info にコピーする．
  void
  copy(
    FaultInfo& fault_info ///< [in] 結果をセットするオブジェクト
  ) const;

  /// @brief fault_list1 の故障に対する支配故障を返す．
  ///
  /// 支配されていない場合は不正値を返す．
  TpgFault
  dominator1(
    SizeType pos ///< [in] fault_list1 中の位置番号
  ) const
  {
    if ( pos >= mDomList1.size() ) {
      throw std::out_of_range{"pos is out of range"};
    }
    return mDomList1[pos];
  }

  /// @brief fault_list2 の故障に対する支配故障を返す．
  ///
  /// 支配されていない場合は不正値を返す．
  TpgFault
  dominator2(
    SizeType pos ///< [in] fault_list2 中の位置番号
  ) const
  {
    if ( pos >= mDomList2.size() ) {
      throw std::out_of_range{"pos is out of range"};
    }
    return mDomList2[pos];
  }

  /// @brief 調べた故障ペアの数を返す．
  SizeType
  check_count() const
  {
    return mCheckCount;
  }

  /// @brief dominator1() 中の有効な故障の数を返す．
  SizeType
  dom1_count() const;

  /// @brief dominator2() 中の有効な故障の数を返す．
  SizeType
  dom2_count() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 調べた故障ペアの数
  SizeType mCheckCount;

  // ffr1 の故障リスト
  TpgFaultList mFaultList1;

  // ffr2 の故障リスト
  TpgFaultList mFaultList2;

  // mFaultList1 中の支配故障を持つ位置番号のリスト
  std::vector<SizeType> mPosList1;

  // mFaultList1 に対する支配故障のリスト
  std::vector<TpgFault> mDomList1;

  // mFaultList2 中の支配故障を持つ位置番号のリスト
  std::vector<SizeType> mPosList2;

  // mFaultList2 に対する支配故障のリスト
  std::vector<TpgFault> mDomList2;

};

END_NAMESPACE_DRUID

#endif // DOMCHECKER_H
