#ifndef EXIMPL_H
#define EXIMPL_H

/// @file ExImpl.h
/// @brief ExImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ExData.h"
#include "NodeTimeValList.h"
#include "VidMap.h"
#include "Val3.h"
#include "ym/SatModel.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ExImpl ExImpl.h "ExImpl.h"
/// @brief Extractor の実装を行うクラス
///
/// ここでの割り当ては
/// - どの外部出力を選ぶのか
/// - 制御値で故障差の伝搬をマスクしている場合に複数のファンインが制御値
///   を持っていた場合にどれを選ぶのか
/// という選択で答が変わりうる．
//////////////////////////////////////////////////////////////////////
class ExImpl
{
public:

  /// @brief コンストラクタ
  ExImpl();

  /// @brief デストラクタ
  virtual
  ~ExImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 値割り当てを１つ求める．
  /// @return 値の割当リスト
  NodeTimeValList
  get_assignment(
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

  /// @brief バックトレース対象の出力を選ぶ．
  virtual
  const TpgNode*
  select_output() = 0;

  /// @brief 制御値を持つ入力を選ぶ．
  virtual
  const TpgNode*
  select_cnode(
    const vector<const TpgNode*>& node_list ///< [in] 候補のノードのリスト
  ) = 0;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

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
    const TpgNode* node ///< [in] 対象のノード
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

#endif // EXIMPL_H
