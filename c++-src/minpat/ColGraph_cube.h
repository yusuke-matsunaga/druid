#ifndef COLGRAPH_CUBE_H
#define COLGRAPH_CUBE_H

/// @file ColGraph_cube.h
/// @brief ColGraph_cube のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BaseEnc.h"
#include "TestCover.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ColGraph_cube ColGraph_cube.h "ColGraph_cube.h"
/// @brief パタン圧縮用の彩色問題用のグラフを表すクラス
///
/// * 拡張テストキューブをグラフのノードに対応させる．
/// * 相反する割り当てを持つノード同士は衝突している．
/// * ただし，割り当てが衝突していなくても両立しないノードもあるので
///   最終的には全ての割り当てを行った上でSAT問題を解く必要がある．
/// * 最小彩色問題は互いに衝突しているノードに同じ色を割り当てない条件
///   で使う色の数を最小化する問題．
/// * ただし，同じ故障に関連するテストキューブは一つだけ彩色すればよい．
//////////////////////////////////////////////////////////////////////
class ColGraph_cube
{
public:

  /// @brief コンストラクタ
  ColGraph_cube(
    const TpgNetwork& network,           ///< [in] 対象のネットワーク
    const vector<TestCover>& cover_list, ///< [in] カバーのリスト
    const JsonValue& option              ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  ~ColGraph_cube();


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

  /// @brief ノード(キューブ)数を返す．
  SizeType
  node_num() const
  {
    return mNodeList.size();
  }

  /// @brief 故障数を返す．
  SizeType
  fault_num() const
  {
    return mFaultNum;
  }

  /// @brief 対応する値割当を返す．
  const AssignList&
  cube(
    SizeType id ///< [in] ノード番号( 0 <= id < node_num() )
  ) const
  {
    ASSERT_COND( 0 <= id && id < node_num() );

    return mNodeList[id].mCube;
  }

  /// @brief 対応する故障を返す．
  const TpgFault*
  fault(
    SizeType id ///< [in] ノード番号( 0 <= id < node_num() )
  ) const
  {
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
  );

  /// @brief ノードの adjacent degree を返す．
  SizeType
  adjacent_degree(
    SizeType id ///< [in] ノード番号 ( 0 <= id < node_num() )
  );

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
  new_color()
  {
    SizeType color = color_num() + 1;
    mGroupList.push_back({color});
    return color;
  }

  /// @brief ノードを色をつける．
  void
  set_color(
    SizeType id,   ///< [in] ノード番号 ( 0 <= id < node_num() )
    SizeType color ///< [in] 色番号 ( 1 <= color <= color_num() )
  );

  /// @brief ノードの集合を色をつける．
  void
  set_color(
    const vector<SizeType>& node_list, ///< [in] ノード番号のリスト
    SizeType color                     ///< [in] 色番号 ( 1 <= color <= color_num() )
  )
  {
    for ( auto id: node_list ) {
      set_color(id, color);
    }
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


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief assign1 と assign2 が衝突する時 true を返す．
  bool
  is_conflict(
    const AssignList& assign1,
    const AssignList& assign2
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノードの情報を表す構造体
  struct Node {
    // 故障
    const TpgFault* mFault;
    // テストキューブ
    AssignList mCube;
    // 色
    SizeType mColor;
    // 衝突しているノード番号のリスト
    vector<SizeType> mConflictList;
    // 衝突している色のリスト
    vector<SizeType> mConflictColList;
  };

  // 色(ノードグループ)を表す構造体
  struct Group {
    // 色
    SizeType mColor;
    // ノード番号のリスト
    vector<SizeType> mNodeList;
    // 値割り当てのリスト
    AssignList mAssignments;
  };

  // ネットワーク
  const TpgNetwork& mNetwork;

  // 基本エンコーダ
  BaseEnc mBaseEnc;

  // ノードのリスト
  vector<Node> mNodeList;

  // 故障数
  SizeType mFaultNum;

  // 故障番号をキーにして関連するノード番号を格納する配列
  vector<vector<SizeType>> mCubeListArray;

  // 色(ノードグループ)のリスト
  // 0 は未彩色を表すのでキーは一つずれている．
  vector<Group> mGroupList;

};

END_NAMESPACE_DRUID

#endif // COLGRAPH_CUBE_H
