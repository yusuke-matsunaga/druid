#ifndef TPGGATE_H
#define TPGGATE_H

/// @file TpgGate.h
/// @brief TpgGate のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgObjBase.h"
#include "types/TpgNode.h"
#include "ym/logic.h"
#include "ym/PrimType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgGate TpgGate.h "TpgGate.h"
/// @brief ゲートの情報を表すクラス
/// @ingroup TypesGroup
///
/// 主な目的は縮退故障/遷移故障などのゲート構造に基づいた故障の
/// 設定用の情報を提供すること
///
/// 外部入力/外部出力も「ゲート」とみなす．
//////////////////////////////////////////////////////////////////////
class TpgGate :
  public TpgObjBase
{
public:

  /// @brief ブランチの情報を表す構造体
  struct BranchInfo
  {
    TpgNode node;     ///< ノード
    SizeType ipos{0}; ///< 入力位置
  };


public:

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgGate() = default;

  /// @brief 値を指定したコンストラクタ
  TpgGate(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    SizeType id                                 ///< [in] ID番号
  ) : TpgObjBase(network, id)
  {
  }

  /// @brief デストラクタ
  ~TpgGate() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前を返す．
  std::string
  name() const;

  /// @brief 出力に対応するノードを返す．
  TpgNode
  output_node() const;

  /// @brief 入力数を返す．
  SizeType
  input_num() const;

  /// @brief 入力に対応するノードを返す．
  TpgNode
  input_node(
    SizeType pos ///< [in] 入力位置 ( 0 <= pos < input_num() )
  ) const;

  /// @brief ブランチの情報を返す．
  BranchInfo
  branch_info(
    SizeType pos ///< [in] 入力位置
  ) const;

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

  /// @brief ステムの故障を返す．
  ///
  /// - 故障タイプが網羅故障/遷移故障でない場合には不正な呼び出しとなる．
  /// - 定義されていない場合は空の故障を返す．
  TpgFault
  stem_fault(
    Fval2 fval ///< [in] 故障値
  ) const;

  /// @brief ブランチの故障を返す．
  ///
  /// - 故障タイプが網羅故障/遷移故障でない場合には不正な呼び出しとなる．
  /// - 定義されていない場合は空の故障を返す．
  TpgFault
  branch_fault(
    SizeType ipos, ///< [in] 入力位置
    Fval2 fval     ///< [in] 故障値
  ) const;

  /// @brief 網羅故障の故障を得る.
  ///
  /// - 故障タイプが網羅故障でない場合には不正な呼び出しとなる．
  TpgFault
  ex_fault(
    const std::vector<bool>& ivals ///< [in] 入力値のベクトル
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief GateRep 本体を取り出す．
  const GateRep*
  _gate() const
  {
    return TpgBase::_gate(id());
  }

};

END_NAMESPACE_DRUID

#endif // TPGGATE_H
