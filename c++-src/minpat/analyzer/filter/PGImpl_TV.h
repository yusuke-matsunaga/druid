#ifndef PGIMPL_TV_H
#define PGIMPL_TV_H

/// @file PGImpl_TV.h
/// @brief PGImpl_TV のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "PGImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PGImpl_TV PGImpl_TV.h "PGImpl_TV.h"
/// @brief 故障のテストパタンをつかう PGImpl の派生クラス
//////////////////////////////////////////////////////////////////////
class PGImpl_TV :
  public PGImpl
{
public:

  /// @brief コンストラクタ
  PGImpl_TV(
    const FaultInfo& fault_info ///< [in] 故障情報を持つオブジェクト
  );

  /// @brief デストラクタ
  ~PGImpl_TV() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief パタンを作る．
  /// @return パタンを生成した時 true を返す．
  bool
  get_pat(
    TestVector& tv ///< [in] 生成したパタンを格納するオブジェクト
  ) override;

  /// @brief 検出結果で更新する．
  void
  update(
    const FsimResults& res ///< [in] 故障シミュレーションの結果
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障のリスト
  TpgFaultList mFaultList;

  // 次の位置
  SizeType mNext;

};

END_NAMESPACE_DRUID

#endif // PGIMPL_TV_H
