#ifndef DTPGMFFC_H
#define DTPGMFFC_H

/// @file DtpgMFFC.h
/// @brief DtpgMFFC のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgMFFC DtpgMFFC.h "DtpgMFFC.h"
/// @brief MFFC 単位で DTPG の基本的な処理を行うクラス
//////////////////////////////////////////////////////////////////////
class DtpgMFFC :
  public DtpgEngine
{
public:

  /// @brief コンストラクタ
  DtpgMFFC(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,	     ///< [in] 故障の種類
    const TpgMFFC& mffc,	     ///< [in] Justifier の種類を表す文字列
    const string& just_type,	     ///< [in] 故障伝搬の起点となる MFFC
    const SatSolverType& solver_type ///< [in] SATソルバの実装タイプ
    = SatSolverType()
  );

  /// @brief デストラクタ
  ~DtpgMFFC();


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief make_cnf() の追加処理
  void
  make_cnf_sub() override;

  /// @brief gen_pattern() で用いる検出条件を作る．
  ///
  /// デフォルトでは空を返す．
  vector<SatLiteral>
  gen_assumptions(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

  /// @brief 故障挿入回路のCNFを作る．
  void
  inject_fault(
    SizeType ffr_id, ///< [in] FFR番号
    SatLiteral ovar  ///< [in] ゲートの出力の変数
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // MFFC の情報
  const TpgMFFC& mMFFC;

  // FFR の根のリスト
  // [0] は MFFC の根でもある．
  vector<const TpgNode*> mRootArray;

  // 各FFRの根に反転イベントを挿入するための変数
  // サイズは mRootArray.size()
  vector<SatLiteral> mEvarArray;

  // ノード番号をキーにしてFFR番号を入れる連想配列
  unordered_map<SizeType, SizeType> mFfrIdMap;

};

END_NAMESPACE_DRUID

#endif // DTPGMFFC_H
