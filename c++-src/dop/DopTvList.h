#ifndef DOPTVLIST_H
#define DOPTVLIST_H

/// @file DopTvList.h
/// @brief DopTvList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DetectOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DopTvList DopTvList.h "DopTvList.h"
/// @brief DetectOp の基本的な動作を行なうクラス
//////////////////////////////////////////////////////////////////////
class DopTvList :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] input_num 入力数
  /// @param[in] dff_numr DFF数
  /// @param[in] fault_type 故障の種類
  /// @param[in] tvlist テストベクタのリスト
  DopTvList(int input_num,
	    int dff_num,
	    FaultType fault_type,
	    vector<TestVector>& tvlist);

  /// @brief デストラクタ
  virtual
  ~DopTvList();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった時の処理
  /// @param[in] f 故障
  /// @param[in] tv テストベクタ
  void
  operator()(const TpgFault* f,
	     const TestVector& tv) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力数
  int mInputNum;

  // DFF数
  int mDffNum;

  // 故障の種類
  FaultType mFaultType;

  // テストベクタのリスト
  vector<TestVector>& mTvList;

};

END_NAMESPACE_DRUID

#endif // DOPTVLIST_H
