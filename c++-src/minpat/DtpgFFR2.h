#ifndef DTPGFFR2_H
#define DTPGFFR2_H

/// @file DtpgFFR2.h
/// @brief DtpgFFR2 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine2.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgFFR2 DtpgFFR2.h "DtpgFFR2.h"
/// @brief FFR 単位で DTPG の基本的な処理を行うクラス
///
/// こちらは故障を検出しない条件を調べるために用いる．
//////////////////////////////////////////////////////////////////////
class DtpgFFR2 :
  public DtpgEngine2
{
public:

  /// @brief コンストラクタ
  DtpgFFR2(
    const string& sat_type,    ///< [in] SATソルバの種類を表す文字列
    const string& sat_option,  ///< [in] SATソルバに渡すオプション文字列
    ostream* sat_outp,	       ///< [in] SATソルバ用の出力ストリーム
    FaultType fault_type,      ///< [in] 故障の種類
    const TpgNetwork& network, ///< [in] 象のネットワーク
    const TpgFFR& ffr	       ///< [in] 故障伝搬の起点となる FFR
  );

  /// @brief デストラクタ
  ~DtpgFFR2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief fault が検出不能か調べる．
  /// @return 結果を返す．
  SatBool3
  check_untestable(
    const TpgFault* fault,       ///< [in] 対象の故障
    const NodeValList& condition ///< [in] 制約条件
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief FFR 内の故障差が伝搬しない条件を作る．
  void
  gen_ffr2_cnf();

  /// @brief node の plit を得る．
  SatLiteral
  get_plit(
    const TpgNode* node
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  unordered_map<int, SatLiteral> mPvarMap;

};

END_NAMESPACE_DRUID

#endif // DTPGFFR2_H
