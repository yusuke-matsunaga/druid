#ifndef AUXNODEINFO_H
#define AUXNODEINFO_H

/// @file AuxNodeInfo.h
/// @brief AuxNodeInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgFaultBase.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class AuxNodeInfo AuxNodeInfo.h "AuxNodeInfo.h"
/// @brief TpgNode の付加的な情報を保持するクラス
//////////////////////////////////////////////////////////////////////
class AuxNodeInfo
{
public:

  /// @brief コンストラクタ
  AuxNodeInfo() = default;

  /// @brief コンストラクタ
  AuxNodeInfo(
    const string& name,  ///< [in] 名前
    SizeType ni          ///< [in] ファンイン数
  )
  {
    init(name, ni);
  }

  /// @brief デストラクタ
  ~AuxNodeInfo() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前を返す．
  const string&
  name() const
  {
    return mName;
  }

  /// @brief FFRの根の場合にFFRを返す．
  ///
  /// そうでなければ nullptr を返す．
  const TpgFFR*
  ffr() const
  {
    return mFfr;
  }

  /// @brief MFFCの根の場合にMFFCを返す．
  ///
  /// そうでなければ nullptr を返す．
  const TpgMFFC*
  mffc() const
  {
    return mMffc;
  }

  /// @brief このノードに含まれる代表故障の数を返す．
  int
  fault_num() const
  {
    return mFaultList.size();
  }

  /// @brief このノードに含まれる代表故障を返す．
  const TpgFault*
  fault(
    SizeType pos  ///< [in] 位置番号 ( 0 <= pos < fault_num() )
  ) const
  {
    ASSERT_COND( pos < fault_num() );

    return mFaultList[pos];
  }

  /// @brief このノードが持っている代表故障をリストに追加する．
  void
  add_to_fault_list(
    vector<const TpgFault*>& fault_list ///< [out] 追加する故障リスト
  )
  {
    fault_list.insert(fault_list.end(), mFaultList.begin(), mFaultList.end());
  }

  /// @brief 出力の故障を返す．
  TpgFaultBase*
  output_fault(
    int val ///< [in] 故障値 ( 0 / 1 )
  ) const
  {
    ASSERT_COND( val == 0 || val == 1 );

    return mOutputFaults[val];
  }

  /// @brief 入力の故障を返す．
  TpgFaultBase*
  input_fault(
    SizeType pos, ///< [in] 入力の位置番号
    int val       ///< [in] 故障値 ( 0 / 1 )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < mFaninNum );
    ASSERT_COND( val == 0 || val == 1 );

    return mInputFaults[(pos * 2) + val];
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  void
  init(
    const string& name, ///< [in] 名前
    SizeType ni         ///< [in] ファンイン数
  )
  {
    mName = name;
    mFaninNum = ni;

    SizeType ni2 = ni * 2;
    mInputFaults.clear();
    mInputFaults.resize(ni2, nullptr);
  }

  /// @brief FFR を設定する．
  void
  set_ffr(
    TpgFFR* ffr ///< [in] このノードを根とするFFR
  )
  {
    mFfr = ffr;
  }

  /// @brief MFFC を設定する．
  void
  set_mffc(
    const TpgMFFC* mffc ///< [in] このノードを根とするMFFC
  )
  {
    mMffc = mffc;
  }

  /// @brief 故障リストを設定する．
  void
  set_fault_list(
    const vector<const TpgFault*>& fault_list ///< [in] 故障リスト
  )
  {
    mFaultList = fault_list;
  }

  /// @brief 出力の故障を設定する．
  void
  set_output_fault(
    int val,        ///< [in] 故障値 ( 0 / 1 )
    TpgFaultBase* f ///< [in] 故障
  )
  {
    ASSERT_COND( val == 0 || val == 1 );

    mOutputFaults[val] = f;
  }

  /// @brief 入力の故障を設定する．
  void
  set_input_fault(
    SizeType ipos,   ///< [in] 入力位置
    int val,         ///< [in] 故障値 ( 0 / 1 )
    TpgFaultBase* f  ///< [in] 故障
  )
  {
    ASSERT_COND( val == 0 || val == 1 );
    ASSERT_COND( ipos >= 0 && ipos < mFaninNum );

    mInputFaults[(ipos * 2) + val] = f;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード名
  string mName;

  /// @brief FFR
  const TpgFFR* mFfr{nullptr};

  /// @brief MFFC
  const TpgMFFC* mMffc{nullptr};

  /// @brief 代表故障のリスト
  vector<const TpgFault*> mFaultList;

  /// @brief 出力の故障
  TpgFaultBase* mOutputFaults[2]{nullptr, nullptr};

  /// @brief 入力数
  SizeType mFaninNum;

  /// @brief 入力の故障の配列
  ///
  /// サイズは mFaninNum * 2
  vector<TpgFaultBase*> mInputFaults;

};

END_NAMESPACE_DRUID

#endif // AUXNODEINFO_H
