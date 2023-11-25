#ifndef TD_DTPG_H
#define TD_DTPG_H

/// @file td/Dtpg.h
/// @brief Dtpg のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/td_nsdef.h"

#include "DtpgStats.h"
#include "FaultStatus.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_DRUID_TD

class DtpgImpl;

//////////////////////////////////////////////////////////////////////
/// @class Dtpg Dtpg.h "sa/Dtpg.h"
/// @brief DTPG の基本エンジン
//////////////////////////////////////////////////////////////////////
class Dtpg
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] bt バックトレーサー
  Dtpg(const string& sat_type,
       const string& sat_option,
       ostream* sat_outp,
       BackTracer& bt);

  /// @brief デストラクタ
  ~Dtpg();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 回路の構造を表すCNF式を作る(FFRモード)．
  /// @param[in] network 対象のネットワーク
  /// @param[in] ffr 故障伝搬の起点となる FFR
  /// @param[out] stats DTPGの統計情報
  void
  gen_ffr_cnf(const TpgNetwork& network,
	      const TpgFFR* ffr,
	      DtpgStats& stats);

  /// @brief 回路の構造を表すCNF式を作る(MfFCモード)．
  /// @param[in] network 対象のネットワーク
  /// @param[in] mffc 故障伝搬の起点となる MFFC
  /// @param[out] stats DTPGの統計情報
  ///
  /// この MFFC に含まれるすべての FFR が対象となる．
  /// FFR と MFFC が一致している場合は gen_ffr_cnf と同じことになる．
  void
  gen_mffc_cnf(const TpgNetwork& network,
	       const TpgMFFC* mffc,
	       DtpgStats& stats);

  /// @brief テスト生成を行なう．
  /// @param[in] fault 対象の故障
  /// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
  /// @param[inout] stats DTPGの統計情報
  /// @return 結果を返す．
  ///
  /// 直前にどちらのモードでCNFを作っていたかで動作は異なる．<br>
  /// どちらの関数も呼んでいなければなにもしないで kB3X を返す．
  SatBool3
  dtpg(const TpgFault* fault,
       NodeValList& nodeval_list,
       DtpgStats& stats);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATタイプ
  string mSatType;

  // SATオプション
  string mSatOption;

  // SATのログ出力
  ostream* mSatOutP;

  // バックトレーサー
  BackTracer& mBackTracer;

  // 実装クラス
  DtpgImpl* mImpl;

};

END_NAMESPACE_DRUID_TD

#endif // TD_DTPG_H
