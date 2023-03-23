#ifndef MFFCENGINE_H
#define MFFCENGINE_H

/// @file MFFCEngine.h
/// @brief MFFCEngine のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MFFCEngine MFFCEngine.h "MFFCEngine.h"
/// @brief MFFC 単位で DTPG の基本的な処理を行うクラス
//////////////////////////////////////////////////////////////////////
class MFFCEngine :
  public DtpgEngine
{
public:

  /// @brief コンストラクタ
  MFFCEngine(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    bool has_prev_state,	     ///< [in] 1時刻前の回路を持つ時 true
    const TpgMFFC& mffc,	     ///< [in] 故障伝搬の起点となる MFFC
    const SatSolverType& solver_type ///< [in] SATソルバの実装タイプ
  );

  /// @brief デストラクタ
  ~MFFCEngine();


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief make_cnf() の追加処理
  void
  opt_make_cnf() override;

  /// @brief gen_pattern() で用いる検出条件を作る．
  ///
  /// デフォルトでは空を返す．
  vector<SatLiteral>
  gen_assumptions(
    const TpgFault& fault ///< [in] 対象の故障
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

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

#endif // MFFCENGINE_H
