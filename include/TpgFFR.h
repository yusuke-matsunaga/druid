#ifndef TPGFFR_H
#define TPGFFR_H

/// @file TpgFFR.h
/// @brief TpgFFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class TpgNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgFFR TpgFFR.h "TpgFFR.h"
/// @brief TpgNetwork の FFR の情報を表すクラス
/// @sa TpgNetwork
/// @sa TpgNode
/// @sa TpgFault
/// @sa TpgMFFC
///
/// 具体的には以下の情報を持つ．
/// - FFR の根のノード
/// - FFR に含まれる代表故障のリスト
/// 一度設定された不変のオブジェクトとなる．
//////////////////////////////////////////////////////////////////////
class TpgFFR
{
public:

  /// @brief コンストラクタ
  TpgFFR() = default;

  /// @brief コンストラクタ
  TpgFFR(
    const TpgNetworkImpl* network, ///< [in] ネットワーク
    SizeType id                    ///< [in] ID番号
  ) : mNetwork{network},
      mId{id}
  {
  }

  /// @brief デストラクタ
  ~TpgFFR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 根のノードを返す．
  const TpgNode*
  root() const;

  /// @brief 葉(FFRの入力)の数を返す．
  SizeType
  input_num() const
  {
    return input_list().size();
  }

  /// @brief 葉(FFRの入力)を返す．
  const TpgNode*
  input(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < input_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < input_num() );

    return input_list()[pos];
  }

  /// @brief 葉(FFRの入力)のリストを返す．
  const vector<const TpgNode*>&
  input_list() const;

  /// @brief このFFRに含まれる代表故障の数を返す．
  SizeType
  fault_num() const
  {
    return fault_list().size();
  }

  /// @brief このFFRに含まれる代表故障を返す．
  const TpgFault*
  fault(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < fault_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < fault_num() );

    return fault_list()[pos];
  }

  /// @brief このFFRに含まれる代表故障のリストを返す．
  const vector<const TpgFault*>&
  fault_list() const;

  /// @brief 等価比較演算子
  bool
  operator==(
    const TpgFFR& right
  ) const
  {
    return mNetwork == right.mNetwork && mId == right.mId;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const TpgFFR& right
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

#endif // TPGFFR_H
