#ifndef TPGMFFC_H
#define TPGMFFC_H

/// @file TpgMFFC.h
/// @brief TpgMFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgFFR.h"


BEGIN_NAMESPACE_DRUID

class TpgNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgMFFC TpgMFFC.h "TpgMFFC.h"
/// @brief TpgNetwork の MFFC(Maximal Fanout Free Cone) の情報を表すクラス
/// @sa TpgNetwork
/// @sa TpgNode
/// @sa TpgFFR
///
/// 具体的には以下の情報を持つ．
/// - MFFC の根のノード
/// - MFFC に含まれる FFR のリスト
/// 一度設定されたら不変のオブジェクトとなる．
//////////////////////////////////////////////////////////////////////
class TpgMFFC
{
public:

  /// @brief コンストラクタ
  TpgMFFC() = default;

  /// @brief コンストラクタ
  TpgMFFC(
    const TpgNetworkImpl* network, ///< [in] ネットワーク
    SizeType id                    ///< [in] ID番号
  ) : mNetwork{network},
      mId{id}
  {
  }

  /// @brief デストラクタ
  ~TpgMFFC() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 根のノードを返す．
  const TpgNode*
  root() const;

  /// @brief このMFFCに含まれるFFR数を返す．
  SizeType
  ffr_num() const
  {
    return ffr_list().size();
  }

  /// @brief このMFFCに含まれるFFRを返す．
  TpgFFR
  ffr(
    SizeType pos /// [in] 位置番号 ( 0 <= pos < ffr_num() )
  ) const
  {
    ASSERT_COND( pos < ffr_num() );

    return ffr_list()[pos];
  }

  /// @brief このMFFCに含まれるFFRのリストを返す．
  const vector<TpgFFR>&
  ffr_list() const;

  /// @brief 等価比較演算子
  bool
  operator==(
    const TpgMFFC& right
  ) const
  {
    return mNetwork == right.mNetwork && mId == right.mId;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const TpgMFFC& right
  ) const
  {
    return !operator==(right);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  const TpgNetworkImpl* mNetwork{nullptr};

  // ID番号
  SizeType mId;

};

END_NAMESPACE_DRUID

#endif // TPGMFFC_H
