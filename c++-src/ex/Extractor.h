#ifndef EXTRACTOR_H
#define EXTRACTOR_H

/// @file Extractor.h
/// @brief Extractor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014, 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "NodeValList.h"
#include "VidMap.h"
#include "Val3.h"
//#include "ym/SatBool3.h"
#include "ym/SatModel.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Extractor Extractor.h "Extractor.h"
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
/// という選択で答が変わりうる．
//////////////////////////////////////////////////////////////////////
class Extractor
{
public:

  /// @brief コンストラクタ
  Extractor(
    const TpgNode* root,    ///< [in] 起点となるノード
    const VidMap& gvar_map, ///< [in] 正常値の変数番号のマップ
    const VidMap& fvar_map, ///< [in] 故障値の変数番号のマップ
    const SatModel& model   ///< [in] SATソルバの作ったモデル
  );

  /// @brief デストラクタ
  ~Extractor() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 値割り当てを１つ求める．
  /// @return 値の割当リスト
  NodeValList
  get_assignment();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの種類を求める．
  /// @retval 1 故障差が伝搬している．
  /// @retval 2 故障差が伝搬していない．
  /// @retval 3 fcone の外側
  int
  type(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    if ( mFconeMark.count(node->id()) > 0 ) {
      if ( gval(node) != fval(node) ) {
	return 1;
      }
      return 2;
    }
    return 3;
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
    const TpgNode* node ///< [in] 対象のノード
  );

  void
  put_queue(
    const TpgNode* node,
    int mark
  )
  {
    if ( mMarks.count(node->id()) == 0 ) {
      mMarks.emplace(node->id(), mark);
      mQueue.push_back(node);
    }
  }

  /// @brief 正常回路の値を返す．
  Val3
  gval(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return bool3_to_val3(mSatModel[mGvarMap(node)]);
  }

  /// @brief 故障回路の値を返す．
  Val3
  fval(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return bool3_to_val3(mSatModel[mFvarMap(node)]);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 起点となるノード
  const TpgNode* mRoot;

  // 正常値を表す変数のマップ
  const VidMap& mGvarMap;

  // 故障値を表す変数のマップ
  const VidMap& mFvarMap;

  // SAT ソルバの解
  const SatModel& mSatModel;

  // 故障の fanout cone のマーク
  unordered_set<SizeType> mFconeMark;

  // 故障差の伝搬している外部出力のリスト
  vector<const TpgNode*> mSpoList;

  // 処理対象のノードをためておくキュー
  vector<const TpgNode*> mQueue;

  // キューに積んだノードの印
  unordered_map<SizeType, SizeType> mMarks;

};

END_NAMESPACE_DRUID

#endif // EXTRACTOR_H
