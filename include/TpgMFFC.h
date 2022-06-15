#ifndef TPGMFFC_H
#define TPGMFFC_H

/// @file TpgMFFC.h
/// @brief TpgMFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
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
  root() const;

  /// @brief このMFFCに含まれるFFR数を返す．
  int
  ffr_num() const;

  /// @brief このMFFCに含まれるFFRを返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < ffr_num() )
  const TpgFFR*
  ffr(int pos) const;

  /// @brief このMFFCに含まれるFFRのリストを返す．
  const vector<const TpgFFR*>&
  ffr_list() const;

  /// @brief このMFFCに含まれる代表故障の数を返す．
  int
  fault_num() const;

  /// @brief このFFRに含まれる代表故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
  const TpgFault*
  fault(int pos) const;

  /// @brief このFFRに含まれる代表故障のリストを返す．
  const vector<const TpgFault*>&
  fault_list() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を設定する．
  /// @param[in] root 根のノード
  /// @param[in] ffr_num このMFFCに含まれるFFR数
  /// @param[in] ffr_list このMFFCに含まれるFFRのリスト
  /// @param[in] fault_num このMFFCに含まれる故障数
  /// @param[in] fault_list このMFFCに含まれる故障のリスト
  void
  set(const TpgNode* root,
      const vector<const TpgFFR*>& ffr_list,
      const vector<const TpgFault*>& fault_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


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


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 根のノードを返す．
inline
const TpgNode*
TpgMFFC::root() const
{
  return mRoot;
}

// @brief このMFFCに含まれるFFR数を返す．
inline
int
TpgMFFC::ffr_num() const
{
  return mFfrList.size();
}

// @brief このMFFCに含まれるFFRを返す．
// @param[in] pos 位置番号 ( 0 <= pos < ffr_num() )
inline
const TpgFFR*
TpgMFFC::ffr(int pos) const
{
  ASSERT_COND( pos < ffr_num() );

  return mFfrList[pos];
}

// @brief このMFFCに含まれるFFRのリストを返す．
inline
const vector<const TpgFFR*>&
TpgMFFC::ffr_list() const
{
  return mFfrList;
}

// @brief このFFRに含まれる代表故障の数を返す．
inline
int
TpgMFFC::fault_num() const
{
  return mFaultList.size();
}

// @brief このFFRに含まれる代表故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
inline
const TpgFault*
TpgMFFC::fault(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < fault_num() );

  return mFaultList[pos];
}

// @brief このFFRに含まれる代表故障のリストを返す．
inline
const vector<const TpgFault*>&
TpgMFFC::fault_list() const
{
  return mFaultList;
}

// @brief 内容を設定する．
// @param[in] root 根のノード
// @param[in] ffr_num このMFFCに含まれるFFR数
// @param[in] ffr_list このMFFCに含まれるFFRのリスト
// @param[in] fault_num このMFFCに含まれる故障数
// @param[in] fault_list このMFFCに含まれる故障のリスト
inline
void
TpgMFFC::set(
  const TpgNode* root,
  const vector<const TpgFFR*>& ffr_list,
  const vector<const TpgFault*>& fault_list
)
{
  mRoot = root;
  mFfrList = ffr_list;
  mFaultList = fault_list;
}

END_NAMESPACE_DRUID

#endif // TPGMFFC_H
