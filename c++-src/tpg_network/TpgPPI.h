#ifndef TPGPPI_H
#define TPGPPI_H

/// @file TpgPPI.h
/// @brief TpgPPI のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgPPI TpgPPI.h "TpgPPI.h"
/// @brief FF出力も含めた擬似外部入力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgPPI :
  public TpgNode
{
protected:

  /// @brief コンストラクタ
  TpgPPI(
    SizeType input_id ///< [in] 入力番号
  );

  /// @brief デストラクタ
  ~TpgPPI() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_input() || is_dff_output()
  bool
  is_ppi() const override;

  /// @brief 外部入力タイプの時に入力番号を返す．
  ///
  /// node = TpgNetwork::input(node->input_id()
  /// の関係を満たす．
  /// is_input() が false の場合の返り値は不定
  SizeType
  input_id() const override;

  /// @brief ゲートタイプを得る．
  ///
  /// - is_logic() が true の時はゲートタイプを返す．
  /// - is_ppi() が true の時は kGateINPUT を返す．
  /// - is_ppo() が true の時は kGateBUFF を返す．
  /// - それ以外の返り値は不定
  GateType
  gate_type() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力番号
  SizeType mInputId;

};

END_NAMESPACE_DRUID

#endif // TPGPPI_H
