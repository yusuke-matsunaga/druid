#ifndef DOPBASE_H
#define DOPBASE_H

/// @file DopBase.h
/// @brief DopBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DetectOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DopBase DopBase.h "DopBase.h"
/// @brief DetectOp の基本的な動作を行なうクラス
//////////////////////////////////////////////////////////////////////
class DopBase :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] fsmgr 故障マネージャ
  DopBase(FaultStatusMgr& fsmgr);

  /// @brief デストラクタ
  virtual
  ~DopBase();


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

  // 故障マネージャ
  FaultStatusMgr& mFaultStatusMgr;

};

END_NAMESPACE_DRUID

#endif // DOPBASE_H
