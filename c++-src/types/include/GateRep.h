#ifndef GATE_REP_H
#define GATE_REP_H

/// @file GateRep.h
/// @brief GateRep のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/logic.h"
#include "ym/PrimType.h"
#include "types/Fval2.h"


BEGIN_NAMESPACE_DRUID

class GateType;
class NodeRep;
class FaultRep;

//////////////////////////////////////////////////////////////////////
/// @class GateRep GateRep.h "GateRep.h"
/// @brief TpgGate の実体
//////////////////////////////////////////////////////////////////////
class GateRep
{
public:

  /// @brief ブランチの情報を表す構造体
  struct BranchInfo
  {
    const NodeRep* node; ///< ノード
    SizeType ipos{0};     ///< 入力位置
  };


public:

  /// @brief コンストラクタ
  GateRep(
    SizeType id,               ///< [in] ID番号
    const GateType* gate_type, ///< [in] ゲートの種類
    FaultType fault_type       ///< [in] 故障の種類
  );

  /// @brief プリミティブ型のインスタンスを生成するクラスメソッド
  static
  GateRep*
  new_primitive(
    SizeType id,               ///< [in] ID番号
    const GateType* gate_type, ///< [in] ゲートの種類
    const NodeRep* node,       ///< [in] ノード
    FaultType fault_type       ///< [in] 故障の種類
  );

  /// @brief 複合ゲート型のインスタンスを生成するクラスメソッド
  static
  GateRep*
  new_cplx(
    SizeType id,                                ///< [in] ID番号
    const GateType* gate_type,                  ///< [in] ゲートの種類
    const NodeRep* node,                        ///< [in] 出力のノード
    const std::vector<BranchInfo>& branch_info, ///< [in] ブランチの情報のリスト
    FaultType fault_type                        ///< [in] 故障の種類
  );

  /// @brief デストラクタ
  virtual
  ~GateRep() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 名前を返す．
  std::string
  name() const;

  /// @brief 出力に対応するノードを返す．
  virtual
  const NodeRep*
  output_node() const = 0;

  /// @brief 入力数を返す．
  virtual
  SizeType
  input_num() const = 0;

  /// @brief 入力に対応するノードを返す．
  const NodeRep*
  input_node(
    SizeType pos ///< [in] 入力位置 ( 0 <= pos < input_num() )
  ) const;

  /// @brief ブランチの情報を返す．
  virtual
  BranchInfo
  branch_info(
    SizeType pos ///< [in] 入力位置
  ) const = 0;

  /// @brief PPI のときに true を返す．
  bool
  is_ppi() const;

  /// @brief PPO のときに true を返す．
  bool
  is_ppo() const;

  /// @brief 組み込みタイプのときに true を返す．
  bool
  is_simple() const;

  /// @brief 論理式タイプのときに true を返す．
  bool
  is_complex() const;

  /// @brief ゲートタイプを返す．
  ///
  /// 組み込みタイプ(is_simple() = true)のときのみ意味を持つ．
  PrimType
  primitive_type() const;

  /// @brief 論理式を返す．
  ///
  /// 論理式タイプ(is_complex() = true)のときのみ意味を持つ．
  Expr
  expr() const;

  /// @brief 追加ノード数を返す．
  SizeType
  extra_node_num() const;

  /// @brief 制御値を返す．
  ///
  /// pos に val を与えた時の出力値を返す．
  /// 他の入力値に依存している場合は val3::_X を返す．
  Val3
  cval(
    SizeType pos, ///< [in] 入力位置
    Val3 val      ///< [in] 値
  ) const;

  /// @brief ステムの故障を設定する．
  void
  set_stem_fault(
    Fval2 fval,            ///< [in] 故障値
    const FaultRep* fault ///< [in] 故障
  )
  {
    auto index = _stem_index(fval);
    mFaultArray[index] = fault;
  }

  /// @brief ブランチの故障を設定する．
  void
  set_branch_fault(
    SizeType ipos,         ///< [in] 入力位置
    Fval2 fval,            ///< [in] 故障値
    const FaultRep* fault ///< [in] 故障
  )
  {
    auto index = _branch_index(ipos, fval);
    mFaultArray[index] = fault;
  }

  /// @brief 網羅故障を設定する．
  void
  set_ex_fault(
    const std::vector<bool>& ivals, ///< [in] 入力値のベクトル
    const FaultRep* fault          ///< [in] 故障
  )
  {
    auto index = _ex_index(ivals);
    mFaultArray[index] = fault;
  }

  /// @brief ステムの故障を返す．
  ///
  /// - 定義されていない場合は nullptr を返す．
  const FaultRep*
  stem_fault(
    Fval2 fval ///< [in] 故障値
  ) const
  {
    auto index = _stem_index(fval);
    return mFaultArray[index];
  }

  /// @brief ブランチの故障を返す．
  ///
  /// - 定義されていない場合は nullptr を返す．
  const FaultRep*
  branch_fault(
    SizeType ipos, ///< [in] 入力位置
    Fval2 fval     ///< [in] 故障値
  ) const
  {
    auto index = _branch_index(ipos, fval);
    return mFaultArray[index];
  }

  /// @brief 網羅故障の故障を得る.
  const FaultRep*
  ex_fault(
    const std::vector<bool>& ivals ///< [in] 入力値のベクトル
  ) const
  {
    auto index = _ex_index(ivals);
    return mFaultArray[index];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ステムの故障のインデックスを求める．
  SizeType
  _stem_index(
    Fval2 fval ///< [in] 故障値
  ) const
  {
    auto index = input_num() * 2;
    if ( fval == Fval2::one ) {
      ++ index;
    }
    return index;
  }

  /// @brief ブランチの故障のインデックスを求める．
  SizeType
  _branch_index(
    SizeType ipos, ///< [in] 入力位置
    Fval2 fval     ///< [in] 故障値
  ) const
  {
    auto index = ipos * 2;
    if ( fval == Fval2::one ) {
      ++ index;
    }
    return index;
  }

  /// @brief 網羅故障のインデックスを求める．
  SizeType
  _ex_index(
    const std::vector<bool>& ivals ///< [in] 入力値のベクトル
  ) const
  {
    SizeType index = 0;
    for ( SizeType i = 0; i < input_num(); ++ i ) {
      if ( ivals[i] ) {
	index += (1 << i);
      }
    }
    return index;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

  // ゲートの種類
  // 所有権は GateTypeMgr が持つ．
  // このクラスは borrowed reference
  const GateType* mGateType;

  // 故障を収めた配列
  // インデックスは故障の種類によって異なる．
  std::vector<const FaultRep*> mFaultArray;

};

END_NAMESPACE_DRUID

#endif // GATE_REP_H
