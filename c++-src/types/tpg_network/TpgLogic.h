#ifndef TPGLOGIC_H
#define TPGLOGIC_H

/// @file TpgLogic.h
/// @brief TpgLogic のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgLogic TpgLogic.h "TpgLogic.h"
/// @brief 論理ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogic :
  public TpgNode
{
protected:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  TpgLogic(
    const vector<const TpgNode*>& fanin_list ///< [in] ファンインリスト
  );

  /// @brief デストラクタ
  ~TpgLogic() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief logic タイプの時 true を返す．
  bool
  is_logic() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgLogicC0 TpgLogicC0.h "TpgLogicC0.h"
/// @brief constant-0 を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicC0 :
  public TpgLogic
{
public:

  /// @brief コンストラクタ
  TpgLogicC0();

  /// @brief デストラクタ
  ~TpgLogicC0() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgLogicC1 TpgLogicC1.h "TpgLogicC1.h"
/// @brief constant-1 を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicC1 :
  public TpgLogic
{
public:

  /// @brief コンストラクタ
  TpgLogicC1();

  /// @brief デストラクタ
  ~TpgLogicC1() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgLogicBUFF TpgLogicBUFF.h "TpgLogicBUFF.h"
/// @brief buffer を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicBUFF :
  public TpgLogic
{
public:

  /// @brief コンストラクタ
  TpgLogicBUFF(
    const TpgNode* fanin /// @param[in] ファンイン
  );

  /// @brief デストラクタ
  ~TpgLogicBUFF() = default;


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


//////////////////////////////////////////////////////////////////////
/// @class TpgLogicNOT TpgLogicNOT.h "TpgLogicNOT.h"
/// @brief inverter を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicNOT :
  public TpgLogic
{
public:

  /// @brief コンストラクタ
  TpgLogicNOT(
    const TpgNode* fanin ///< [in] ファンイン
  );

  /// @brief デストラクタ
  ~TpgLogicNOT() = default;


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

};


//////////////////////////////////////////////////////////////////////
/// @class TpgLogicNAND TpgLogicNAND.h "TpgLogicNAND.h"
/// @brief NANDゲートを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicNAND :
  public TpgLogicAND
{
public:

  /// @brief コンストラクタ
  TpgLogicNAND(
    const vector<const TpgNode*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief デストラクタ
  ~TpgLogicNAND() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

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


//////////////////////////////////////////////////////////////////////
/// @class TpgLogicOR TpgLogicOR.h "TpgLogicOR.h"
/// @brief ORゲートを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicOR :
  public TpgLogic
{
public:

  /// @brief コンストラクタ
  TpgLogicOR(
    const vector<const TpgNode*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief デストラクタ
  ~TpgLogicOR() = default;


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


//////////////////////////////////////////////////////////////////////
/// @class TpgLogicNOR TpgLogicNOR.h "TpgLogicNOR.h"
/// @brief NORゲートを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicNOR :
  public TpgLogicOR
{
public:

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


//////////////////////////////////////////////////////////////////////
/// @class TpgLogicXOR2 TpgLogicXOR.h "TpgLogicXOR.h"
/// @brief XORを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicXOR :
  public TpgLogic
{
public:

  /// @brief コンストラクタ
  TpgLogicXOR(
    const vector<const TpgNode*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief デストラクタ
  ~TpgLogicXOR() = default;


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

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicXNOR2 TpgLogicXNOR.h "TpgLogicXNOR.h"
/// @brief XNORを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicXNOR :
  public TpgLogicXOR
{
public:

  /// @brief コンストラクタ
  TpgLogicXNOR(
    const vector<const TpgNode*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief デストラクタ
  ~TpgLogicXNOR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

};

END_NAMESPACE_DRUID

#endif // TPGLOGIC_H
