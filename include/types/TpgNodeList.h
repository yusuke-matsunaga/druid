#ifndef TPGNODELIST_H
#define TPGNODELIST_H

/// @file TpgNodeList.h
/// @brief TpgNodeList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgListBase.h"
#include "types/TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgNodeIter TpgNodeList.h "TpgNodeList.h"
/// @brief TpgNodeList の反復子
/// @ingroup TypesGroup
/// @sa TpgNode, TpgNodeList
//////////////////////////////////////////////////////////////////////
class TpgNodeIter :
  public TpgIterBase
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgNodeIter() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgNodeIter(
    const std::shared_ptr<NetworkRep>& network,
    std::vector<SizeType>::const_iterator iter
  ) : TpgIterBase(network, iter)
  {
  }

  /// @brief デストラクタ
  ~TpgNodeIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  TpgNode
  operator*() const
  {
    return TpgBase::node(get_id());
  }

};


//////////////////////////////////////////////////////////////////////
/// @class TpgNodeIter2 TpgNodeList.h "TpgNodeList.h"
/// @brief TpgNodeList の反復子2(Python用)
/// @ingroup TypesGroup
/// @sa TpgNode, TpgNodeList
//////////////////////////////////////////////////////////////////////
class TpgNodeIter2:
  public TpgIter2Base
{
public:

  /// @brief 空のコンストラクタ
  TpgNodeIter2() = default;

  /// @brief 値を指定したコンストラクタ
  TpgNodeIter2(
    const std::shared_ptr<NetworkRep>& network,
    IdIter cur,
    IdIter end
  ) : TpgIter2Base(network, cur, end)
  {
  }

  /// @brief デストラクタ
  ~TpgNodeIter2() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 次の要素を返す．
  ///
  /// has_next() == true と仮定している．
  TpgNode
  next()
  {
    auto id = next_id();
    return TpgBase::node(id);
  }

};


//////////////////////////////////////////////////////////////////////
/// @class TpgNodeList TpgNodeList.h "TpgNodeList.h"
/// @brief TpgNode のリスト
/// @ingroup TypesGroup
/// @sa TpgNode, TpgNodeIter
//////////////////////////////////////////////////////////////////////
class TpgNodeList :
  public TpgListBase
{
public:

  using iterator = TpgNodeIter;
  using iterator2 = TpgNodeIter2;

public:

  /// @brief 空のコンストラクタ
  TpgNodeList() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgNodeList(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    const std::vector<SizeType>& id_list        ///< [in] ノード番号のリスト
    = {}
  ) : TpgListBase(network, id_list)
  {
  }

  /// @brief デストラクタ
  ~TpgNodeList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素を返す．
  TpgNode
  operator[](
    SizeType index ///< [in] インデックス （ 0 <= index < size() );
  ) const
  {
    return TpgBase::node(get_id(index));
  }

  /// @brief 先頭の反復子を返す．
  iterator
  begin() const
  {
    return iterator(_network(), begin_iter());
  }

  /// @brief 末尾の反復子を返す．
  iterator
  end() const
  {
    return iterator(_network(), end_iter());
  }

  /// @brief Python 用の反復子を返す．
  iterator2
  iter() const
  {
    return iterator2(_network(), begin_iter(), end_iter());
  }

  /// @brief 要素を末尾に追加する．
  void
  push_back(
    const TpgNode& node
  )
  {
    put_id(node, node.id());
  }

};

END_NAMESPACE_DRUID

#endif // TPGNODELIST_H
