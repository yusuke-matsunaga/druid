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


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFREnc FFREnc.h "FFREnc.h"
/// @brief FFR内の故障伝搬条件を表すCNFを作るクラス
///
/// FFR内の故障伝搬条件は論理積で表せるのでCNF式を作る必要はないが，
/// 故障伝搬しない条件を表すために個々の場所ごとに変数を用意する．
//////////////////////////////////////////////////////////////////////
class FFREnc :
  public SubEnc
{
public:

  /// @brief コンストラクタ
  FFREnc(
    BaseEnc& base_enc, ///< [in] 親の BaseEnc
    const TpgFFR* ffr  ///< [in] 対象の FFR
  );

  /// @brief デストラクタ
  ~FFREnc() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬条件を表す変数を返す．
  ///
  /// node の出力から FFR の根のノードの出力までの伝搬条件
  SatLiteral
  prop_var(
    const TpgNode* node ///< [in] 起点となるノード
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


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief make_cnf() の下請け関数
  void
  make_cnf_sub(
    const TpgNode* node,
    const vector<SatLiteral>& cond
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のFFR
  const TpgFFR* mFFR;

  // 伝搬条件を表す変数の辞書
  // キーはノード番号
  unordered_map<SizeType, SatLiteral> mPropVarMap;

};

END_NAMESPACE_DRUID

#endif // FFRENC_H
