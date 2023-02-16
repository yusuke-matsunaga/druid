#ifndef JUSTIMPL_H
#define JUSTIMPL_H

/// @file JustImpl.h
/// @brief JustImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNode.h"
#include "NodeVal.h"
#include "ym/SatModel.h"


BEGIN_NAMESPACE_DRUID

class JustData;

//////////////////////////////////////////////////////////////////////
/// @class JustImpl JustImpl.h "JustImpl.h"
/// @brief Justifier の実装クラス
//////////////////////////////////////////////////////////////////////
class JustImpl
{
public:

  /// @brief コンストラクタ
  JustImpl(
    SizeType max_id ///< [in] ID番号の最大値
  );

  /// @brief デストラクタ
  virtual
  ~JustImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める(縮退故障用)．
  /// @return 外部入力上の値の割当リスト
  NodeValList
  justify(
    const NodeValList& assign_list, ///< [in] 値の割り当てリスト
    const VidMap& var_map,	    ///< [in] 変数番号のマップ
    const SatModel& model	    ///< [in] SAT問題の解
  );

  /// @brief 正当化に必要な割当を求める(遷移故障用)．
  /// @return 外部入力上の値の割当リスト
  NodeValList
  justify(
    const NodeValList& assign_list, ///< [in] 値の割り当てリスト
    const VidMap& var1_map,	    ///< [in] 1時刻目の変数番号のマップ
    const VidMap& var2_map,	    ///< [in] 2時刻目の変数番号のマップ
    const SatModel& model	    ///< [in] SAT問題の解
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief JustData を得る．
  const JustData&
  just_data()
  {
    return *mJustDataPtr;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief justify の実際の処理
  NodeValList
  _justify(
    const JustData& jd,
    const NodeValList& assign_list
  );

  /// @brief 初期化処理
  virtual
  void
  just_init(
    const NodeValList& assign_list ///< [in] 割当リスト
  ) = 0;

  /// @brief 正当化処理
  void
  just_main(
    const TpgNode* node,	///< [in] 対象のノード
    int time,			///< [in] 時刻 ( 0 or 1 )
    NodeValList& pi_assign_list	///< [in] 結果の割当を保持するリスト
  );

  /// @brief 制御値を持つファンインを一つ選ぶ．
  /// @return 選んだファンインのノードを返す．
  virtual
  const TpgNode*
  select_cval_node(
    const TpgNode* node, ///< [in] 対象のノード
    int time		 ///< [in] 時刻 ( 0 or 1 )
  ) = 0;

  /// @brief 終了処理
  virtual
  void
  just_end() = 0;


private:
  //////////////////////////////////////////////////////////////////////
  // マークに関するアクセス関数
  //////////////////////////////////////////////////////////////////////

  /// @brief キューに追加する
  void
  put_queue(
    const TpgNode* node, ///< [in] 対象のノード
    int time		 ///< [in] タイムフレーム ( 0 or 1 )
  )
  {
    if ( !mark(node, time) ) {
      set_mark(node, time);
      mQueue.push_back(NodeVal{node, time, false});
    }
  }

  /// @brief justified マークをつけ，mJustifiedNodeList に加える．
  void
  set_mark(
    const TpgNode* node, ///< [in] 対象のノード
    int time		 ///< [in] タイムフレーム ( 0 or 1 )
  )
  {
    // 念のため time の最下位ビットだけ使う．
    time &= 1;
    mMarkArray[node->id()][time] = true;
  }

  /// @brief justified マークを読む．
  bool
  mark(
    const TpgNode* node, ///< [in] 対象のノード
    int time		 ///< [in] タイムフレーム ( 0 or 1 )
  ) const
  {
    // 念のため time の最下位ビットだけ使う．
    time &= 1;
    return mMarkArray[node->id()][time];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 個々のノードのマークを表す配列
  vector<bitset<2>> mMarkArray;

  // JustData
  const JustData* mJustDataPtr{nullptr};

  // 対象のノードを入れるキュー
  vector<NodeVal> mQueue;

};

END_NAMESPACE_DRUID

#endif // JUSTIMPL_H
