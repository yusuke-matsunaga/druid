#ifndef JUSTIFIER_H
#define JUSTIFIER_H

/// @file Justifier.h
/// @brief Justifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/sat.h"


BEGIN_NAMESPACE_DRUID

class JustImpl;

//////////////////////////////////////////////////////////////////////
/// @class Justifier Justifier.h "Justifier.h"
/// @brief 正当化に必要な割当を求めるファンクター
///
/// SAT ソルバで解を求めた後で，必要最小限の PI の割当を求める．
/// 正当化が必要な値の割当は assign_list に入っている．
//////////////////////////////////////////////////////////////////////
class Justifier
{
public:

  /// @brief コンストラクタ
  Justifier(
    const string& type,       ///< [in] 種類を表す文字列
    const TpgNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief デストラクタ
  ~Justifier();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める
  /// @return テストベクタ
  TestVector
  operator()(
    FaultType fault_type,           ///< [in] 故障タイプ
    const NodeValList& assign_list, ///< [in] 値の割り当てリスト
    const VidMap& var1_map,	    ///< [in] 1時刻目の変数番号のマップ
    const VidMap& var2_map,	    ///< [in] 2時刻目の変数番号のマップ
    const SatModel& model           ///< [in] SAT問題の解
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  // input_num, dff_num を取得するために用いる．
  const TpgNetwork& mNetwork;

  // 実装クラス
  std::unique_ptr<JustImpl> mImpl;

};

END_NAMESPACE_DRUID

#endif // JUSTIFIER_H
