#ifndef TPGOBJBASE_H
#define TPGOBJBASE_H

/// @file TpgObjBase.h
/// @brief TpgObjBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgBase.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgObjBase TpgObjBase.h "TpgObjBase.h"
/// @brief TpgNetwork の要素を表す基底クラス
/// @ingroup TypesGroup
///
/// TpgBase の継承クラスであり ID 番号を持つ．
/// それが何のID番号なのかは継承クラスで決める．
//////////////////////////////////////////////////////////////////////
class TpgObjBase:
  public TpgBase
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgObjBase() = default;

  /// @brief コンストラクタ
  TpgObjBase(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    SizeType id                                 ///< [in] ID番号
  ) : TpgBase(network),
      mId{id}
  {
  }

  /// @brief コンストラクタ
  TpgObjBase(
    const TpgBase& base, ///< [in] 親のネットワーク
    SizeType id          ///< [in] ID番号
  ) : TpgBase(base),
      mId{id}
  {
  }

  /// @brief デストラクタ
  ~TpgObjBase() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const
  {
    _check_valid();
    return mId;
  }

  /// @brief 等価比較演算子
  bool
  operator==(
    const TpgObjBase& right
  ) const
  {
    return TpgBase::operator==(right) && mId == right.mId;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const TpgObjBase& right
  ) const
  {
    return !operator==(right);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

};

END_NAMESPACE_DRUID

#endif // TPGOBJBASE_H
