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
  TpgMFFC();

  /// @brief コピーコンストラクタは禁止
  TpgMFFC(const TpgMFFC& src) = delete;

  /// @brief コピー代入演算子も禁止
  TpgMFFC&
  operator=(const TpgMFFC& src) = delete;

  /// @brief ムーブコンストラクタは禁止
  TpgMFFC(TpgMFFC&& src) = delete;

  /// @brief ムーブ代入演算子も禁止
  TpgMFFC&
  operator=(TpgMFFC&& src) = delete;

  /// @brief デストラクタ
  ~TpgMFFC();


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
  Array<const TpgFFR*>
  ffr_list() const;

  /// @brief このMFFCに含まれる代表故障の数を返す．
  int
  fault_num() const;

  /// @brief このFFRに含まれる代表故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
  const TpgFault*
  fault(int pos) const;

  /// @brief このFFRに含まれる代表故障のリストを返す．
  Array<const TpgFault*>
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
      int ffr_num,
      const TpgFFR** ffr_list,
      int fault_num,
      const TpgFault** fault_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 根のノード
  const TpgNode* mRoot;

  // FFR数
  int mFfrNum;

  // FFRの配列
  const TpgFFR** mFfrList;

  // 故障数
  int mFaultNum;

  // 故障の配列
  const TpgFault** mFaultList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
TpgMFFC::TpgMFFC()
{
  mRoot = nullptr;
  mFfrNum = 0;
  mFfrList = nullptr;
  mFaultNum = 0;
  mFaultList = nullptr;
}

// @brief デストラクタ
inline
TpgMFFC::~TpgMFFC()
{
}

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
  return mFfrNum;
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
Array<const TpgFFR*>
TpgMFFC::ffr_list() const
{
  return Array<const TpgFFR*>(mFfrList, 0, ffr_num());
}

// @brief このFFRに含まれる代表故障の数を返す．
inline
int
TpgMFFC::fault_num() const
{
  return mFaultNum;
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
Array<const TpgFault*>
TpgMFFC::fault_list() const
{
  return Array<const TpgFault*>(mFaultList, 0, fault_num());
}

// @brief 内容を設定する．
// @param[in] root 根のノード
// @param[in] ffr_num このMFFCに含まれるFFR数
// @param[in] ffr_list このMFFCに含まれるFFRのリスト
// @param[in] fault_num このMFFCに含まれる故障数
// @param[in] fault_list このMFFCに含まれる故障のリスト
inline
void
TpgMFFC::set(const TpgNode* root,
	     int ffr_num,
	     const TpgFFR** ffr_list,
	     int fault_num,
	     const TpgFault** fault_list)
{
  mRoot = root;
  mFfrNum = ffr_num;
  mFfrList = ffr_list;
  mFaultNum = fault_num;
  mFaultList = fault_list;
}

END_NAMESPACE_DRUID

#endif // TPGMFFC_H
