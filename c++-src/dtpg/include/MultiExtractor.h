#ifndef MULTIEXTRACTOR_H
#define MULTIEXTRACTOR_H

/// @file MultiExtractor.h
/// @brief MultiExtractor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ExData.h"
#include "AssignList.h"
#include "AssignExpr.h"
#include "VidMap.h"
#include "Val3.h"
#include "ym/SatModel.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MultiExtractor MultiExtractor.h "MultiExtractor.h"
/// @brief 十分割当を求めるクラス
///
/// 起点となるノードから外部出力まで故障の影響が伝搬する条件を求める．
/// その際に，正常回路の値だけで伝搬が保障される十分な割り当てとなる
/// ようにする．
///
/// ここでの割り当ては
/// - どの外部出力を選ぶのか
/// - 制御値で故障差の伝搬をマスクしている場合に複数のファンインが制御値
///   を持っていた場合にどれを選ぶのか
/// という選択を全て求める．
//////////////////////////////////////////////////////////////////////
class MultiExtractor
{
public:

  /// @brief インスタンスを生成するクラスメソッド
  static
  MultiExtractor*
  new_impl(
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

  /// @brief コンストラクタ
  MultiExtractor() = default;

  /// @brief デストラクタ
  ~MultiExtractor() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 各出力へ故障伝搬する値割り当てを求める．
  /// @return 複数の値割り当てを返す．
  AssignExpr
  operator()(
    const TpgNode* root,    ///< [in] 起点となるノード
    const VidMap& gvar_map, ///< [in] 正常値の変数番号のマップ
    const VidMap& fvar_map, ///< [in] 故障値の変数番号のマップ
    const SatModel& model   ///< [in] SATソルバの作ったモデル
  );

};

END_NAMESPACE_DRUID

#endif // MULTIEXTRACTOR_H
