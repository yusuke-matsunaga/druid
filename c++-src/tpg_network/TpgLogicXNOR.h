#ifndef TPGLOGICXNOR_H
#define TPGLOGICXNOR_H

/// @file TpgLogicXNOR.h
/// @brief TpgLogicXNOR[x] のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogic.h"


BEGIN_NAMESPACE_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicXNOR2 TpgLogicXNOR.h "TpgLogicXNOR.h"
/// @brief 2入力XNORを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicXNOR2 :
  public TpgLogic2
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin_list ファンインのリスト
  TpgLogicXNOR2(int id,
		const vector<TpgNode*>& fanin_list);

  /// @brief デストラクタ
  virtual
  ~TpgLogicXNOR2();


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

};

END_NAMESPACE_SATPG

#endif // TPGLOGICXNOR_H
