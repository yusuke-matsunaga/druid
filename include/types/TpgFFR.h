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
/// @sa TpgMFFC
///
/// 具体的には以下の情報を持つ．
/// - FFR の根のノード
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

  /// @brief ID番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

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

  /// @brief このFFRに含まれるノード数を返す．
  SizeType
  node_num() const
  {
    return node_list().size();
  }

  /// @brief このFFRに含まれるノードを返す．
  const TpgNode*
  node(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < node_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < node_num() );

    return node_list()[pos];
  }

  /// @brief このFFRに含まれるノードのリストを返す．
  const vector<const TpgNode*>&
  node_list() const;

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
