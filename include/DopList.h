#ifndef DOPLIST_H
#define DOPLIST_H

/// @file DopList.h
/// @brief DopList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DetectOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DopList DopList.h "sa/DopList.h"
/// @brief DetectOp のリストを保持するクラス
///
/// 自身も DetectOp の継承クラスである．
//////////////////////////////////////////////////////////////////////
class DopList :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  DopList();

  /// @brief デストラクタ
  ///
  /// ここに登録された DetectOp も破棄される．
  ~DopList();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dop を追加する．
  void
  add(
    DetectOp* dop
  );

  /// @brief テストパタンが見つかった時の処理
  void
  operator()(
    const TpgFault* f,   ///< [in] 故障
    const TestVector& tv ///< [in] テストベクタ
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // dop のリスト
  vector<DetectOp*> mDopList;

};

END_NAMESPACE_DRUID

#endif // DOPLIST_H
