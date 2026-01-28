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
/// @class TpgGateList TpgGateList.h "TpgGateList.h"
/// @brief TpgGate のリスト
/// @ingroup TypesGroup
//////////////////////////////////////////////////////////////////////
class TpgGateList :
  public TpgListBase
{
public:

  using iterator = TpgGateIter;

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
