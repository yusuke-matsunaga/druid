#ifndef TPGGATEIMPL_H
#define TPGGATEIMPL_H

/// @file TpgGateImpl.h
/// @brief TpgGateImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgGate.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgGateImpl TpgGateImpl.h "TpgGateImpl.h"
/// @brief ゲートの情報を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgGateImpl
{
public:

  using BranchInfo = TpgGate::BranchInfo;

public:

  /// @brief コンストラクタ
  TpgGateImpl(
    const string& name,       ///< [in] 名前
    const GateType* gate_type ///< [in] ゲートの種類
  ) : mName{name},
      mGateType{gate_type}
  {
  }

  /// @brief デストラクタ
  virtual
  ~TpgGateImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前を返す．
  string
  name() const
  {
    return mName;
  }

  /// @brief 出力に対応するノードを返す．
  virtual
  const TpgNode*
  output_node() const = 0;

  /// @brief 入力数を返す．
  virtual
  SizeType
  input_num() const = 0;

  /// @brief ブランチの情報を返す．
  virtual
  BranchInfo
  branch_info(
    SizeType pos ///< [in] 入力位置
  ) const = 0;

  /// @brief ゲートの種類を返す．
  const GateType*
  gate_type() const
  {
    return mGateType;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード名
  string mName;

  // ゲートの種類
  const GateType* mGateType;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgGate_Simple TpgGateImpl.h "TpgGateImpl.h"
/// @brief TpgNode そのものに対応する TpgGateImpl の派生クラス
//////////////////////////////////////////////////////////////////////
class TpgGate_Simple :
  public TpgGateImpl
{
public:

  /// @brief コンストラクタ
  TpgGate_Simple(
    const string& name,        ///< [in] ノード名
    const GateType* gate_type, ///< [in] ゲートの種類
    const TpgNode* node        ///< [in] ノード
  ) : TpgGateImpl{name, gate_type},
      mNode{node}
  {
  }

  /// @brief デストラクタ
  ~TpgGate_Simple() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力に対応するノードを返す．
  const TpgNode*
  output_node() const override;

  /// @brief 入力数を返す．
  SizeType
  input_num() const override;

  /// @brief ブランチの情報を返す．
  BranchInfo
  branch_info(
    SizeType pos ///< [in] 入力位置
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実際のノード
  const TpgNode* mNode;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgGate_Cplx TpgGateImpl.h "TpgGateImpl.h"
/// @brief 複合ゲート用の TpgGateImpl
//////////////////////////////////////////////////////////////////////
class TpgGate_Cplx :
  public TpgGateImpl
{
public:

  /// @brief コンストラクタ
  TpgGate_Cplx(
    const string& name,                   ///< [in] ノード名
    const GateType* gate_type,             ///< [in] ゲートの種類
    const TpgNode* node,                  ///< [in] 出力のノード
    const vector<BranchInfo>& branch_info ///< [in] ブランチの情報のリスト
  ) : TpgGateImpl{name, gate_type},
      mOutputNode{node},
      mBranchInfoList{branch_info}
  {
  }

  /// @brief デストラクタ
  ~TpgGate_Cplx() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力に対応するノードを返す．
  const TpgNode*
  output_node() const override;

  /// @brief 入力数を返す．
  SizeType
  input_num() const override;

  /// @brief ブランチの情報を返す．
  BranchInfo
  branch_info(
    SizeType pos ///< [in] 入力位置
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 出力のノード
  const TpgNode* mOutputNode;

  // ブランチの情報
  vector<BranchInfo> mBranchInfoList;

};

END_NAMESPACE_DRUID

#endif // TPGGATEIMPL_H
