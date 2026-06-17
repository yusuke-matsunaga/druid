#ifndef DPATGRAPH_H
#define DPATGRAPH_H

/// @file DPatGraph.h
/// @brief DPatGraph のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/PackedVal.h"
#include "POSet.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DPatGraph DPatGraph.h "DPatGraph.h"
/// @brief PackedVal の支配関係を表すグラフ
///
/// pat1 が pat2 に含まれるとき pat1 が pat2 を支配すると定義する．
/// これだけならただのビットワイズ演算でチェック可能だが，
/// ここでは与えられたパタンの集合の支配関係をグラフにして直接の支配関係
/// のみを取り出す．
/// つまり，推移律を用いて導かれる支配関係は記録しない．
///
/// 内部ではパタン中の1の数で区別する．
//////////////////////////////////////////////////////////////////////
class DPatGraph
{
public:

  /// @brief 空のコンストラクタ
  DPatGraph();

  /// @brief コンストラクタ
  DPatGraph(
    const std::vector<PackedVal>& pat_list ///< [in] パタンのリスト
  );

  /// @brief デストラクタ
  ~DPatGraph();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をセットする．
  void
  rebuild(
    const std::vector<PackedVal>& pat_list ///< [in] パタンのリスト
  );

  /// @brief パタンのリストを返す．
  const std::vector<PackedVal>&
  pat_list() const
  {
    return mPatList;
  }

  /// @brief ID番号からパタンを返す．
  PackedVal
  pat(
    SizeType id ///< [in] パタン番号
  ) const
  {
    _check_id(id);
    return mPatList[id];
  }

  /// @brief パタンからID番号を返す．
  SizeType
  id(
    PackedVal pat ///< [in] パタン
  ) const
  {
    return mIdMap.at(pat);
  }

  /// @brief 直接の後続パタンのリストを返す．
  ///
  /// pat はこの集合に含まれているとは限らない．
  std::vector<PackedVal>
  imm_succ_list(
    PackedVal pat ///< [in] 対象のパタン
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

  /// @brief 内容をセットする．
  void
  _set(
    const std::vector<PackedVal>& pat_list ///< [in] パタンのリスト
  );

  /// @brief ID番号をチェックする．
  void
  _check_id(
    SizeType id ///< [in] パタン番号
  ) const
  {
    if ( id >= mPatList.size() ) {
      throw std::out_of_range{"id is out of range"};
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パタンのリスト
  std::vector<PackedVal> mPatList;

  // パタンをキーにしてノード番号を持つ辞書
  // mPatList の逆関数
  std::unordered_map<PackedVal, SizeType> mIdMap;

  // ランク(パタン中の1の数)ごとのパタンのリストの配列
  std::vector<std::vector<PackedVal>> mLayeredList;

};

END_NAMESPACE_DRUID

#endif // DPATGRAPH_H
