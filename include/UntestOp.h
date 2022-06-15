#ifndef UNTESTOP_H
#define UNTESTOP_H

/// @file UntestOp.h
/// @brief UntestOp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class UntestOp UntestOp.h "UntestOp.h"
/// @brief テスト不能故障と判定された時の処理を行なうファンクタ
//////////////////////////////////////////////////////////////////////
class UntestOp
{
public:

  /// @brief デストラクタ
  virtual
  ~UntestOp() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト不能故障と判定された時の処理
  /// @param[in] f 故障
  virtual
  void
  operator()(const TpgFault* f) = 0;

};

/// @brief 'base' タイプを生成する．
UntestOp*
new_UopBase(
  FaultStatusMgr& fmgr ///< [in] 故障マネージャ
);

/// @brief 'skip' タイプを生成する．
UntestOp*
new_UopSkip(
  int threshold,   ///< [in] しきい値
  int max_fault_id ///< [in] 故障番号の最大値
);

/// @brief 'dummy' タイプを生成する．
UntestOp*
new_UopDummy();

END_NAMESPACE_DRUID

#endif // UNTESTOP_H
