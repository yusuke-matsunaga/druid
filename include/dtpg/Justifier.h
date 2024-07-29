#ifndef JUSTIFIER_H
#define JUSTIFIER_H

/// @file Justifier.h
/// @brief Justifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/sat.h"
#include "ym/JsonValue.h"


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
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~Justifier();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める
  /// @return 入力の値割り当て
  ///
  /// 縮退故障の場合は var1_map は無視される．
  AssignList
  operator()(
    const AssignList& assign_list, ///< [in] 値の割り当てリスト
    const VidMap& var1_map,	        ///< [in] 1時刻目の変数番号のマップ
    const VidMap& var2_map,	        ///< [in] 2時刻目の変数番号のマップ
    const SatModel& model               ///< [in] SAT問題の解
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 1時刻前の状態を持つ時 true にするフラグ
  bool mHasPrevState;

  // 実装クラス
  std::unique_ptr<JustImpl> mImpl;

};

END_NAMESPACE_DRUID

#endif // JUSTIFIER_H
