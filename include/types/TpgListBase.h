#ifndef TPGLISTBASE_H
#define TPGLISTBASE_H

/// @file TpgListBase.h
/// @brief TpgListBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgBase.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgIterBase TpgListBase.h "TpgListBase.h"
/// @brief TpgListBase の反復子
/// @ingroup TypesGroup
//////////////////////////////////////////////////////////////////////
class TpgIterBase :
  public TpgBase
{
public:

  using IdList = std::vector<SizeType>;
  using IdIter = IdList::const_iterator;

public:

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgIterBase() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgIterBase(
    const std::shared_ptr<NetworkRep>& network,
    IdIter iter
  ) : TpgBase(network),
      mIdIter{iter}
  {
  }

  /// @brief デストラクタ
  ~TpgIterBase() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  //// @brief 次の要素に進む．
  void
  operator++()
  {
    ++ mIdIter;
  }

  /// @brief 等価比較演算子
  bool
  operator==(
    const TpgIterBase& right
  ) const
  {
    return TpgBase::operator==(right) && mIdIter == right.mIdIter;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const TpgIterBase& right
  ) const
  {
    return !operator==(right);
  }


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief IDの値を得る．
  SizeType
  get_id() const
  {
    return *mIdIter;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // mIdList の反復子
  IdIter mIdIter;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgIter2Base TpgListBase.h "TpgListBase.h"
/// @brief TpgListBase の反復子
/// @ingroup TypesGroup
//////////////////////////////////////////////////////////////////////
class TpgIter2Base :
  public TpgBase
{
public:

  using IdList = std::vector<SizeType>;
  using IdIter = IdList::const_iterator;

public:

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgIter2Base() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgIter2Base(
    const std::shared_ptr<NetworkRep>& network,
    IdIter cur,
    IdIter end
  ) : TpgBase(network),
      mCurIter{cur},
      mEndIter{end}
  {
  }

  /// @brief デストラクタ
  ~TpgIter2Base() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 有効な値を持っているか調べる．
  bool
  has_next() const
  {
    return mCurIter != mEndIter;
  }

  /// @brief 次の要素番号を返す．
  ///
  /// has_next() == true と仮定している．
  SizeType
  next_id()
  {
    auto id = *mCurIter;
    ++ mCurIter;
    return id;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 現在の反復子
  IdIter mCurIter;

  // 末尾の反復子
  IdIter mEndIter;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgListBase TpgListBase.h "TpgListBase.h"
/// @brief TpgBase のリスト
/// @ingroup TypesGroup
//////////////////////////////////////////////////////////////////////
class TpgListBase :
  public TpgBase
{
public:

  using IdList = std::vector<SizeType>;
  using IdIter = IdList::const_iterator;

public:

  /// @brief 空のコンストラクタ
  TpgListBase() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgListBase(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    const IdList& id_list = {}                  ///< [in] ノード番号のリスト
  ) : TpgBase(network),
      mIdList{id_list}
  {
  }

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgListBase(
    const TpgBase& base,                ///< [in] 親のネットワーク
    const IdList& id_list = {}          ///< [in] ノード番号のリスト
  ) : TpgBase(base),
      mIdList{id_list}
  {
  }

  /// @brief デストラクタ
  ~TpgListBase() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 空の時 true を返す．
  bool
  empty() const
  {
    return mIdList.empty();
  }

  /// @brief 要素数を返す．
  SizeType
  size() const
  {
    return mIdList.size();
  }

  /// @brief ID番号のリストを返す．
  const IdList&
  id_list() const
  {
    return mIdList;
  }

  /// @brief 空リストにする．
  void
  clear()
  {
    return mIdList.clear();
  }

  /// @brief 容量を確保する．
  void
  reserve(
    SizeType size
  )
  {
    mIdList.reserve(size);
  }


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素のID番号を取り出す．
  SizeType
  get_id(
    SizeType index ///< [in] インデックス ( 0 <= index < size() )
  ) const
  {
    if ( index >= size() ) {
      throw std::out_of_range{"index is out of range"};
    }
    return mIdList[index];
  }

  /// @brief mIdList の先頭の反復子を返す．
  IdIter
  begin_iter() const
  {
    return mIdList.begin();
  }

  /// @brief mIdList の末尾の反復子を返す．
  IdIter
  end_iter() const
  {
    return mIdList.end();
  }

  /// @brief ID を末尾に追加する．
  void
  put_id(
    const TpgBase& base,
    SizeType id
  )
  {
    if ( is_valid() ) {
      if ( !TpgBase::operator==(base) ) {
	throw std::invalid_argument{"network mismatch"};
      }
    }
    else {
      *(static_cast<TpgBase*>(this)) = base;
    }
    mIdList.push_back(id);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号のリスト
  std::vector<SizeType> mIdList;

};

END_NAMESPACE_DRUID

#endif // TPGLISTBASE_H
