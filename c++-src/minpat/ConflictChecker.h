#ifndef COMFLICTCHECKER_H
#define COMFLICTCHECKER_H

/// @file ComflictChecker.h
/// @brief ComflictChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class FaultInfoMgr;

//////////////////////////////////////////////////////////////////////
/// @class ConflictChecker ConflictChecker.h "ConflictChecker.h"
/// @brief 衝突している故障ペアを求めるためのクラス
//////////////////////////////////////////////////////////////////////
class ConflictChecker
{
public:

  /// @brief コンストラクタ
  ConflictChecker(
    const FaultInfoMgr& mgr ///< [in] 故障情報を保持するオブジェクト
  );

  /// @brief デストラクタ
  ~ConflictChecker() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 衝突している故障を求める．
  void
  run(
    const JsonValue& option
    = JsonValue{}
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークを返す．
  const TpgNetwork&
  network() const;

  /// @brief シミュレーションを行い両立故障を求める．
  void
  check_compatible();

  /// @brief 割り当てが衝突しているペアを求める．
  void
  check_trivial(
    bool localimp
  );

  /// @brief FFR 内の衝突チェックを行う．
  void
  check_ffr();

  /// @brief mandatory condition を使ったチェックを行う．
  void
  check_mandatory_condition();

  /// @brief 最終チェックを行う．
  void
  check_final();

  /// @brief キーを作る．
  SizeType
  gen_key(
    const TpgFault* fault1,
    const TpgFault* fault2
  ) const
  {
    SizeType id1 = fault1->id();
    SizeType id2 = fault2->id();
    if ( id1 > id2 ) {
      std::swap(id1, id2);
    }
    return id1 * network().max_fault_id() + id2;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障情報を保持するオブジェクト
  const FaultInfoMgr& mMgr;

  // 対象の故障リスト
  vector<const TpgFault*> mFaultList;

  // 故障数
  SizeType mFaultNum;

  // 衝突している故障ペアを表すハッシュ表
  std::unordered_set<SizeType> mConflictPair;

  // 両立している故障ペアを表すハッシュ表
  std::unordered_set<SizeType> mCompatPair;

};

END_NAMESPACE_DRUID

#endif // COMFLICTCHECKER_H
