#ifndef EXTRACTOR_H
#define EXTRACTOR_H

/// @file Extractor.h
/// @brief Extractor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/AssignList.h"
#include "dtpg/VidMap.h"
#include "types/Val3.h"
#include "ym/SatModel.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class PropGraph;

//////////////////////////////////////////////////////////////////////
/// @class Extractor Extractor.h "Extractor.h"
/// @brief 十分割当を求めるクラス
///
/// 起点となるノードから外部出力まで故障の影響が伝搬する条件を求める．
/// その際に，正常回路の値だけで伝搬が保障される十分な割り当てとなる
/// ようにする．
///
/// 具体的なアルゴリズムは以下の通り
/// - 起点となるノードの TFO のノードに印をつける．
/// - 印のついた外部出力ノードのうち，故障の影響が伝搬しているノード
///   のリストを作る．
/// - 外部出力ごとに以下の処理を行う．
///   * そのノードの出力に故障の影響が伝搬する条件を求める．
///   * 具体的にはTFO以外のノードの場合はその値(かならず非制御値のはず)
///     を条件に加える．
///   * TFOに含まれるノードの場合はそのノードに再帰して同様の処理を行う．
/// - 単純にはTFOに含まれるノードの入力のうち，TFOに含まれないノード
///   (side input) の値をすべて条件に加えればよいが，制御値を持つ場合
///   (これは故障の伝搬を阻害している)には一つだけ選べば良い．
/// - 結果としてAND条件として非制御値を持つノードのリストと
///   制御値を持つノードグループのリストが得られることになる．
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
  SuffCond
  operator()(
    const TpgNode& root,          ///< [in] 起点となるノード
    const VidMap& gvar_map,       ///< [in] 正常値の変数番号のマップ
    const VidMap& fvar_map,       ///< [in] 故障値の変数番号のマップ
    const SatModel& model,        ///< [in] SATソルバの作ったモデル
    const AssignList& assign_list ///< [in] 事前の割り当て
  );

  /// @brief 値割り当てを１つ求める．
  /// @return 値の割当リスト
  SuffCond
  operator()(
    const TpgNode& root,          ///< [in] 起点となるノード
    const VidMap& gvar_map,       ///< [in] 正常値の変数番号のマップ
    const VidMap& fvar_map,       ///< [in] 故障値の変数番号のマップ
    const TpgNode& output,        ///< [in] 故障の影響が伝搬している外部出力
    const SatModel& model,        ///< [in] SATソルバの作ったモデル
    const AssignList& assign_list ///< [in] 事前の割り当て
  );


private:
  //////////////////////////////////////////////////////////////////////
  // Extractor の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 指定された出力からバックトレースを行う．
  virtual
  SuffCond
  backtrace(
    const PropGraph& data, ///< [in] 故障伝搬の情報
    const TpgNode& output  ///< [in] 対象の出力ノード
  ) = 0;

};

END_NAMESPACE_DRUID

#endif // EXTRACTOR_H
