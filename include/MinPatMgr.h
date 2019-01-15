#ifndef MINPATMGR_H
#define MINPATMGR_H

/// @file MinPatMgr.h
/// @brief MinPatMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
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
  MinPatMgr();

  /// @brief デストラクタ
  ~MinPatMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障リストを縮約する．
  static
  void
  fault_reduction(vector<const TpgFault*>& fault_list,
		  const TpgNetwork& network,
		  FaultType fault_type,
		  const string& algorithm,
		  bool debug);

  /// @brief 極大両立集合を求める．
  static
  void
  gen_mcsets(const vector<TestVector>& tv_list,
	     vector<TestVector>& new_tv_list);

  /// @brief 彩色問題でパタン圧縮を行う．
  /// @param[in] tv_list 初期テストパタンのリスト
  /// @param[out] new_tv_list 圧縮結果のテストパタンのリスト
  /// @return 結果のパタン数を返す．
  static
  int
  coloring(const vector<const TpgFault*>& fault_list,
	   const vector<TestVector>& tv_list,
	   const TpgNetwork& network,
	   FaultType fault_type,
	   vector<TestVector>& new_tv_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 縮約を行う．
  /// @param[in] matrix 対象の被覆行列
  /// @param[in] graph 衝突グラフ
  /// @param[in] selected_cols この縮約で選択された列のリスト
  static
  void
  reduce(McMatrix& matrix,
	 MpColGraph& graph,
	 vector<int>& selected_cols);

  /// @brief ヒューリスティック1
  static
  void
  heuristic1(McMatrix& matrix,
	     MpColGraph& graph,
	     vector<int>& selected_cols);

  /// @brief 両立集合を取り出す．
  /// @param[in] graph 衝突グラフ
  /// @param[in] matrix 被覆行列
  /// @param[in] selected_nodes 選択済みのノードリスト
  /// @param[out] node_list 結果の両立集合を表すリスト
  ///
  /// * selected_nodes に含まれるノードは matrix からは削除されている．
  static
  void
  get_compatible_nodes(const MpColGraph& graph,
		       const McMatrix& matrix,
		       const vector<int>& selected_nodes,
		       vector<int>& node_list);

  /// @brief 彩色結果から新しいテストパタンのリストを生成する．
  /// @param[in] tv_list テストパタンのリスト
  /// @param[in] nc 彩色数
  /// @param[in] color_map 彩色結果
  /// @param[out] new_tv_list マージされたテストパタンのリスト
  static
  void
  merge_tv_list(const vector<TestVector>& tv_list,
		int nc,
		const vector<int>& color_map,
		vector<TestVector>& new_tv_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_DRUID

#endif // MINPATMGR_H
