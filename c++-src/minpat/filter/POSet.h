#ifndef POSET_H
#define POSET_H

/// @file POSet.h
/// @brief POSet のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class PONode;

//////////////////////////////////////////////////////////////////////
/// @class POSet POSet.h "POSet.h"
/// @brief Partially Ordered Set の構造を表すクラス
///
/// いわゆる順序関係の構造を表すグラフ
/// 順序関係とは以下の性質を満たす二項関係である．
/// - 反射律:   a -> a がすべて a に対して成り立つ．
/// - 反対称律: a -> b かつ b -> a が成り立つのは a = b の時のみ
/// - 推移律:   a -> b かつ b -> c でるようなすべての
///            a, b, c に対して a -> c が成り立つ．
///
/// 与えられた順序関係から推移律を用いることで導出できる関係を除外することを
/// 推移簡約(transitive reduction)と呼ぶ．
/// このクラスは主に推移簡約を行うために用いられる．
//////////////////////////////////////////////////////////////////////
class POSet
{
public:

  /// @brief ビルダクラス
  class Builder
  {
  public:

    /// @brief 空のコンストラクタ
    Builder() = default;

    /// @brief デストラクタ
    ~Builder() = default;

    /// @brief id1 < id2 の順序関係を追加する．
    void
    add(
      SizeType id1, ///< [in] 要素番号1
      SizeType id2  ///< [in] 要素番号2
    )
    {
      mSize = std::max(mSize, id1 + 1);
      mSize = std::max(mSize, id2 + 1);
      mElemList.push_back({id1, id2});
    }

    /// @brief 要素対の数
    SizeType
    elem_size() const
    {
      return mElemList.size();
    }

    // 要素番号のサイズ(最大値+1)
    SizeType mSize{0};

    // 要素対のリスト
    std::vector<std::pair<SizeType, SizeType>> mElemList;

  };


public:

  /// @brief 空のコンストラクタ
  POSet();

  /// @brief コンストラクタ
  POSet(
    const Builder& builder ///< [in] ビルダオブジェクト
  );

  /// @brief デストラクタ
  ~POSet();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 再初期化
  ///
  /// 以前の内容は消去される．
  void
  rebuild(
    const Builder& builder ///< [in] ビルダオブジェクト
  );

  /// @brief 要素数を返す．
  SizeType
  elem_num() const
  {
    return mNodeList.size();
  }

  /// @brief 後続の要素番号のリストを返す．
  std::vector<SizeType>
  succ_list(
    SizeType id ///< [in] 要素番号 ( 0 <= id < elem_num() )
  ) const;

  /// @brief 先行の要素番号のリストを返す．
  std::vector<SizeType>
  pred_list(
    SizeType id ///< [in] 要素番号 ( 0 <= id < elem_num() )
  ) const;

  /// @brief 直接の後続の要素番号のリストを返す．
  std::vector<SizeType>
  imm_succ_list(
    SizeType id ///< [in] 要素番号
  ) const;

  /// @brief ランクを返す．
  SizeType
  rank(
    SizeType id ///< [in] 要素番号 ( 0 <= id < elem_num() )
  ) const;

  /// @brief 内容を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を設定する．
  void
  _set(
    const Builder& builder ///< [in] ビルダオブジェクト
  );

  /// @brief 到達可能か調べる．
  static
  bool
  _reachable(
    PONode* from,
    PONode* to
  );

  /// @brief _reachable の下請け関数
  static
  bool
  _reachable_sub(
    PONode* node,
    std::unordered_set<SizeType>& mark,
    PONode* to
  );

  /// @brief traverse の下請け関数
  void
  _dfs(
    SizeType id,                         ///< [in] 要素番号
    std::vector<int>& mark,              ///< [in] マーク
    std::vector<SizeType>& media_list,   ///< [out] 内側の要素番号のリスト
    std::vector<SizeType>& boundary_list ///< [out] 境界の要素番号のリスト
  ) const;

  /// @brief ノードを取り出す．
  const PONode*
  _node(
    SizeType id ///< [in] 要素番号
  ) const
  {
    if ( id >= elem_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
    return mNodeList[id].get();
  }

  /// @brief ノードのリストを要素番号のリストに変換する．
  std::vector<SizeType>
  _id_list(
    const std::vector<PONode*>& node_list
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノードのリスト
  std::vector<std::unique_ptr<PONode>> mNodeList;

  // ランクごとの要素番号のリスト
  std::vector<std::vector<SizeType>> mRankArray;

};

END_NAMESPACE_DRUID

#endif // POSET_H
