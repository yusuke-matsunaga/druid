#ifndef DOMGRAPH_H
#define DOMGRAPH_H

/// @file DomGraph.h
/// @brief DomGraph のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

class DichoCandMgr;
class DomNode;

//////////////////////////////////////////////////////////////////////
/// @class DomGraph DomGraph.h "DomGraph.h"
/// @brief 故障の支配関係を表すグラフ
//////////////////////////////////////////////////////////////////////
class DomGraph
{
public:

  /// @brief コンストラクタ
  DomGraph(
    const DichoCandMgr& candmgr
  );

  /// @brief デストラクタ
  ~DomGraph() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードのランクを返す．
  SizeType
  rank(
    SizeType id ///< [in] ノード番号
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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノードのリスト
  std::vector<std::unique_ptr<DomNode>> mNodeList;

  // ノード番号をキーにして DomNode を持つ辞書
  std::unordered_map<SizeType, DomNode*> mNodeMap;

};


//////////////////////////////////////////////////////////////////////
/// @class DomNode DomGraph.h "DomGraph.h"
/// @brief DomGraph のノードを表すクラス
//////////////////////////////////////////////////////////////////////
class DomNode
{
  friend class DomGraph;

public:

  /// @brief コンストラクタ
  DomNode(
    SizeType id ///< [in] ノード番号
  ) : mId{id}
  {
  }

  /// @brief デストラクタ
  ~DomNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief ランクを返す．
  SizeType
  rank() const
  {
    return mRank;
  }

  /// @grief 内容を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const
  {
    s << "Node#" << id();
    if ( mHasRank ) {
      s << "@" << mRank;
    }
    else {
      s << "[" << mCount << "]";
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号
  SizeType mId;

  // ランク
  SizeType mRank{0};

  bool mHasRank{false};

  SizeType mCount{0};

  // 支配する故障のリスト
  std::vector<DomNode*> mDownLink;

  // 支配している故障のリスト
  std::vector<DomNode*> mUpLink;

};

END_NAMESPACE_DRUID

#endif // DOMGRAPH_H
