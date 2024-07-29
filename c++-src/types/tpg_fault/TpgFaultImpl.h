#ifndef TPGFAULTIMPL_H
#define TPGFAULTIMPL_H

/// @file TpgFaultImpl.h
/// @brief TpgFaultImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgFault_StemSa TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ステムの故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgFault_Stem :
  public TpgFault
{
public:

  /// @brief コンストラクタ
  TpgFault_Stem(
    SizeType id,        ///< [in] ID番号
    const TpgGate* gate ///< [in] 対象のゲート
  ) : TpgFault{id, gate}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_Stem() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬の起点となるノードを返す．
  const TpgNode*
  origin_node() const override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の内容を表す文字列の基本部分を返す．
  string
  str_base() const;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_StemSa TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ステムの縮退故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgFault_StemSa :
  public TpgFault_Stem
{
public:

  /// @brief コンnストラクタ
  TpgFault_StemSa(
    SizeType id,        ///< [in] ID番号
    const TpgGate* gate ///< [in] 対象のゲート
  ) : TpgFault_Stem{id, gate}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_StemSa() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_StemSa0 TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ステムの0縮退故障
//////////////////////////////////////////////////////////////////////
class TpgFault_StemSa0 :
  public TpgFault_StemSa
{
public:

  /// @brief コンストラクタ
  TpgFault_StemSa0(
    SizeType id,        ///< [in] ID番号
    const TpgGate* gate ///< [in] 対象のゲート
  ) : TpgFault_StemSa{id, gate}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_StemSa0() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  AssignList
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_StemSa1 TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ステムの1縮退故障
//////////////////////////////////////////////////////////////////////
class TpgFault_StemSa1 :
  public TpgFault_StemSa
{
public:

  /// @brief コンストラクタ
  TpgFault_StemSa1(
    SizeType id,        ///< [in] ID番号
    const TpgGate* gate ///< [in] 対象のゲート
  ) : TpgFault_StemSa{id, gate}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_StemSa1() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  AssignList
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_Branch TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ブランチの故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgFault_Branch :
  public TpgFault
{
public:

  /// @brief コンストラクタ
  TpgFault_Branch(
    SizeType id,         ///< [in] ID番号
    const TpgGate* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : TpgFault{id, gate},
      mIpos{ipos}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_Branch() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ステムの故障の時 true を返す．
  bool
  is_stem() const override;

  /// @brief ブランチの故障の時の入力位置を返す．
  SizeType
  branch_pos() const override;

  /// @brief 故障伝搬の起点となるノードを返す．
  const TpgNode*
  origin_node() const override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の伝搬条件を追加する．
  void
  add_gate_propagation_condition(
    AssignList& assign_list ///< [out] 条件を追加するオブジェクト
  ) const;

  /// @brief 入力位置を得る．
  SizeType
  ipos() const
  {
    return mIpos;
  }

  /// @brief 故障の内容を表す文字列の基本部分を返す．
  string
  str_base() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力位置
  SizeType mIpos;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_BranchSa TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief 入力の縮退故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgFault_BranchSa :
  public TpgFault_Branch
{
public:

  /// @brief コンストラクタ
  TpgFault_BranchSa(
    SizeType id,         ///< [in] ID番号
    const TpgGate* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : TpgFault_Branch{id, gate, ipos}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_BranchSa() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_BranchSa0 TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ブランチの0縮退故障
//////////////////////////////////////////////////////////////////////
class TpgFault_BranchSa0 :
  public TpgFault_BranchSa
{
public:

  /// @brief コンストラクタ
  TpgFault_BranchSa0(
    SizeType id,         ///< [in] ID番号
    const TpgGate* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : TpgFault_BranchSa{id, gate, ipos}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_BranchSa0() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  AssignList
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_BranchSa1 TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ブランチの1縮退故障
//////////////////////////////////////////////////////////////////////
class TpgFault_BranchSa1 :
  public TpgFault_BranchSa
{
public:

  /// @brief コンストラクタ
  TpgFault_BranchSa1(
    SizeType id,         ///< [in] ID番号
    const TpgGate* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : TpgFault_BranchSa{id, gate, ipos}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_BranchSa1() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  AssignList
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_StemTd TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief 出力の縮退故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgFault_StemTd :
  public TpgFault_Stem
{
public:

  /// @brief コンストラクタ
  TpgFault_StemTd(
    SizeType id,        ///< [in] ID番号
    const TpgGate* gate ///< [in] 対象のゲート
  ) : TpgFault_Stem{id, gate}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_StemTd() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_StemRise TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ステムの rise 遷移故障
//////////////////////////////////////////////////////////////////////
class TpgFault_StemRise :
  public TpgFault_StemTd
{
public:

  /// @brief コンストラクタ
  TpgFault_StemRise(
    SizeType id,        ///< [in] ID番号
    const TpgGate* gate ///< [in] 対象のゲート
  ) : TpgFault_StemTd{id, gate}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_StemRise() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  AssignList
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_StemFall TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ステムの fall 遷移故障
//////////////////////////////////////////////////////////////////////
class TpgFault_StemFall :
  public TpgFault_StemTd
{
public:

  /// @brief コンストラクタ
  TpgFault_StemFall(
    SizeType id,        ///< [in] ID番号
    const TpgGate* gate ///< [in] 対象のゲート
  ) : TpgFault_StemTd{id, gate}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_StemFall() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  AssignList
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_BranchTd TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief 入力の縮退故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgFault_BranchTd :
  public TpgFault_Branch
{
public:

  /// @brief コンストラクタ
  TpgFault_BranchTd(
    SizeType id,         ///< [in] ID番号
    const TpgGate* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : TpgFault_Branch{id, gate, ipos}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_BranchTd() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_BranchRise TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ブランチの rise 遷移故障
//////////////////////////////////////////////////////////////////////
class TpgFault_BranchRise :
  public TpgFault_BranchTd
{
public:

  /// @brief コンストラクタ
  TpgFault_BranchRise(
    SizeType id,         ///< [in] ID番号
    const TpgGate* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : TpgFault_BranchTd{id, gate, ipos}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_BranchRise() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  AssignList
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_BranchFall TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ブランチの rise 遷移故障
//////////////////////////////////////////////////////////////////////
class TpgFault_BranchFall :
  public TpgFault_BranchTd
{
public:

  /// @brief コンストラクタ
  TpgFault_BranchFall(
    SizeType id,         ///< [in] ID番号
    const TpgGate* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : TpgFault_BranchTd{id, gate, ipos}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_BranchFall() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  AssignList
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_Ex TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ゲート網羅故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgFault_Ex :
  public TpgFault
{
public:

  /// @brief コンストラクタ
  TpgFault_Ex(
    SizeType id,              ///< [in] ID番号
    const TpgGate* gate,      ///< [in] 対象のゲート
    const vector<bool>& ivals ///< [in] 入力の値のリスト
  ) : TpgFault{id, gate},
      mIvals{ivals}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_Ex() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const override;

  /// @brief 故障値を返す．
  ///
  /// 網羅故障の場合は意味を持たない．
  Fval2
  fval() const override;

  /// @brief 故障伝搬の起点となるノードを返す．
  const TpgNode*
  origin_node() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  AssignList
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力値のリスト
  vector<bool> mIvals;

};

END_NAMESPACE_DRUID

#endif // TPGFAULTIMPL_H
