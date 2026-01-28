#ifndef TPGMFFC_H
#define TPGMFFC_H

/// @file TpgMFFC.h
/// @brief TpgMFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgObjBase.h"
#include "types/TpgFFR.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgMFFC TpgMFFC.h "TpgMFFC.h"
/// @brief TpgNetwork の MFFC(Maximal Fanout Free Cone) の情報を表すクラス
/// @ingroup TypesGroup
/// @sa TpgNetwork
/// @sa TpgNode
/// @sa TpgFFR
///
/// 具体的には以下の情報を持つ．
/// - MFFC の根のノード
/// - MFFC に含まれる FFR のリスト
/// - MFFC に含まれる故障のリスト
/// 一度設定されたら不変のオブジェクトとなる．
//////////////////////////////////////////////////////////////////////
class TpgMFFC:
  public TpgObjBase
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgMFFC() = default;

  /// @brief 値を指定したコンストラクタ
  TpgMFFC(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    SizeType id                                 ///< [in] ID番号
  ) : TpgObjBase(network, id)
  {
  }

  /// @brief デストラクタ
  ~TpgMFFC() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 根のノードを返す．
  TpgNode
  root() const;

  /// @brief このMFFCに含まれるFFR数を返す．
  SizeType
  ffr_num() const;

  /// @brief このMFFCに含まれるFFRを返す．
  TpgFFR
  ffr(
    SizeType index /// [in] 位置番号 ( 0 <= index < ffr_num() )
  ) const;

  /// @brief このMFFCに含まれるFFRのリストを返す．
  TpgFFRList
  ffr_list() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief MFFCRep のポインタを取り出す．
  const MFFCRep*
  _mffc() const
  {
    return TpgBase::_mffc(id());
  }

};

END_NAMESPACE_DRUID

#endif // TPGMFFC_H
