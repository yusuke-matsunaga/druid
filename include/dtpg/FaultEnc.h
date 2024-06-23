#ifndef FAULTENC_H
#define FAULTENC_H

/// @file FaultEnc.h
/// @brief FaultEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BaseEnc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultEnc.h FaultEnc.h "FaultEnc.h"
/// @brief 故障の検出条件を表すCNF式を生成するクラス
///
/// 実は検出条件は SatLiteral の論理積なので節を作る必要はない．
//////////////////////////////////////////////////////////////////////
class FaultEnc
{
public:

  /// @brief コンストラクタ
  FaultEnc(
    BaseEnc& base_enc,    ///< [in] 回路全体のCNF式を作るエンコーダー
    const TpgFault* fault ///< [in] 対象の故障
  );

  /// @brief デストラクタ
  ~FaultEnc() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の活性化条件を表す変数を返す．
  ///
  /// 故障の影響が fault->origin_node() の出力に現れる条件
  SatLiteral
  prop_var()
  {
    return mPropVar;
  }


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
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障
  const TpgFault* mFault;

  // 関係するノードのリスト
  vector<const TpgNode*> mNodeList;

  // 故障の活性化条件を表す変数
  SatLiteral mPropVar;

};

END_NAMESPACE_DRUID

#endif // FAULTENC_H
