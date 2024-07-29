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


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief データを得る．
  const ExData&
  data() const
  {
    return *mData;
  }

  /// @brief 起点のノードを返す．
  const TpgNode*
  root() const
  {
    return data().root();
  }

  /// @brief 故障差の伝搬している出力のリストを返す．
  const vector<const TpgNode*>&
  sensitized_output_list() const
  {
    return data().sensitized_output_list();
  }

  /// @brief root() から到達可能なノードの時に true を返す．
  bool
  is_in_fcone(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return data().is_in_fcone(node);
  }

  /// @brief 正常回路の値を返す．
  Val3
  gval(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return data().gval(node);
  }

  /// @brief 故障回路の値を返す．
  Val3
  fval(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return data().fval(node);
  }

  /// @brief ノードの種類を求める．
  /// @retval 1 故障差が伝搬している．
  /// @retval 2 故障差が伝搬していない．
  /// @retval 3 fcone の外側
  int
  type(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return data().type(node);
  }


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
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障差の伝搬している経路の side input の値を記録する．
  AssignList
  backtrace(
    const TpgNode* po,                          ///< [in] 起点となる外部出力ノード
    vector<vector<const TpgNode*>>& choice_list ///< [out] 選択ノードのリスト
  );

  /// @brief 故障の影響の伝搬する値割当を記録する．
  ///
  /// node は TFO 内のノードでかつ故障差が伝搬している．
  void
  record_sensitized_node(
    const TpgNode* node,    ///< [in] 対象のノード
    AssignList& assign_list ///< [in] 値を記録するリスト
  );

  /// @brief 故障の影響の伝搬を阻害する値割当を記録する．
  ///
  /// node は TFO 内のノードかつ故障差が伝搬していない．
  void
  record_masking_node(
    const TpgNode* node,                        ///< [in] 対象のノード
    AssignList& assign_list,                    ///< [in] side input の割り当てリスト
    vector<vector<const TpgNode*>>& choice_list ///< [out] 選択ノードのリスト
  );

  /// @brief side input の値を記録する．
  void
  record_side_val(
    const TpgNode* node,    ///< [in] 対象ノード
    AssignList& assign_list ///< [in] 値を記録するリスト
  );

  /// @brief キューに積む．
  void
  put_queue(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    if ( mMarks.count(node->id()) == 0 ) {
      mMarks.emplace(node->id());
      mQueue.push_back(node);
    }
  }

  /// @brief キューから取り出す．
  const TpgNode*
  get_queue()
  {
    auto node = mQueue.front();
    mQueue.pop_front();
    return node;
  }

  /// @brief キューをクリアする．
  void
  clear_queue()
  {
    mQueue.clear();
    mMarks.clear();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // データ
  ExData* mData{nullptr};

  // 処理対象のノードをためておくキュー
  std::deque<const TpgNode*> mQueue;

  // キューに積んだノードの印
  std::unordered_set<SizeType> mMarks;

};

END_NAMESPACE_DRUID

#endif // EXTRACTOR_H
