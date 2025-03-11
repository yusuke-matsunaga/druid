#ifndef DTPGDRIVER_ENC_H
#define DTPGDRIVER_ENC_H

/// @file DtpgDriver_Enc.h
/// @brief DtpgDriver_Enc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriverImpl.h"
#include "BdEngine.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver_Enc DtpgDriver_Enc.h "DtpgDriver_Enc.h"
/// @brief BoolDiffEnc を用いた DtpgDriverImpl の基底クラス
//////////////////////////////////////////////////////////////////////
class DtpgDriver_Enc:
  public DtpgDriverImpl
{
public:

  /// @brief コンストラクタ
  DtpgDriver_Enc(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgNode* node,       ///< [in] 故障伝搬の起点となるノード
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  virtual
  ~DtpgDriver_Enc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を検出する条件を求める．
  ///
  /// - fault はコンストラクタで指定した FFR 内の故障でなければならない．
  SatBool3
  solve(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

  /// @brief テストパタン生成を行う．
  TestVector
  gen_pattern(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

  /// @brief CNF の生成時間を返す．
  double
  cnf_time() const override;

  /// @brief SATの統計情報を返す．
  SatStats
  sat_stats() const override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SubEnc を登録する．
  void
  add_subenc(
    std::unique_ptr<SubEnc> enc
  )
  {
    mEngine.add_subenc(std::move(enc));
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の伝搬条件を得る．
  virtual
  AssignList
  fault_prop_condition(
    const TpgFault* fault ///< [in] 対象の故障
  ) = 0;

  /// @brief 追加の条件を加える．
  virtual
  void
  add_extra_assumptions(
    const TpgFault* fault,          ///< [in] 対象の故障
    vector<SatLiteral>& assumptions ///< [inout] 追加する対象のリスト
  );

  /// @brief 追加の割り当てを加える．
  virtual
  void
  add_extra_assignments(
    const TpgFault* fault,  ///< [in] 対象の故障
    AssignList& assign_list ///< [inout] 追加する対象のリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // エンジン本体
  BdEngine mEngine;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVEE_ENC_H
