#ifndef FFRENC_H
#define FFRENC_H

/// @file FFREnc.h
/// @brief FFREnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "types/TpgNodeList.h"
#include "types/TpgFFR.h"
#include "types/TpgFaultList.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFREnc FFREnc.h "FFREnc.h"
/// @brief FFR内の故障伝搬条件を表すCNFを作るクラス
/// @ingroup DtpgGroup
/// @sa StructEngine
///
/// FFR内の故障伝搬条件は論理積で表せるのでCNF式を作る必要はないが，
/// 故障伝搬しない条件を表すために個々の場所ごとに変数を用意する．
///
/// FFRの出力から外部出力までの伝搬条件は BoolDiffEnc が担当するが，
/// このクラスは BoolDiffEnc::prop_var() を参照しているので
/// BoolDiffEnc を操作する必要はない．
//////////////////////////////////////////////////////////////////////
class FFREnc :
  public SubEnc
{
public:

  /// @brief コンストラクタ
  FFREnc(
    SatLiteral root_pvar, ///< [in] 根から外部出力までの伝搬条件を表す変数
    const TpgFFR& ffr,             ///< [in] 対象の FFR
    const TpgFaultList& fault_list ///< [in] 対象の故障リスト
  );

  /// @brief デストラクタ
  ~FFREnc() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬条件を表す変数を返す．
  ///
  /// FFR の根のノードの出力までの故障伝搬条件を返す．
  SatLiteral
  prop_var(
    const TpgFault& fault ///< [in] 対象の故障
  );


private:
  //////////////////////////////////////////////////////////////////////
  // SubEnc の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief データ構造の初期化を行う．
  void
  init() override;

  /// @brief 必要な変数を割り当て CNF 式を作る．
  void
  make_cnf() override;

  /// @brief 関連するノードのリストを返す．
  const TpgNodeList&
  node_list() const override;

  /// @brief 1時刻前の値に関連するノードのリストを返す．
  const TpgNodeList&
  prev_node_list() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief make_cnf() の下請け関数
  void
  make_cnf_sub(
    const TpgNode& node
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 根の故障伝搬条件
  SatLiteral mRootPropVar;

  // 対象のFFR
  TpgFFR mFFR;

  // 対象の故障リスト
  TpgFaultList mFaultList;

  // 関連するノードのリスト
  TpgNodeList mNodeList;

  // 1時刻前の値に関係するノードのリスト
  TpgNodeList mPrevNodeList;

  // ノードの伝搬条件を表す変数の辞書
  // キーはノード番号
  std::unordered_map<SizeType, SatLiteral> mPropNodeVarMap;

  // 故障伝搬条件を表す変数の辞書
  // キーは故障番号
  std::unordered_map<SizeType, SatLiteral> mPropFaultVarMap;

};

END_NAMESPACE_DRUID

#endif // FFRENC_H
