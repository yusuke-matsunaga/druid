#ifndef TPGMFFC_H
#define TPGMFFC_H

/// @file TpgMFFC.h
/// @brief TpgMFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/Array.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgMFFC TpgMFFC.h "TpgMFFC.h"
/// @brief TpgNetwork の MFFC(Maximal Fanout Free Cone) の情報を表すクラス
/// @sa TpgNetwork
/// @sa TpgNode
/// @sa TpgFault
/// @sa TpgFFR
///
/// 具体的には以下の情報を持つ．
/// - MFFC の根のノード
/// - MFFC に含まれる FFR のリスト
/// - MFFC に含まれる代表故障のリスト
/// 一度設定されたら不変のオブジェクトとなる．
//////////////////////////////////////////////////////////////////////
class TpgMFFC
{
public:

  /// @brief コンストラクタ
  TpgMFFC() = default;

  /// @brief デストラクタ
  ~TpgMFFC() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 根のノードを返す．
  const TpgNode*
  root() const
  {
    return mRoot;
  }

  /// @brief このMFFCに含まれるFFR数を返す．
  SizeType
  ffr_num() const
  {
    return mFfrList.size();
  }

  /// @brief このMFFCに含まれるFFRを返す．
  const TpgFFR*
  ffr(
    SizeType pos /// [in] 位置番号 ( 0 <= pos < ffr_num() )
  ) const
  {
    ASSERT_COND( pos < ffr_num() );

    return mFfrList[pos];
  }

  /// @brief このMFFCに含まれるFFRのリストを返す．
  const vector<const TpgFFR*>&
  ffr_list() const
  {
    return mFfrList;
  }

  /// @brief このMFFCに含まれる代表故障の数を返す．
  SizeType
  fault_num() const
  {
    return mFaultList.size();
  }

  /// @brief このFFRに含まれる代表故障を返す．
  const TpgFault*
  fault(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < fault_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < fault_num() );

    return mFaultList[pos];
  }

  /// @brief このFFRに含まれる代表故障のリストを返す．
  const vector<const TpgFault*>&
  fault_list() const
  {
    return mFaultList;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を設定する．
  void
  set(
    const TpgNode* root,                      ///< [in] 根のノード
    const vector<const TpgFFR*>& ffr_list,    ///< [in]	このMFFCに含まれるFFRのリスト
    const vector<const TpgFault*>& fault_list ///< [in] このMFFCに含まれる故障のリスト
  )
  {
    mRoot = root;
    mFfrList = ffr_list;
    mFaultList = fault_list;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 根のノード
  const TpgNode* mRoot{nullptr};

  // FFRの配列
  vector<const TpgFFR*> mFfrList;

  // 故障の配列
  vector<const TpgFault*> mFaultList;

};

END_NAMESPACE_DRUID

#endif // TPGMFFC_H
