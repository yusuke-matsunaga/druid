#ifndef DOPDUMMY_H
#define DOPDUMMY_H

/// @file DopDummy.h
/// @brief DopDummy のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DetectOp.h"


BEGIN_NAMESPACE_DRUID

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
  /// @param[in] tv テストベクタ
  void
  operator()(const TpgFault* f,
	     const TestVector& tv) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};

END_NAMESPACE_DRUID

#endif // DOPDUMMY_H
