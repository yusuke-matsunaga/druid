#ifndef TPGLOGICNAND_H
#define TPGLOGICNAND_H

/// @file TpgLogicNAND.h
/// @brief TpgLogicNAND[x] のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogic.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicNAND2 TpgLogicNAND.h "TpgLogicNAND.h"
/// @brief 2入力NANDを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicNAND2 :
  public TpgLogic2
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin_list ファンインのリスト
  ///
  /// fanin_list.size() == 2 であることを仮定している．
  TpgLogicNAND2(int id,
		const vector<TpgNode*>& fanin_list);

  /// @brief デストラクタ
  virtual
  ~TpgLogicNAND2();


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


//////////////////////////////////////////////////////////////////////
/// @class TpgLogicNAND3 TpgLogicNAND.h "TpgLogicNAND.h"
/// @brief 3入力NANDを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicNAND3 :
  public TpgLogic3
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin_list ファンインのリスト
  ///
  /// fanin_list.size() == 3 であることを仮定している．
  TpgLogicNAND3(int id,
		const vector<TpgNode*>& fanin_list);

  /// @brief デストラクタ
  virtual
  ~TpgLogicNAND3();


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


//////////////////////////////////////////////////////////////////////
/// @class TpgLogicNAND4 TpgLogicNAND.h "TpgLogicNAND.h"
/// @brief 4入力NANDを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicNAND4 :
  public TpgLogic4
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin_list ファンインのリスト
  ///
  /// fanin_list.size() == 4 であることを仮定している．
  TpgLogicNAND4(int id,
		const vector<TpgNode*>& fanin_list);

  /// @brief デストラクタ
  virtual
  ~TpgLogicNAND4();


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


//////////////////////////////////////////////////////////////////////
/// @class TpgLogicNANDN TpgLogicNAND.h "TpgLogicNAND.h"
/// @brief N入力NANDを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicNANDN :
  public TpgLogicN
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  TpgLogicNANDN(int id);

  /// @brief デストラクタ
  virtual
  ~TpgLogicNANDN();


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

END_NAMESPACE_DRUID

#endif // TPGLOGICNAND_H
