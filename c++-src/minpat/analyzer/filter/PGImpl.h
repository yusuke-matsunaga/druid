#ifndef PGIMPL_H
#define PGIMPL_H

/// @file PGImpl.h
/// @brief PGImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultInfo.h"
#include "types/TestVector.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PGImpl PGImpl.h "PGImpl.h"
/// @brief PatGen の実装クラス
//////////////////////////////////////////////////////////////////////
class PGImpl
{
public:

  /// @brief コンストラクタ
  PGImpl(
    const FaultInfo& fault_info, ///< [in] 故障情報を持つオブジェクト
    const ConfigParam& option    ///< [in] オプション
  ) : mFaultInfo{fault_info}
  {
  }

  /// @brief デストラクタ
  virtual
  ~PGImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief パタンを作る．
  /// @return パタンを生成した時 true を返す．
  virtual
  bool
  get_pat(
    TestVector& tv ///< [in] 生成したパタンを格納するオブジェクト
  ) = 0;

  /// @brief 検出結果で更新する．
  virtual
  void
  update(
    const FsimResults& res ///< [in] 故障シミュレーションの結果
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の情報を得る．
  const FaultInfo&
  fault_info() const
  {
    return mFaultInfo;
  }

  /// @brief 対象の故障リストを得る．
  TpgFaultList
  fault_list() const
  {
    return mFaultInfo.rep_fault_list();
  }

  /// @brief 対象のネットワークを得る．
  TpgNetwork
  network() const
  {
    return mFaultInfo.network();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障の情報
  const FaultInfo& mFaultInfo;

};

END_NAMESPACE_DRUID

#endif // PGIMPL_H
