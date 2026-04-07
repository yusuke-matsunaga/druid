#ifndef FAULTINFO_H
#define FAULTINFO_H

/// @file FaultInfo.h
/// @brief FaultInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/FaultStatus.h"
#include "types/AssignList.h"
#include "types/TestVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultInfo FaultInfo.h "FaultInfo.h"
/// @brief 故障の解析結果
///
/// - 故障は以下の情報を持つ．
///   * FaultStatus(Detected, Untestable, Undetected)
///   * 故障検出条件
///   * 故障検出の必須条件
///   * テストベクタ
///   * 他の故障に支配されているか否か
/// - ２つの故障の両立関係
//////////////////////////////////////////////////////////////////////
class FaultInfo
{
public:

  // 故障に関する情報を表す構造体
  struct Cell {
    FaultStatus fault_status{FaultStatus::Undetected};  ///< 故障の検出状況
    AssignList detect_cond;                             ///< 故障検出条件
    AssignList mandatory_cond;                          ///< 故障検出の必須条件
    TestVector testvector;                              ///< テストベクタ
    bool dominated{false};                              ///< 被支配フラグ
  };


public:

  /// @brief コンストラクタ
  FaultInfo(
    SizeType max_fault_id ///< [in] 故障番号の最大値
  );

  /// @brief デストラクタ
  ~FaultInfo() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を読み出す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の検出状況を調べる．
  FaultStatus
  fault_status(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief 検出可能で他の故障に支配されていない時 true を返す．
  bool
  is_rep(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief 支配されている故障の時 true を返す．
  bool
  is_dominated(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief 検出可能な場合の検出条件(拡張テストキューブ)を求める．
  ///
  /// fault_status(fault) == Detected の故障のみが対象となる．
  const AssignList&
  detect_cond(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief 検出可能な場合の必須条件(拡張テストキューブ)を求める．
  ///
  /// fault_status(fault) == Detected の故障のみが対象となる．
  const AssignList&
  mandatory_cond(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief 検出可能な場合のテストキューブを求める．
  ///
  /// fault_status(fault) == Detected の故障のみが対象となる．
  const TestVector&
  testvector(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief ２つの故障の両立関係を調べる．
  ///
  /// fault_status(fault) == Detected の故障のみが対象となる．
  bool
  check_compatible(
    const TpgFault& fault1, ///< [in] 対象の故障1
    const TpgFault& fault2  ///< [in] 対象の故障2
  ) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 検出情報をセットする．
  void
  set_detected(
    const TpgFault& fault,              ///< [in] 対象の故障
    const AssignList& detect_condition, ///< [in] 検出条件
    const TestVector& tv                ///< [in] テストベクタ
  );

  /// @brief 検出の必須条件をセットする．
  void
  set_mandatory_condition(
    const TpgFault& fault, ///< [in] 対象の故障
    const AssignList& cond ///< [in] 必須条件
  );

  /// @brief 検出不能の情報をセットする．
  void
  set_untestable(
    const TpgFault& fault ///< [in] 対象の故障
  );

  /// @brief 支配されている情報をセットする．
  void
  set_dominated(
    const TpgFault& fault ///< [in] 対象の故障
  );

  /// @brief 両立関係をセットする．
  void
  set_compatible(
    const TpgFault& fault1, ///< [in] 対象の故障1
    const TpgFault& fault2  ///< [in] 対象の故障2
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// 故障に対応する Cell を取り出す．
  const Cell&
  _cell(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// 故障に対応する Cell を取り出す．
  Cell&
  _cell(
    const TpgFault& fault ///< [in] 対象の故障
  );

  /// @brief ２つの故障の対からキーを作る．
  SizeType
  make_key(
    const TpgFault& fault1, ///< [in] 対象の故障1
    const TpgFault& fault2  ///< [in] 対象の故障2
  ) const;

  /// @brief 故障番号のチェックを行う．
  void
  _check_fid(
    SizeType fid ///< [in] 故障番号
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障番号をキーとして故障の情報を格納した配列
  std::vector<Cell> mCellArray;

  // 故障の両立関係を表す集合
  std::unordered_set<SizeType> mCompatibleSet;

};

END_NAMESPACE_DRUID

#endif // FAULTINFO_H
