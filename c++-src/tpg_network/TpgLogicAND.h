#ifndef TPGLOGICAND_H
#define TPGLOGICAND_H

/// @file TpgLogicAND.h
/// @brief TpgLogicAND[x] のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgLogic.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicAND TpgLogicAND.h "TpgLogicAND.h"
/// @brief ANDゲートを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicAND :
  public TpgLogic
{
public:

  /// @brief コンストラクタ
  TpgLogicAND(
    const vector<const TpgNode*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief デストラクタ
  ~TpgLogicAND() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  cval() const override;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  nval() const override;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  coval() const override;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  noval() const override;

  /// @brief side-input の値を得る．
  Val3
  side_val() const override;

};

END_NAMESPACE_DRUID

#endif // TPGLOGICAND_H
