#ifndef AUXNODEINFO_H
#define AUXNODEINFO_H

/// @file AuxNodeInfo.h
/// @brief AuxNodeInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"
#include "TpgFaultBase.h"
#include "ym/Alloc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class AuxNodeInfo AuxNodeInfo.h "AuxNodeInfo.h"
/// @brief TpgNode の付加的な情報を保持するクラス
//////////////////////////////////////////////////////////////////////
class AuxNodeInfo
{
public:

  /// @brief コンストラクタ
  AuxNodeInfo();

  /// @brief デストラクタ
  ~AuxNodeInfo();


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前を返す．
  const char*
  name() const;

  /// @brief FFRの根の場合にFFRを返す．
  ///
  /// そうでなければ nullptr を返す．
  const TpgFFR*
  ffr() const;

  /// @brief MFFCの根の場合にMFFCを返す．
  ///
  /// そうでなければ nullptr を返す．
  const TpgMFFC*
  mffc() const;

  /// @brief このノードに含まれる代表故障の数を返す．
  int
  fault_num() const;

  /// @brief このノードに含まれる代表故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
  const TpgFault*
  fault(int pos) const;

  /// @brief このノードが持っている代表故障をリストに追加する．
  void
  add_to_fault_list(vector<const TpgFault*>& fault_list);

  /// @brief 出力の故障を返す．
  /// @param[in] val 故障値 ( 0 / 1 )
  TpgFaultBase*
  output_fault(int val) const;

  /// @brief 入力の故障を返す．
  /// @param[in] pos 入力の位置番号
  /// @param[in] val 故障値 ( 0 / 1 )
  TpgFaultBase*
  input_fault(int pos,
	      int val) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  /// @param[in] name 名前
  /// @param[in] ni ファンイン数
  /// @param[in] alloc メモリアロケータ
  void
  init(const string& name,
       int ni,
       Alloc& alloc);

  /// @brief FFR を設定する．
  /// @param[in] ffr このノードを根とするFFR
  void
  set_ffr(TpgFFR* ffr);

  /// @brief MFFC を設定する．
  /// @param[in] mffc このノードを根とするMFFC
  void
  set_mffc(const TpgMFFC* mffc);

  /// @brief 故障リストを設定する．
  void
  set_fault_list(int fault_num,
		 const TpgFault** fault_list);

  /// @brief 出力の故障を設定する．
  /// @param[in] val 故障値 ( 0 / 1 )
  /// @param[in] f 故障
  void
  set_output_fault(int val,
		   TpgFaultBase* f);

  /// @brief 入力の故障を設定する．
  /// @param[in] ipos 入力位置
  /// @param[in] val 故障値 ( 0 / 1 )
  /// @param[in] f 故障
  void
  set_input_fault(int ipos,
		  int val,
		  TpgFaultBase* f);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード名
  char* mName;

  /// @brief FFR
  const TpgFFR* mFfr;

  /// @brief MFFC
  const TpgMFFC* mMffc;

  /// @brief 代表故障数
  int mFaultNum;

  /// @brief 代表故障のリスト
  const TpgFault** mFaultList;

  /// @brief 出力の故障
  TpgFaultBase* mOutputFaults[2];

  /// @brief 入力数
  int mFaninNum;

  /// @brief 入力の故障の配列
  ///
  /// サイズは mFaninNum * 2
  TpgFaultBase** mInputFaults;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 名前を返す．
inline
const char*
AuxNodeInfo::name() const
{
  return mName;
}

// @brief FFRの根の場合にFFRを返す．
//
// そうでなければ nullptr を返す．
inline
const TpgFFR*
AuxNodeInfo::ffr() const
{
  return mFfr;
}

// @brief MFFCの根の場合にMFFCを返す．
//
// そうでなければ nullptr を返す．
inline
const TpgMFFC*
AuxNodeInfo::mffc() const
{
  return mMffc;
}

// @brief このノードに含まれる代表故障の数を返す．
inline
int
AuxNodeInfo::fault_num() const
{
  return mFaultNum;
}

// @brief このノードに含まれる代表故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
inline
const TpgFault*
AuxNodeInfo::fault(int pos) const
{
  ASSERT_COND( pos < fault_num() );

  return mFaultList[pos];
}

// @brief 出力の故障を返す．
// @param[in] val 故障値 ( 0 / 1 )
inline
TpgFaultBase*
AuxNodeInfo::output_fault(int val) const
{
  ASSERT_COND( val == 0 || val == 1 );

  return mOutputFaults[val];
}

// @brief 入力の故障を返す．
// @param[in] pos 入力の位置番号
// @param[in] val 故障値 ( 0 / 1 )
inline
TpgFaultBase*
AuxNodeInfo::input_fault(int pos,
			 int val) const
{
  ASSERT_COND( val == 0 || val == 1 );
  ASSERT_COND( pos >= 0 && pos < mFaninNum );

  return mInputFaults[(pos * 2) + val];
}

END_NAMESPACE_DRUID

#endif // AUXNODEINFO_H
