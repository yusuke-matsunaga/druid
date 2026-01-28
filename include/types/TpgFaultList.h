#ifndef TPGFAULTLIST_H
#define TPGFAULTLIST_H

/// @file TpgFaultList.h
/// @brief TpgFaultList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgListBase.h"
#include "types/TpgFault.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgFaultIter TpgFaultList.h "TpgFaultList.h"
/// @brief TpgFaultList の反復子
/// @ingroup TypesGroup
/// @sa TpgFault, TpgFaultList
//////////////////////////////////////////////////////////////////////
class TpgFaultIter:
  public TpgIterBase
{
public:

  /// @brief 空のコンストラクタ
  TpgFaultIter() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgFaultIter(
    const std::shared_ptr<NetworkRep>& network,
    std::vector<SizeType>::const_iterator iter
  ) : TpgIterBase(network, iter)
  {
  }

  /// @brief デストラクタ
  ~TpgFaultIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  TpgFault
  operator*() const
  {
    return TpgBase::fault(get_id());
  }

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFaultList TpgFaultList.h "TpgFaultList.h"
/// @brief 故障のリストを表すクラス
/// @ingroup TypesGroup
/// @sa TpgFault, TpgFaultIter
///
/// NetworkRep への共有ポインタと故障番号のリストを持つ．
//////////////////////////////////////////////////////////////////////
class TpgFaultList:
  public TpgListBase
{
public:

  using iterator = TpgFaultIter;

public:

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgFaultList() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgFaultList(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    const std::vector<SizeType>& id_list        ///< [in] 故障番号のリスト
    = {}
  ) : TpgListBase(network, id_list)
  {
  }

  /// @brief vector<TpgFault> からの変換コンストラクタ
  explicit
  TpgFaultList(
    const std::vector<TpgFault>& fault_list
  )
  {
    reserve(fault_list.size());
    for ( auto& fault: fault_list ) {
      push_back(fault);
    }
  }

  /// @brief デストラクタ
  ~TpgFaultList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素を返す．
  TpgFault
  operator[](
    SizeType index ///< [in] インデックス （ 0 <= index < size() );
  ) const
  {
    return TpgBase::fault(get_id(index));
  }

  /// @brief 最大の故障番号を返す．
  SizeType
  max_fid() const
  {
    SizeType max_val = 0;
    for ( auto id: id_list() ) {
      max_val = std::max(max_val, id);
    }
    return max_val;
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
    const TpgFault& fault
  )
  {
    put_id(fault, fault.id());
  }

};

END_NAMESPACE_DRUID

#endif // TPGFAULTLIST_H
