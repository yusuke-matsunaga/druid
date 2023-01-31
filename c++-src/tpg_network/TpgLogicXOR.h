#ifndef TPGLOGICXOR_H
#define TPGLOGICXOR_H

/// @file TpgLogicXOR.h
/// @brief TpgLogicXOR[x] のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgLogic.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicXOR2 TpgLogicXOR.h "TpgLogicXOR.h"
/// @brief 2入力XORを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicXOR2 :
  public TpgLogic
{
  friend class TpgNetworkImpl;

private:

  /// @brief コンストラクタ
  ///
  /// fanin_list.size() == 2 であることを仮定している．
  TpgLogicXOR2(
    const vector<const TpgNode*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief デストラクタ
  ~TpgLogicXOR2() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  GateType
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

};

END_NAMESPACE_DRUID

#endif // TPGLOGICXOR_H
