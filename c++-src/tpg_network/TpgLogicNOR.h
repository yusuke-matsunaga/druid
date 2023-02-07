#ifndef TPGLOGICNOR_H
#define TPGLOGICNOR_H

/// @file TpgLogicNOR.h
/// @brief TpgLogicNOR[x] のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgLogic.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicNOR TpgLogicNOR.h "TpgLogicNOR.h"
/// @brief NORゲートを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicNOR :
  public TpgLogic
{
  friend class TpgNetworkImpl;

private:

  /// @brief コンストラクタ
  TpgLogicNOR(
    const vector<const TpgNode*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief デストラクタ
  ~TpgLogicNOR() = default;


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

};

END_NAMESPACE_DRUID

#endif // TPGLOGICNOR_H
