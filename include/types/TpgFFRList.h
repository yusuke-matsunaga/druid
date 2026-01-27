#ifndef TPGFFRLIST_H
#define TPGFFRLIST_H

/// @file TpgFFRList.h
/// @brief TpgFFRList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgListBase.h"
#include "types/TpgFFR.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgFFRIter TpgFFRList.h "TpgFFRList.h"
/// @brief TpgFFRList の反復子
//////////////////////////////////////////////////////////////////////
class TpgFFRIter :
  public TpgIterBase
{
public:

  /// @brief 空のコンストラクタ
  TpgFFRIter() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgFFRIter(
    const std::shared_ptr<NetworkRep>& network,
    std::vector<SizeType>::const_iterator iter
  ) : TpgIterBase(network, iter)
  {
  }

  /// @brief デストラクタ
  ~TpgFFRIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  TpgFFR
  operator*() const
  {
    return TpgBase::ffr(get_id());
  }

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFFRList TpgFFRList.h "TpgFFRList.h"
/// @brief TpgFFR のリスト
//////////////////////////////////////////////////////////////////////
class TpgFFRList :
  public TpgListBase
{
public:

  using iterator = TpgFFRIter;

public:

  /// @brief 空のコンストラクタ
  TpgFFRList() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgFFRList(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    const std::vector<SizeType>& id_list        ///< [in] ノード番号のリスト
    = {}
  ) : TpgListBase(network, id_list)
  {
  }

  /// @brief デストラクタ
  ~TpgFFRList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素を返す．
  TpgFFR
  operator[](
    SizeType index ///< [in] インデックス （ 0 <= index < size() );
  ) const
  {
    return TpgBase::ffr(get_id(index));
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
    const TpgFFR& ffr
  )
  {
    put_id(ffr, ffr.id());
  }

};

END_NAMESPACE_DRUID

#endif // TPGFFRLIST_H
