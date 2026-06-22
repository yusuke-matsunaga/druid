#ifndef PATGEN_H
#define PATGEN_H

/// @file PatGen.h
/// @brief PatGen のヘッダファイル
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
/// @class PatGen PatGen.h "PatGen.h"
/// @brief 故障シミュレーション用のパタンを生成するクラス
//////////////////////////////////////////////////////////////////////
class PatGen
{
public:

  /// @brief コンストラクタ
  PatGen(
    const FaultInfo& fault_info ///< [in] 故障情報を持つオブジェクト
  ) : mFaultInfo{fault_info}
  {
  }

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<PatGen>
  new_obj(
    const FaultInfo& fault_info, ///< [in] 故障情報を持つオブジェクト
    const ConfigParam& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  virtual
  ~PatGen() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief パタンを作る．
  virtual
  void
  gen(
    SizeType size,                   ///< [in] パタンのサイズ
    std::vector<TestVector>& tv_buff ///< [in] 生成したパタンを格納するオブジェクト
  ) = 0;

  /// @brief 検出結果で更新する．
  virtual
  void
  update(
    const FsimResults& res ///< [in] 故障シミュレーションの結果
  ) = 0;


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

#endif // PATGEN_H
