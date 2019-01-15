#ifndef TPGFFR_H
#define TPGFFR_H

/// @file TpgFFR.h
/// @brief TpgFFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/Array.h"


BEGIN_NAMESPACE_DRUID

class TpgFaultBase;

//////////////////////////////////////////////////////////////////////
/// @class TpgFFR TpgFFR.h "TpgFFR.h"
/// @brief TpgNetwork の FFR の情報を表すクラス
/// @sa TpgNetwork
/// @sa TpgNode
/// @sa TpgFault
/// @sa TpgMFFC
///
/// 具体的には以下の情報を持つ．
/// - FFR の根のノード
/// - FFR に含まれる代表故障のリスト
/// 一度設定された不変のオブジェクトとなる．
//////////////////////////////////////////////////////////////////////
class TpgFFR
{
public:

  /// @brief コンストラクタ
  TpgFFR();

  /// @brief コピーコンストラクタは禁止
  TpgFFR(const TpgFFR& src) = delete;

  /// @brief コピー代入演算子も禁止
  TpgFFR&
  operator=(const TpgFFR& src) = delete;

  /// @brief ムーブコンストラクタは禁止
  TpgFFR(TpgFFR&& src) = delete;

  /// @brief ムーブ代入演算子も禁止
  TpgFFR&
  operator=(TpgFFR&& src) = delete;

  /// @brief デストラクタ
  ~TpgFFR();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 根のノードを返す．
  const TpgNode*
  root() const;

  /// @brief このFFRに含まれる代表故障の数を返す．
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
  /// @param[in] fault_num  故障数
  /// @param[in] fault_list 故障のリスト(配列)
  void
  set(const TpgNode* root,
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
TpgFFR::TpgFFR()
{
  mRoot = nullptr;
  mFaultNum = 0;
  mFaultList = nullptr;
}

// @brief デストラクタ
inline
TpgFFR::~TpgFFR()
{
}

// @brief 根のノードを返す．
inline
const TpgNode*
TpgFFR::root() const
{
  return mRoot;
}

// @brief このFFRに含まれる代表故障の数を返す．
inline
int
TpgFFR::fault_num() const
{
  return mFaultNum;
}

// @brief このFFRに含まれる代表故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
inline
const TpgFault*
TpgFFR::fault(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < fault_num() );

  return mFaultList[pos];
}

// @brief このFFRに含まれる代表故障のリストを返す．
inline
Array<const TpgFault*>
TpgFFR::fault_list() const
{
  return Array<const TpgFault*>(mFaultList, 0, fault_num());
}

// @brief 内容を設定する．
// @param[in] root 根のノード
// @param[in] fault_num  故障数
// @param[in] fault_list 故障のリスト(配列)
inline
void
TpgFFR::set(const TpgNode* root,
	    int fault_num,
	    const TpgFault** fault_list)
{
  mRoot = root;
  mFaultNum = fault_num;
  mFaultList = fault_list;
}

END_NAMESPACE_DRUID

#endif // TPGFFR_H
