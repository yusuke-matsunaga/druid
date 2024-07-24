#ifndef COLGRAPH_H
#define COLGRAPH_H

/// @file ColGraph.h
/// @brief ColGraph のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BaseEnc.h"
#include "TestCover.h"
#include "TestVector.h"
#include "Sim.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ColGraph ColGraph.h "ColGraph.h"
/// @brief パタン圧縮用の彩色問題用のグラフを表すクラス
///
/// * 故障をグラフのノードに対応させる．
/// * 相反する割り当てを持つノード同士は衝突している．
/// * 最小彩色問題は互いに衝突しているノードに同じ色を割り当てない条件
///   で使う色の数を最小化する問題．
//////////////////////////////////////////////////////////////////////
class ColGraph
{
public:

  /// @brief コンストラクタ
  ColGraph(
    const TpgNetwork& network,           ///< [in] 対象のネットワーク
    const vector<TestCover>& cover_list, ///< [in] カバーのリスト
    const JsonValue& option              ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  ~ColGraph();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief ノード(故障)数を返す．
  SizeType
  node_num() const
  {
    return mNodeList.size();
  }

  /// @brief 対応する故障を返す．
  const TpgFault*
  fault(
    SizeType id ///< [in] ノード番号( 0 <= id < node_num() )
  ) const
  {
    ASSERT_COND( 0 <= id && id < node_num() );

    return mNodeList[id].mFault;
  }

  /// @brief ノードの衝突リストを返す．
  const vector<SizeType>&
  conflict_list(
    SizeType id ///< [in] ノード番号 ( 0 <= id < node_num() )
  ) const
  {
    ASSERT_COND( 0 <= id && id < node_num() );

    return mNodeList[id].mConflictList;
  }

  /// @brief ノードの衝突色リストを返す．
  const vector<SizeType>&
  conflict_color_list(
    SizeType id ///< [in] ノード番号 ( 0 <= id < node_num() )
  ) const
  {
    ASSERT_COND( 0 <= id && id < node_num() );

    return mNodeList[id].mConflictColList;
  }

  /// @brief ノードの saturation degree を返す．
  SizeType
  saturation_degree(
    SizeType id ///< [in] ノード番号 ( 0 <= id < node_num() )
  )
  {
    return conflict_color_list(id).size();
  }

  /// @brief ノードの adjacent degree を返す．
  SizeType
  adjacent_degree(
    SizeType id ///< [in] ノード番号 ( 0 <= id < node_num() )
  )
  {
    ASSERT_COND( 0 <= id && id < node_num() );

    return mNodeList[id].mAdjDegree;
  }

  /// @brief 色数を返す．
  SizeType
  color_num() const
  {
    return mGroupList.size();
  }

  /// @brief ノードの色を得る．
  SizeType
  color(
    SizeType id ///< [in] ノード番号 ( 0 <= id < node_num() )
  ) const
  {
    ASSERT_COND( 0 <= id && id < node_num() );

    return mNodeList[id].mColor;
  }

  /// @brief 指定された色のテストベクタを返す．
  TestVector
  testvector(
    SizeType color
  );

  /// @brief 割り当てを充足させる外部入力の割り当てを求める．
  NodeTimeValList
  justify(
    const NodeTimeValList& assign_list ///< [in] 値割り当てのリスト
  );

  /// @brief 指定された色のノード番号のリストを返す．
  const vector<SizeType>&
  node_list(
    SizeType color
  ) const
  {
    ASSERT_COND( 1 <= color && color <= color_num() );

    return mGroupList[color - 1].mNodeList;
  }

  /// @brief 新しい色を割り当てる．
  /// @return 新しい色番号を返す．
  SizeType
  new_color();

  /// @brief ノードを色をつける．
  void
  set_color(
    SizeType id,   ///< [in] ノード番号 ( 0 <= id < node_num() )
    SizeType color ///< [in] 色番号 ( 1 <= color <= color_num() )
  )
  {
    _set_color(id, color);
    update_color(color);
  }

  /// @brief ノードの集合を色をつける．
  void
  set_color(
    const vector<SizeType>& node_list, ///< [in] ノード番号のリスト
    SizeType color                     ///< [in] 色番号 ( 1 <= color <= color_num() )
  )
  {
    for ( auto id: node_list ) {
      _set_color(id, color);
    }
    update_color(color);
  }

  /// @brief color_map を作る．
  /// @return 色数を返す．
  SizeType
  get_color_map(
    vector<SizeType>& color_map ///< [out] 結果を格納する変数
                                ///<       ノード番号をキーにして色を返す配列
  ) const;

  /// @brief 隣接しているノード対に同じ色が割り当てられていないか確認する．
  bool
  verify() const;

  /// @brief 2つのノードが衝突する時 true を返す(簡易版)．
  bool
  is_trivial_conflict(
    SizeType id1, ///< [in] ノード番号1
    SizeType id2  ///< [in] ノード番号2
  );

  /// @brief 2つのノードが衝突する時 true を返す．
  bool
  is_conflict(
    SizeType id1, ///< [in] ノード番号1
    SizeType id2  ///< [in] ノード番号2
  );

  /// @brief ノードとノード集合が衝突するとき true を返す．
  bool
  is_conflict(
    SizeType id1,                   ///< [in] ノード番号1
    const vector<SizeType>& id_list ///< [in] ノード番号のリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障検出条件を作る．
  SatLiteral
  make_cover_condition(
    const TestCover& cover
  );

  /// @brief set_color() の下請け関数
  void
  _set_color(
    SizeType id,   ///< [in] ノード番号
    SizeType color ///< [in] 色
  );

  /// @brief set_color() の後の更新処理
  void
  update_color(
    SizeType color ///< [in] 変更のあった色
  );

  /// @brief 衝突リストを作る．
  void
  make_conflict_list(
    SizeType limit
  );

  /// @brief 故障シミュレーションを用いて両立ペアの集合を作る(組み合わせ回路用)．
  ///
  /// 結果は mCompatMark に格納される．
  void
  make_compat_mark(
    SizeType limit
  );

  /// @brief 故障シミュレーションを用いて両立ペアの集合を作る(組み合わせ回路用)．
  ///
  /// 結果は mCompatMark2 に格納される．
  void
  make_compat_mark2(
    SizeType color,
    SizeType limit
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノードの情報を表す構造体
  struct Node {
    // コンストラクタ
    Node(
      const TestCover& cover,
      SatLiteral cvar
    );

    // 故障
    const TpgFault* mFault;
    // 検出条件のリスト
    vector<NodeTimeValList> mCubeList;
    // 色
    SizeType mColor{0};
    // 衝突しているノード番号のリスト
    vector<SizeType> mConflictList;
    // 衝突している色のリスト
    vector<SizeType> mConflictColList;
    // 対応する制御変数
    SatLiteral mControlVar;
    // adjacent degree
    SizeType mAdjDegree;
  };

  // 色(ノードグループ)を表す構造体
  struct Group {
    // 色
    SizeType mColor;
    // 含まれるノード番号のリスト
    vector<SizeType> mNodeList;
    // 両立しているノード番号のリスト
    vector<SizeType> mCompatList;
    // 検出パタン
    TestVector mPattern;
  };

  // ネットワーク
  const TpgNetwork& mNetwork;

  // 基本エンコーダ
  BaseEnc mBaseEnc;

  // ノードのリスト
  vector<Node> mNodeList;

  // 衝突ペアの集合
  std::unordered_set<SizeType> mConflictMark;

  // 両立ペアの集合
  std::unordered_set<SizeType> mCompatMark;

  // 両立ペアの集合
  std::unordered_set<SizeType> mCompatMark2;

  // シミュレータ
  Sim mSim;

  // 色(ノードグループ)のリスト
  // 0 は未彩色を表すのでキーは一つずれている．
  vector<Group> mGroupList;

  // デバッグフラグ
  bool mDebug{false};

};

END_NAMESPACE_DRUID

#endif // COLGRAPH_H
