#ifndef DOPTVLIST_H
#define DOPTVLIST_H

/// @file DopTvList.h
/// @brief DopTvList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
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
  DopTvList(
    vector<TestVector>& tvlist ///< [in] テストベクタのリスト
  );

  /// @brief デストラクタ
  ~DopTvList();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった時の処理
  void
  operator()(
    const TpgFault& f,   ///< [in] 故障
    const TestVector& tv ///< [in] テストベクタ
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // テストベクタのリスト
  vector<TestVector>& mTvList;

};

END_NAMESPACE_DRUID

#endif // DOPTVLIST_H
