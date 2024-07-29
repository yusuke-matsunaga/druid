#ifndef DTPGENGINE_MFFC_H
#define DTPGENGINE_MFFC_H

/// @file DtpgEngine_MFFC.h
/// @brief DtpgEngine_MFFC のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgEngine_MFFC DtpgEngine_MFFC.h "DtpgEngine_MFFC.h"
/// @brief MFFC 単位で DTPG の基本的な処理を行うクラス
//////////////////////////////////////////////////////////////////////
class DtpgEngine_MFFC :
  public DtpgEngine
{
public:

  /// @brief コンストラクタ
  DtpgEngine_MFFC(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgMFFC* mffc,       ///< [in] 故障伝搬の起点となる MFFC
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DtpgEngine_MFFC();


private:
  //////////////////////////////////////////////////////////////////////
  // DtpgEngine の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬の起点ノードを返す．
  const TpgNode*
  fault_origin(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

  /// @brief 故障の活性化条件
  AssignList
  fault_condition(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

  /// @brief gen_pattern() で用いる追加の検出条件
  vector<SatLiteral>
  extra_assumptions(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief make_cnf() の追加処理
  void
  opt_make_cnf();

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
  const TpgMFFC* mMFFC;

  // FFR の根のリスト
  // [0] は MFFC の根でもある．
  vector<const TpgNode*> mRootArray;

  // 各FFRの根に反転イベントを挿入するための変数
  // サイズは mRootArray.size()
  vector<SatLiteral> mEvarArray;

  // ノード番号をキーにしてFFR番号を入れる連想配列
  unordered_map<SizeType, SizeType> mFFRIdMap;

};

END_NAMESPACE_DRUID

#endif // DTPGENGINE_MFFC_H
