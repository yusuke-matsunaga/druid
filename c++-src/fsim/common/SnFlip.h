#ifndef SNFLIP_H
#define SNFLIP_H

/// @file SnFlip.h
/// @brief SnFlip のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SnGate.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SnFlip SnFlip.h "SnFlip.h"
/// @brief 反転イベントを表すノード
//////////////////////////////////////////////////////////////////////
class SnFlip :
  public SnGate1
{
public:

  /// @brief コンストラクタ
  SnFlip(
    SizeType id,
    SimNode* orig_node ///< [in] 元のノード
  ) : SnGate1(id, {orig_node})
  {
  }

  /// @brief デストラクタ
  ~SnFlip();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを返す．
  PrimType
  gate_type() const override;

  /// @brief 反転マスクをセットする．
  void
  set_flip_mask(
    PackedVal flip_mask ///< [in] 反転マスク
  )
  {
    mFlipMask = flip_mask;
  }

  /// @brief 内容をダンプする．
  void
  dump(
    std::ostream& s
  ) const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  /// @return 計算結果を返す．
  FSIM_VALTYPE
  _calc_val() const override;

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  PackedVal
  _calc_gobs(
    SizeType ipos ///< [in] 入力番号
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 反転マスク
  PackedVal mFlipMask{PV_ALL0};

};

END_NAMESPACE_DRUID_FSIM

#endif // SNFLIP_H
