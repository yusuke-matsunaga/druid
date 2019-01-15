#ifndef DETECTOP_H
#define DETECTOP_H

/// @file DetectOp.h
/// @brief DetectOp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"
#include "FaultType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DetectOp DetectOp.h "DetectOp.h"
/// @brief テストパタンが見つかったときの処理を行なうファンクタ
//////////////////////////////////////////////////////////////////////
class DetectOp
{
public:

  /// @brief デストラクタ
  virtual
  ~DetectOp() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった時の処理
  /// @param[in] f 故障
  /// @param[in] tv テストベクタ
  virtual
  void
  operator()(const TpgFault* f,
	     const TestVector& tv) = 0;

};

/// @brief 'base' タイプを生成する．
/// @param[in] fsmgr 故障マネージャ
DetectOp*
new_DopBase(FaultStatusMgr& fmgr);

/// @brief 'drop' タイプを生成する．
/// @param[in] fsmgr 故障マネージャ
/// @param[in] fsim 故障シミュレータ
DetectOp*
new_DopDrop(FaultStatusMgr& fmgr,
	    Fsim& fsim);

/// @brief 'tvlist' タイプを生成する．
/// @param[in] tvlist テストベクタのリスト
DetectOp*
new_DopTvList(int input_num,
	      int dff_num,
	      FaultType fault_type,
	      vector<TestVector>& tvlist);

/// @brief 'verify' タイプを生成する．
/// @param[in] fsim 故障シミュレータ
/// @param[in] result 結果を格納するオブジェクト
DetectOp*
new_DopVerify(Fsim& fsim,
	      DopVerifyResult& result);

/// @brief 'dummy' タイプを生成する．
DetectOp*
new_DopDummy();

END_NAMESPACE_DRUID

#endif // DETECTOP_H
