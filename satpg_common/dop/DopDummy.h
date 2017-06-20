﻿#ifndef DOPDUMMY_H
#define DOPDUMMY_H

/// @file DopDummy.h
/// @brief DopDummy のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DetectOp.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DopDummy DopDummy.h "DopDummy.h"
/// @brief DetectOp の基本的な動作を行なうクラス
//////////////////////////////////////////////////////////////////////
class DopDummy :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  DopDummy();

  /// @brief デストラクタ
  virtual
  ~DopDummy();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった時の処理
  /// @param[in] f 故障
  /// @param[in] assign_list 値の割当リスト
  virtual
  void
  operator()(const TpgFault* f,
	     const NodeValList& assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};

END_NAMESPACE_YM_SATPG

#endif // DOPDUMMY_H
