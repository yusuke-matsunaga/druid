#ifndef MINPATMGR_H
#define MINPATMGR_H

/// @file MinPatMgr.h
/// @brief MinPatMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TestVector.h"
#include "ym/McMatrix.h"


BEGIN_NAMESPACE_DRUID

class MpColGraph;

//////////////////////////////////////////////////////////////////////
/// @class MinPatMgr MinPatMgr.h "MinPatMgr.h"
/// @brief テストセット最小化を行うクラス
//////////////////////////////////////////////////////////////////////
class MinPatMgr
{
public:

  /// @brief コンストラクタ
  MinPatMgr() = default;

  /// @brief デストラクタ
  ~MinPatMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障リストを縮約する．
  static
  void
  fault_reduction(
    vector<const TpgFault*>& fault_list,
    const TpgNetwork& network,
    FaultType fault_type,
    const string& algorithm,
    bool debug
  );

  /// @brief 極大両立集合を求める．
  static
  void
  gen_mcsets(
    const vector<TestVector>& tv_list,
    vector<TestVector>& new_tv_list
  );

  /// @brief 彩色問題でパタン圧縮を行う．
  /// @return 結果のパタン数を返す．
  static
  SizeType
  coloring(
    const vector<const TpgFault*>& fault_list,
    const vector<TestVector>& tv_list,	       ///< [in] 初期テストパタンのリスト
    const TpgNetwork& network,
    FaultType fault_type,
    vector<TestVector>& new_tv_list            ///< [out] 圧縮結果のテストパタンのリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 縮約を行う．
  static
  void
  reduce(
    McMatrix& matrix,          ///< [in] 対象の被覆行列
    MpColGraph& graph,	       ///< [in] 衝突グラフ
    vector<SizeType>& selected_cols ///< [in] この縮約で選択された列のリスト
  );

  /// @brief ヒューリスティック1
  static
  void
  heuristic1(
    McMatrix& matrix,
    MpColGraph& graph,
    vector<SizeType>& selected_cols
  );

  /// @brief 両立集合を取り出す．
  ///
  /// * selected_nodes に含まれるノードは matrix からは削除されている．
  static
  void
  get_compatible_nodes(
    const MpColGraph& graph,           ///< [in] 衝突グラフ
    const McMatrix& matrix,	       ///< [in] 被覆行列
    const vector<SizeType>& selected_nodes, ///< [in] 選択済みのノードリスト
    vector<SizeType>& node_list	       ///< [out] 結果の両立集合を表すリスト
  );

  /// @brief 彩色結果から新しいテストパタンのリストを生成する．
  static
  void
  merge_tv_list(
    const vector<TestVector>& tv_list, ///< [in] テストパタンのリスト
    SizeType nc,		       ///< [in] 彩色数
    const vector<SizeType>& color_map, ///< [in] 彩色結果
    vector<TestVector>& new_tv_list    ///< [out] マージされたテストパタンのリスト
  );

};

END_NAMESPACE_DRUID

#endif // MINPATMGR_H
