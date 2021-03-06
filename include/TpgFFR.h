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

  /// @brief 葉(FFRの入力)の数を返す．
  int
  input_num() const;

  /// @brief 葉(FFRの入力)を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
  const TpgNode*
  input(int pos) const;

  /// @brief 葉(FFRの入力)のリストを返す．
  Array<const TpgNode*>
  input_list() const;

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
  /// @param[in] input_num 入力数
  /// @param[in] input_list 入力のノードのリスト(配列)
  /// @param[in] fault_num  故障数
  /// @param[in] fault_list 故障のリスト(配列)
  ///
  /// input_list, fault_list の所有権は移譲しない．
  /// 生成/解放の責任は親の TpgNetwork にある．
  void
  set(const TpgNode* root,
      int input_num,
      const TpgNode** input_list,
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

  // 葉の数
  int mInputNum;

  // 葉のノードの配列
  const TpgNode** mInputList;

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
  mInputNum = 0;
  mInputList = nullptr;
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

// @brief 葉(FFRの入力)の数を返す．
inline
int
TpgFFR::input_num() const
{
  return mInputNum;
}

// @brief 葉(FFRの入力)を返す．
// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
inline
const TpgNode*
TpgFFR::input(int pos) const
{
  ASSERT_COND( 0 <= pos && pos < input_num() );

  return mInputList[pos];
}

// @brief 葉(FFRの入力)のリストを返す．
inline
Array<const TpgNode*>
TpgFFR::input_list() const
{
  return Array<const TpgNode*>(mInputList, 0, input_num());
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
// @param[in] input_num 入力数
// @param[in] input_list 入力のノードのリスト(配列)
// @param[in] fault_num  故障数
// @param[in] fault_list 故障のリスト(配列)
inline
void
TpgFFR::set(const TpgNode* root,
	    int input_num,
	    const TpgNode** input_list,
	    int fault_num,
	    const TpgFault** fault_list)
{
  mRoot = root;
  mInputNum = input_num;
  mInputList = input_list;
  mFaultNum = fault_num;
  mFaultList = fault_list;
}

END_NAMESPACE_DRUID

#endif // TPGFFR_H
