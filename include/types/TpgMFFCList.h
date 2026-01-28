#ifndef TPGMFFCLIST_H
#define TPGMFFCLIST_H

/// @file TpgMFFCList.h
/// @brief TpgMFFCList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgListBase.h"
#include "types/TpgMFFC.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgMFFCIter TpgMFFCList.h "TpgMFFCList.h"
/// @brief TpgMFFCList の反復子
/// @ingroup TypesGroup
//////////////////////////////////////////////////////////////////////
class TpgMFFCIter :
  public TpgIterBase
{
public:

  /// @brief 空のコンストラクタ
  TpgMFFCIter() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgMFFCIter(
    const std::shared_ptr<NetworkRep>& network,
    std::vector<SizeType>::const_iterator iter
  ) : TpgIterBase(network, iter)
  {
  }

  /// @brief デストラクタ
  ~TpgMFFCIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  TpgMFFC
  operator*() const
  {
    return TpgBase::mffc(get_id());
  }

};


//////////////////////////////////////////////////////////////////////
/// @class TpgMFFCList TpgMFFCList.h "TpgMFFCList.h"
/// @brief TpgMFFC のリスト
/// @ingroup TypesGroup
//////////////////////////////////////////////////////////////////////
class TpgMFFCList :
  public TpgListBase
{
public:

  using iterator = TpgMFFCIter;

public:

  /// @brief 空のコンストラクタ
  TpgMFFCList() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgMFFCList(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    const std::vector<SizeType>& id_list        ///< [in] ノード番号のリスト
    = {}
  ) : TpgListBase(network, id_list)
  {
  }

  /// @brief デストラクタ
  ~TpgMFFCList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素を返す．
  TpgMFFC
  operator[](
    SizeType index ///< [in] インデックス （ 0 <= index < size() );
  ) const
  {
    return TpgBase::mffc(get_id(index));
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
    const TpgMFFC& mffc
  )
  {
    put_id(mffc, mffc.id());
  }

};

END_NAMESPACE_DRUID

#endif // TPGMFFCLIST_H
