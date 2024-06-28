#ifndef FFRENC_H
#define FFRENC_H

/// @file FFREnc.h
/// @brief FFREnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BaseEnc.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class BoolDiffEnc;

//////////////////////////////////////////////////////////////////////
/// @class FFREnc FFREnc.h "FFREnc.h"
/// @brief FFR内の故障伝搬条件を表すCNFを作るクラス
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
  ///
  /// bd_enc が nullptr の場合には FFR 内の伝搬条件のみを考慮する．
  FFREnc(
    BaseEnc& base_enc,                        ///< [in] 親の BaseEnc
    BoolDiffEnc* bd_enc,                      ///< [in] FFR の出力の先のエンコーダ
    const TpgFFR* ffr,                        ///< [in] 対象の FFR
    const vector<const TpgFault*>& fault_list ///< [in] 対象の故障リスト
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
    const TpgFault* fault ///< [in] 対象の故障
  );


private:
  //////////////////////////////////////////////////////////////////////
  // SubEnc の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 必要な変数を割り当て CNF 式を作る．
  void
  make_cnf() override;

  /// @brief 関連するノードのリストを返す．
  const vector<const TpgNode*>&
  node_list() const override;

  /// @brief 1時刻前の値に関連するノードのリストを返す．
  const vector<const TpgNode*>&
  prev_node_list() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief make_cnf() の下請け関数
  void
  make_cnf_sub(
    const TpgNode* node
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 根の故障伝搬エンコーダ
  BoolDiffEnc* mBdEnc;

  // 対象のFFR
  const TpgFFR* mFFR;

  // 対象の故障リスト
  vector<const TpgFault*> mFaultList;

  // 1時刻前の値に関係するノードのリスト
  vector<const TpgNode*> mPrevNodeList;

  // ノードの伝搬条件を表す変数の辞書
  // キーはノード番号
  unordered_map<SizeType, SatLiteral> mPropNodeVarMap;

  // 故障伝搬条件を表す変数の辞書
  // キーは故障番号
  unordered_map<SizeType, SatLiteral> mPropFaultVarMap;

};

END_NAMESPACE_DRUID

#endif // FFRENC_H
