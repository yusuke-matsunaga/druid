#ifndef DTPGMGR_H
#define DTPGMGR_H

/// @file DtpgMgr.h
/// @brief DtpgMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "fsim/Fsim.h"
#include "types/TpgNetwork.h"
#include "types/TestVector.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "dtpg/DtpgStats.h"
#include "dtpg/DtpgResult.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgMgr DtpgMgr.h "DtpgMgr.h"
/// @brief テストパタン生成を行う本体
///
/// 基本的には与えられた全ての故障を検出するためのテストベクタの生成を行う
/// だけの関数．
/// ただし，ある故障に対するテストベクタが求まった時点でそのテストベクタで
/// 故障シミュレーションを行って検出できる故障を除外するなどの追加の処理
/// を行えるように故障検出時などに呼び出されるコールバック関数を引数にとる．
///
/// このクラスのインスタンスは以下の情報を持つ．
/// - 故障のリスト
/// - テスト生成の結果として生成されたテストベクタのリスト
/// - 各故障ごとのテスト生成の結果(DtpgResult)
///
/// 基本的に DtpgResult はコールバック関数でのみ設定されるものと仮定している．
//////////////////////////////////////////////////////////////////////
class DtpgMgr
{
public:

  /// @brief 検出時に呼ばれるコールバック関数の型
  using Callback_Det = std::function<void(DtpgMgr&, const TpgFault&, TestVector)>;

  /// @brief 検出不能時に呼ばれるコールバック関数の型
  using Callback_Undet = std::function<void(DtpgMgr&, const TpgFault&)>;

public:

  /// @brief コンストラクタ
  DtpgMgr(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const TpgFaultList& fault_list ///< [in] 対象の故障のリスト
  );

  /// @brief デストラクタ
  ~DtpgMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  ///
  /// 基本的にはコンストラクタに与えられた故障を対象とするが，
  /// コールバック関数中で故障シミュレーションを行う場合などに
  /// 本関数以外で検出された故障は除外する．
  DtpgStats
  run(
    Callback_Det det_func,      ///< [in] 検出時に呼ばれる関数
    const JsonValue& option     ///< [in] オプションを表す JSON オブジェクト
    = JsonValue{}
  )
  {
    return run(det_func,
	       [](DtpgMgr&, const TpgFault&){},
	       [](DtpgMgr&, const TpgFault&){},
	       option);
  }

  /// @brief テスト生成を行う．
  ///
  /// 基本的にはコンストラクタに与えられた故障を対象とするが，
  /// コールバック関数中で故障シミュレーションを行う場合などに
  /// 本関数以外で検出された故障は除外する．
  DtpgStats
  run(
    Callback_Det det_func,      ///< [in] 検出時に呼ばれる関数
    Callback_Undet untest_func, ///< [in] 検出不能の判定時に呼ばれる関数
    Callback_Undet abort_func,  ///< [in] アボート時に呼ばれる関数
    const JsonValue& option     ///< [in] オプションを表す JSON オブジェクト
    = JsonValue{}
  );

  /// @brief 対象のネットワークを得る．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 対象の故障のリストを得る．
  ///
  /// 常にコンストラクタに与えられたものと同じものとなっている．
  const TpgFaultList&
  fault_list() const
  {
    return mFaultList;
  }

  /// @brief 直前の run() で生成されたテストベクタのリスト
  const std::vector<TestVector>&
  testvector_list() const
  {
    return mTvList;
  }

  /// @brief 故障に対するテスト生成の結果を返す．
  DtpgResult
  dtpg_result(
    const TpgFault& fault ///< [in] 故障
  ) const;

  /// @brief 全故障数を返す．
  SizeType
  total_count() const;

  /// @brief 検出済み故障数を返す．
  SizeType
  detected_count() const;

  /// @brief 検出不能故障数を返す．
  SizeType
  untestable_count() const;

  /// @brief 未検出故障数を返す．
  SizeType
  undetected_count() const;


public:
  //////////////////////////////////////////////////////////////////////
  // コールバック関数から使用される関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンを追加する．
  void
  add_testvector(
    const TestVector& tv ///< [in] テストパタン
  );

  /// @brief 故障に対するテスト生成の結果を設定する．
  void
  set_dtpg_result(
    const TpgFault& fault, ///< [in] 故障
    DtpgResult result      ///< [in] 結果
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  TpgNetwork mNetwork;

  // 対象の故障のリスト
  TpgFaultList mFaultList;

  // テストベクタのリスト
  std::vector<TestVector> mTvList;

  // 故障番号をキーとして結果を格納する配列
  std::vector<DtpgResult> mDtpgResult;

};

END_NAMESPACE_DRUID

#endif // DTPGMGR_H
