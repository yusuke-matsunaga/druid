#ifndef FAULT_BRANCH_H
#define FAULT_BRANCH_H

/// @file Fault_Branch.h
/// @brief Fault_Branch とその継承クラスのヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "FaultRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Fault_Branch Fault_Branch.h "Fault_Branch.h"
/// @brief ブランチの故障を表すクラス
//////////////////////////////////////////////////////////////////////
class Fault_Branch :
  public FaultRep
{
public:

  /// @brief コンストラクタ
  Fault_Branch(
    SizeType id,         ///< [in] ID番号
    const GateRep* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : FaultRep{id, gate},
      mIpos{ipos}
  {
  }

  /// @brief デストラクタ
  ~Fault_Branch() = default;


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
  const NodeRep*
  origin_node() const override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の伝搬条件を追加する．
  void
  add_gate_propagation_condition(
    std::vector<SizeType>& assign_list ///< [out] 条件を追加するオブジェクト
  ) const;

  /// @brief 入力位置を得る．
  SizeType
  ipos() const
  {
    return mIpos;
  }

  /// @brief 故障の内容を表す文字列の基本部分を返す．
  std::string
  str_base() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力位置
  SizeType mIpos;

};


//////////////////////////////////////////////////////////////////////
/// @class Fault_BranchSa Fault_Branch.h "Fault_Branch.h"
/// @brief 入力の縮退故障を表すクラス
//////////////////////////////////////////////////////////////////////
class Fault_BranchSa :
  public Fault_Branch
{
public:

  /// @brief コンストラクタ
  Fault_BranchSa(
    SizeType id,         ///< [in] ID番号
    const GateRep* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : Fault_Branch{id, gate, ipos}
  {
  }

  /// @brief デストラクタ
  ~Fault_BranchSa() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Fault_BranchSa0 Fault_Branch.h "Fault_Branch.h"
/// @brief ブランチの0縮退故障
//////////////////////////////////////////////////////////////////////
class Fault_BranchSa0 :
  public Fault_BranchSa
{
public:

  /// @brief コンストラクタ
  Fault_BranchSa0(
    SizeType id,         ///< [in] ID番号
    const GateRep* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : Fault_BranchSa{id, gate, ipos}
  {
  }

  /// @brief デストラクタ
  ~Fault_BranchSa0() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  std::vector<SizeType>
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  std::string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Fault_BranchSa1 Fault_Branch.h "Fault_Branch.h"
/// @brief ブランチの1縮退故障
//////////////////////////////////////////////////////////////////////
class Fault_BranchSa1 :
  public Fault_BranchSa
{
public:

  /// @brief コンストラクタ
  Fault_BranchSa1(
    SizeType id,         ///< [in] ID番号
    const GateRep* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : Fault_BranchSa{id, gate, ipos}
  {
  }

  /// @brief デストラクタ
  ~Fault_BranchSa1() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  std::vector<SizeType>
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  std::string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Fault_BranchTd Fault_Branch.h "Fault_Branch.h"
/// @brief 入力の縮退故障を表すクラス
//////////////////////////////////////////////////////////////////////
class Fault_BranchTd :
  public Fault_Branch
{
public:

  /// @brief コンストラクタ
  Fault_BranchTd(
    SizeType id,         ///< [in] ID番号
    const GateRep* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : Fault_Branch{id, gate, ipos}
  {
  }

  /// @brief デストラクタ
  ~Fault_BranchTd() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Fault_BranchRise Fault_Branch.h "Fault_Branch.h"
/// @brief ブランチの rise 遷移故障
//////////////////////////////////////////////////////////////////////
class Fault_BranchRise :
  public Fault_BranchTd
{
public:

  /// @brief コンストラクタ
  Fault_BranchRise(
    SizeType id,         ///< [in] ID番号
    const GateRep* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : Fault_BranchTd{id, gate, ipos}
  {
  }

  /// @brief デストラクタ
  ~Fault_BranchRise() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  std::vector<SizeType>
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  std::string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Fault_BranchFall Fault_Branch.h "Fault_Branch.h"
/// @brief ブランチの rise 遷移故障
//////////////////////////////////////////////////////////////////////
class Fault_BranchFall :
  public Fault_BranchTd
{
public:

  /// @brief コンストラクタ
  Fault_BranchFall(
    SizeType id,         ///< [in] ID番号
    const GateRep* gate, ///< [in] 対象のゲート
    SizeType ipos        ///< [in] 入力位置
  ) : Fault_BranchTd{id, gate, ipos}
  {
  }

  /// @brief デストラクタ
  ~Fault_BranchFall() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  std::vector<SizeType>
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  std::string
  str() const override;

};

END_NAMESPACE_DRUID

#endif // FAULT_BRANCH_H
