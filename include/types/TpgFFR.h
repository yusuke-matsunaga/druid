#ifndef TPGFFR_H
#define TPGFFR_H

/// @file TpgFFR.h
/// @brief TpgFFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgObjBase.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgFFR TpgFFR.h "TpgFFR.h"
/// @brief TpgNetwork の FFR の情報を表すクラス
/// @ingroup TypesGroup
/// @sa TpgNetwork
/// @sa TpgNode
/// @sa TpgMFFC
///
/// 具体的には以下の情報を持つ．
/// - FFR の根のノード
/// - FFR の葉のノードのリスト
/// - FFR に含まれる全ノードのリスト
/// - ffR に含まれる故障のリスト
/// 一度設定された不変のオブジェクトとなる．
///
/// FFR の葉のノードはそのFFRには含まれない．
//////////////////////////////////////////////////////////////////////
class TpgFFR:
  public TpgObjBase
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgFFR() = default;

  /// @brief コンストラクタ
  TpgFFR(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    SizeType id                                 ///< [in] ID番号
  ) : TpgObjBase(network, id)
  {
  }

  /// @brief デストラクタ
  ~TpgFFR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 根のノードを返す．
  TpgNode
  root() const;

  /// @brief 葉(FFRの入力)の数を返す．
  SizeType
  input_num() const;

  /// @brief 葉(FFRの入力)を返す．
  TpgNode
  input(
    SizeType index ///< [in] 位置番号 ( 0 <= index < input_num() )
  ) const;

  /// @brief 葉(FFRの入力)のリストを返す．
  TpgNodeList
  input_list() const;

  /// @brief このFFRに含まれるノード数を返す．
  SizeType
  node_num() const;

  /// @brief このFFRに含まれるノードを返す．
  TpgNode
  node(
    SizeType index ///< [in] 位置番号 ( 0 <= index < node_num() )
  ) const;

  /// @brief このFFRに含まれるノードのリストを返す．
  TpgNodeList
  node_list() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief FFRRep に変換する．
  const FFRRep*
  _ffr() const
  {
    return TpgBase::_ffr(id());
  }

};

END_NAMESPACE_DRUID

#endif // TPGFFR_H
