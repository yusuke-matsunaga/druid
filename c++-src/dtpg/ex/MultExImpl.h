#ifndef MULTEXIMPL_H
#define MULTEXIMPL_H

/// @file MultExImpl.h
/// @brief MultExImpl のヘッダファイル
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
/// @class MultExImpl MultExImpl.h "MultExImpl.h"
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
class MultExImpl
{
public:

  /// @brief コンストラクタ
  MultExImpl(
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  ~MultExImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 各出力へ故障伝搬する値割り当てを求める．
  /// @return 複数の値割り当てを返す．
  vector<AssignList>
  get_assignments(
    const TpgNode* root,    ///< [in] 起点となるノード
    const VidMap& gvar_map, ///< [in] 正常値の変数番号のマップ
    const VidMap& fvar_map, ///< [in] 故障値の変数番号のマップ
    const SatModel& model   ///< [in] SATソルバの作ったモデル
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
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

  /// @brief 故障の影響の伝搬する値割当を記録する．
  ///
  /// node は TFO 内のノードでかつ故障差が伝搬している．
  void
  record_sensitized_node(
    const TpgNode* node ///< [in] 対象のノード
  );

  /// @brief 故障の影響の伝搬を阻害する値割当を記録する．
  ///
  /// node は TFO 内のノードかつ故障差が伝搬していない．
  void
  record_masking_node(
    const TpgNode* node,                     ///< [in] 対象のノード
    vector<vector<Assign>>& choice_list ///< [out] 選択リスト
  );

  /// @brief キューに積む．
  void
  put_queue(
    const TpgNode* node, ///< [in] 対象のノード
    int mark             ///< [in] マーク
  )
  {
    if ( mMarks.count(node->id()) == 0 ) {
      mMarks.emplace(node->id(), mark);
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
  unordered_map<SizeType, SizeType> mMarks;

};

END_NAMESPACE_DRUID

#endif // MULTEXIMPL_H
