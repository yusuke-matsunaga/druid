#ifndef TPGGATELIST_H
#define TPGGATELIST_H

/// @file TpgGateList.h
/// @brief TpgGateList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgListBase.h"
#include "types/TpgGate.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgGateIter TpgGateList.h "TpgGateList.h"
/// @brief TpgGateList の反復子
/// @ingroup TypesGroup
/// @sa TpgGate, TpgGateList
//////////////////////////////////////////////////////////////////////
class TpgGateIter :
  public TpgIterBase
{
public:

  /// @brief 空のコンストラクタ
  TpgGateIter() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgGateIter(
    const std::shared_ptr<NetworkRep>& network,
    std::vector<SizeType>::const_iterator iter
  ) : TpgIterBase(network, iter)
  {
  }

  /// @brief デストラクタ
  ~TpgGateIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  TpgGate
  operator*() const
  {
    return TpgBase::gate(get_id());
  }

};


//////////////////////////////////////////////////////////////////////
/// @class TpgGateIter2 TpgGateList.h "TpgGateList.h"
/// @brief TpgGateList の反復子2(Python用)
/// @ingroup TypesGroup
/// @sa TpgGate, TpgGateList
//////////////////////////////////////////////////////////////////////
class TpgGateIter2:
  public TpgIter2Base
{
public:

  /// @brief 空のコンストラクタ
  TpgGateIter2() = default;

  /// @brief 値を指定したコンストラクタ
  TpgGateIter2(
    const std::shared_ptr<NetworkRep>& network,
    IdIter cur,
    IdIter end
  ) : TpgIter2Base(network, cur, end)
  {
  }

  /// @brief デストラクタ
  ~TpgGateIter2() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 次の要素を返す．
  ///
  /// has_next() == true と仮定している．
  TpgGate
  next()
  {
    auto id = next_id();
    return TpgBase::gate(id);
  }

};


//////////////////////////////////////////////////////////////////////
/// @class TpgGateList TpgGateList.h "TpgGateList.h"
/// @brief TpgGate のリスト
/// @ingroup TypesGroup
/// @sa TpgGate
//////////////////////////////////////////////////////////////////////
class TpgGateList :
  public TpgListBase
{
public:

  using iterator = TpgGateIter;
  using iterator2 = TpgGateIter2;

public:

  /// @brief 空のコンストラクタ
  TpgGateList() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgGateList(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    const std::vector<SizeType>& id_list        ///< [in] ノード番号のリスト
    = {}
  ) : TpgListBase(network, id_list)
  {
  }

  /// @brief デストラクタ
  ~TpgGateList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素を返す．
  TpgGate
  operator[](
    SizeType index ///< [in] インデックス （ 0 <= index < size() );
  ) const
  {
    return TpgBase::gate(get_id(index));
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
    const TpgGate& gate
  )
  {
    put_id(gate, gate.id());
  }

};

END_NAMESPACE_DRUID

#endif // TPGGATELIST_H
