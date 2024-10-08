#ifndef EXTRACTOR_H
#define EXTRACTOR_H

/// @file Extractor.h
/// @brief Extractor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ExData.h"
#include "AssignList.h"
#include "VidMap.h"
#include "Val3.h"
#include "ym/SatModel.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Extractor Extractor.h "Extractor.h"
/// @brief 十分割当を求めるクラス
///
/// 起点となるノードから外部出力まで故障の影響が伝搬する条件を求める．
/// その際に，正常回路の値だけで伝搬が保障される十分な割り当てとなる
/// ようにする．
///
/// 実際の処理は ExImpl が行う．このクラスは ExImpl のスマートポインタ
//////////////////////////////////////////////////////////////////////
class Extractor
{
public:

  /// @brief インスタンスを生成するクラスメソッド
  static
  Extractor*
  new_impl(
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

  /// @brief コンストラクタ
  Extractor() = default;

  /// @brief デストラクタ
  virtual
  ~Extractor() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 値割り当てを１つ求める．
  /// @return 値の割当リスト
  AssignList
  operator()(
    const TpgNode* root,    ///< [in] 起点となるノード
    const VidMap& gvar_map, ///< [in] 正常値の変数番号のマップ
    const VidMap& fvar_map, ///< [in] 故障値の変数番号のマップ
    const SatModel& model   ///< [in] SATソルバの作ったモデル
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 動作結果に影響を及ぼすヒューリスティックを実現する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 制御値を持つ入力を選ぶ．
  virtual
  vector<const TpgNode*>
  select_cnode(
    const vector<vector<const TpgNode*>>& choice_list ///< [in] 選択ノードのリスト
  ) = 0;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // データ
  ExData* mData{nullptr};

};

END_NAMESPACE_DRUID

#endif // EXTRACTOR_H
