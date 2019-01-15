#ifndef TPGLOGICNOT_H
#define TPGLOGICNOT_H

/// @file TpgLogicNOT.h
/// @brief TpgLogicNOT のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogic.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicNOT TpgLogicNOT.h "TpgLogicNOT.h"
/// @brief inverter を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicNOT :
  public TpgLogic1
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin ファンイン
  TpgLogicNOT(int id,
	      TpgNode* fanin);

  /// @brief デストラクタ
  virtual
  ~TpgLogicNOT();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  virtual
  GateType
  gate_type() const override;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  virtual
  Val3
  cval() const override;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  virtual
  Val3
  nval() const override;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  virtual
  Val3
  coval() const override;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  virtual
  Val3
  noval() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////


};

END_NAMESPACE_DRUID

#endif // TPGLOGICNOT_H
