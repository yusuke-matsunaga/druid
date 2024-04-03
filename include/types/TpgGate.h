#ifndef TPGGATE_H
#define TPGGATE_H

/// @file TpgGate.h
/// @brief TpgGate のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/logic.h"


BEGIN_NAMESPACE_DRUID

class GateType;

//////////////////////////////////////////////////////////////////////
/// @class TpgGate TpgGate.h "TpgGate.h"
/// @brief ゲートの情報を表すクラス
///
/// 主な目的は縮退故障/遷移故障などのゲート構造に基づいた故障の
/// 設定用の情報を提供すること
///
/// 外部入力/外部出力も「ゲート」とみなす．
//////////////////////////////////////////////////////////////////////
class TpgGate
{
public:

  /// @brief ブランチの情報を表す構造体
  struct BranchInfo
  {
    const TpgNode* node{nullptr}; ///< ノード
    SizeType ipos{0};             ///< 入力位置
  };


public:

  /// @brief コンストラクタ
  TpgGate(
    SizeType id,              ///< [in] ID番号
    const GateType* gate_type ///< [in] ゲートの種類
  ) : mId{id},
      mGateType{gate_type}
  {
  }

  /// @brief デストラクタ
  virtual
  ~TpgGate() = default;


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
  string
  name() const;

  /// @brief 出力に対応するノードを返す．
  virtual
  const TpgNode*
  output_node() const = 0;

  /// @brief 入力数を返す．
  virtual
  SizeType
  input_num() const = 0;

  /// @brief 入力に対応するノードを返す．
  const TpgNode*
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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

  // ゲートの種類
  const GateType* mGateType;

};

END_NAMESPACE_DRUID

#endif // TPGGATE_H
