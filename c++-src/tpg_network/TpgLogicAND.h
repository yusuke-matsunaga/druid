#ifndef TPGLOGICAND_H
#define TPGLOGICAND_H

/// @file TpgLogicAND.h
/// @brief TpgLogicAND[x] のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogic.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicAND2 TpgLogicAND.h "TpgLogicAND.h"
/// @brief 2入力ANDを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicAND2 :
  public TpgLogic2
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin_list ファンインのリスト
  ///
  /// fanin_list.size() == 2 であることを仮定している．
  TpgLogicAND2(int id,
	       const vector<TpgNode*>& fanin_list);

  /// @brief デストラクタ
  virtual
  ~TpgLogicAND2();


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
/// @class TpgLogicAND3 TpgLogicAND.h "TpgLogicAND.h"
/// @brief 3入力ANDを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicAND3 :
  public TpgLogic3
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin_list ファンインのリスト
  ///
  /// fanin_list.size() == 3 であることを仮定している．
  TpgLogicAND3(int id,
	       const vector<TpgNode*>& fanin_list);

  /// @brief デストラクタ
  virtual
  ~TpgLogicAND3();


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
/// @class TpgLogicAND4 TpgLogicAND.h "TpgLogicAND.h"
/// @brief 4入力ANDを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicAND4 :
  public TpgLogic4
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin_list ファンインのリスト
  ///
  /// fanin_list.size() == 4 であることを仮定している．
  TpgLogicAND4(int id,
	       const vector<TpgNode*>& fanin_list);

  /// @brief デストラクタ
  virtual
  ~TpgLogicAND4();


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
/// @class TpgLogicAND TpgLogicAND.h "TpgLogicAND.h"
/// @brief N入力ANDを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicANDN :
  public TpgLogicN
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  TpgLogicANDN(int id);

  /// @brief デストラクタ
  virtual
  ~TpgLogicANDN();


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

#endif // TPGLOGICAND_H
