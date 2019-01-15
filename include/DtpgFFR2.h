#ifndef DTPGFFR2_H
#define DTPGFFR2_H

/// @file DtpgFFR2.h
/// @brief DtpgFFR2 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgEngine.h"
#include "ym/HashMap.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgFFR2 DtpgFFR2.h "DtpgFFR2.h"
/// @brief FFR 単位で DTPG の基本的な処理を行うクラス
///
/// こちらは故障を検出しない条件を調べるために用いる．
//////////////////////////////////////////////////////////////////////
class DtpgFFR2 :
  public DtpgEngine
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] fault_type 故障の種類
  /// @param[in] just_type Justifier の種類を表す文字列
  /// @param[in] network 対象のネットワーク
  /// @param[in] ffr 故障伝搬の起点となる FFR
  DtpgFFR2(const string& sat_type,
	  const string& sat_option,
	  ostream* sat_outp,
	  FaultType fault_type,
	  const string& just_type,
	  const TpgNetwork& network,
	  const TpgFFR& ffr);

  /// @brief デストラクタ
  ~DtpgFFR2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief fault が検出不能か調べる．
  /// @param[in] fault 対象の故障
  /// @param[in] condition 制約条件
  /// @return 結果を返す．
  SatBool3
  check_untestable(const TpgFault* fault,
		   const NodeValList& condition);


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief FFR 内の故障差が伝搬しない条件を作る．
  void
  gen_ffr2_cnf();

  /// @brief node の plit を得る．
  SatLiteral
  get_plit(const TpgNode* node);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  HashMap<int, SatLiteral> mPvarMap;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////


END_NAMESPACE_DRUID

#endif // DTPGFFR2_H
