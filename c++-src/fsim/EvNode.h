#ifndef FSIM_EVNODE_H
#define FSIM_EVNODE_H

/// @file EvNode.h
/// @brief EvNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "PackedVal.h"
#include "PackedVal3.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class EvNode EvNode.h "EvNode.h"
/// @brief 故障シミュレーション用のノード
///
/// 出力値の計算はゲートの種類によって異なるので仮想関数にしている．<br>
/// 注意が必要なのがファンアウトの情報．最初のファンアウトだけ個別のポインタで
/// 持ち，２番目以降のファンアウトは配列で保持する．これは多くのノードが
/// 一つしかファンアウトを持たず，その場合に配列を使うとメモリ参照が余分に発生する
/// ため．
//////////////////////////////////////////////////////////////////////
class EvNode
{
  friend class EventQ;

protected:

  /// @brief コンストラクタ
  EvNode(
    const SimNode* sim_node ///< [in] もとの SimNode
  ) : mSimNode{sim_node}
  {
  }


public:

  /// @brief コピーコンストラクタは禁止
  EvNode(const EvNode& src) = delete;

  /// @brief 代入演算子も禁止
  const EvNode&
  operator=(const EvNode& src) = delete;

  /// @brief デストラクタ
  ~EvNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 構造に関する情報の取得
  //////////////////////////////////////////////////////////////////////

  /// @brief もとの SimNode を返す．
  const SimNode*
  sim_node() const
  {
    return mSimNode;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーションに関する情報の取得/設定
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値を得る．
  FSIM_VALTYPE
  val() const
  {
    return mVal;
  }

  /// @brief 出力値のセットを行う．
  void
  set_val(
    FSIM_VALTYPE val ///< [in] 値
  )
  {
    mVal = val;
  }

  /// @brief 出力値のセットを行う(マスク付き)．
  void
  set_val(
    FSIM_VALTYPE val, ///< [in] 値
    PackedVal mask    ///< [in] マスク
  )
  {
#if FSIM_VAL2
    mVal &= ~mask;
    mVal |= (val & mask);
#elif FSIM_VAL3
    mVal.set_with_mask(val, mask);
#endif
  }

  /// @brief 出力値を計算する．
  void
  calc_val()
  {
    set_val(_calc_val());
  }

  /// @brief 出力値を計算する(マスク付き)．
  ///
  /// mask で1の立っているビットだけ更新する．
  void
  calc_val(
    PackedVal mask ///< [in] マスク
  )
  {
    set_val(_calc_val(), mask);
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  /// @return 計算結果を返す．
  FSIM_VALTYPE
  _calc_val();


private:
  //////////////////////////////////////////////////////////////////////
  // EventQ 用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief レベルを返す．
  SizeType
  level() const
  {
    return mSimNode->level();
  }

  /// @brief キューに積まれていたら true を返す．
  bool
  in_queue() const
  {
    return mFlags.test(IN_Q);
  }

  /// @brief キューフラグをセットする．
  void
  set_queue()
  {
    mFlags.set(IN_Q);
  }

  /// @brief キューフラグをクリアする．
  void
  clear_queue()
  {
    mFlags.reset(IN_Q);
  }

  /// @brief 反転マスクを持っていたら true を返す．
  bool
  has_flip_mask() const
  {
    return mFlags.test(FLIP);
  }

  /// @brief 反転マスクをセットする．
  void
  set_flip(
    PackedVal mask ///< [in] マスク値
  )
  {
    mFlags.set(FLIP);
    mFlipMask = mask;
  }

  /// @brief 反転フラグをクリアする．
  void
  clear_flip()
  {
    mFlags.reset(FLIP);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の SimNode
  const SimNode* mSimNode;

  // フラグのビット位置を表す定数
  static const int IN_Q = 0;
  static const int FLIP = 1;
  static const int NFLAGS = 2;

  // 種々のフラグ
  bitset<NFLAGS> mFlags{0};

  // 出力値
  FSIM_VALTYPE mVal;

  // 反転マスク
  PackedVal mFlipMask;

};

END_NAMESPACE_DRUID_FSIM

#endif // FSIM_EVNODE_H
