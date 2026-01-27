#ifndef FAULTENC_H
#define FAULTENC_H

/// @file FaultEnc.h
/// @brief FaultEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "types/TpgFault.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultEnc.h FaultEnc.h "FaultEnc.h"
/// @brief 故障の検出条件を表すCNF式を生成するクラス
///
/// 実は検出条件は SatLiteral の論理積なので節を作る必要はない．
//////////////////////////////////////////////////////////////////////
class FaultEnc :
  public SubEnc
{
public:

  /// @brief コンストラクタ
  FaultEnc(
    const TpgFault& fault ///< [in] 対象の故障
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
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障
  TpgFault mFault;

  // 関係するノードのリスト
  TpgNodeList mNodeList;

  // 1時刻前の値に関係するノードのリスト
  TpgNodeList mPrevList;

  // 故障の活性化条件を表す変数
  SatLiteral mPropVar;

};

END_NAMESPACE_DRUID

#endif // FAULTENC_H
