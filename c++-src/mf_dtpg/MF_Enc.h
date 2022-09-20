#ifndef MF_ENC_H
#define MF_ENC_H

/// @file MF_Enc.h
/// @brief MF_Enc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNetwork.h"
#include "ym/sat.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MF_Enc MF_Enc.h "MF_Enc.h"
/// @brief 多重故障回路のCNFを作るクラス
//////////////////////////////////////////////////////////////////////
class MF_Enc
{
public:

public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFR に対する正常回路を作る．
  /// @param[in] solver SATソルバ
  /// @param[in] input_list 入力のノードと対応するSATのリテラルのペアのリスト
  /// @param[in] onode 出力のノード
  /// @param[in] olit 出力のノードに対応するSATのリテラル
  static
  void
  make_good_FFR(
    SatSolver& solver,
    const vector<pair<const TpgNode*, SatLiteral>>& input_list,
    const TpgNode* onode,
    SatLiteral olit
  );

  /// @brief 部分回路に対する正常回路を作る．
  /// @param[in] solver SATソルバ
  /// @param[in] input_list 入力のノードと対応するSATのリテラルのペアのリスト
  /// @param[in] output_list 出力のノードと対応するSATのリテラルのペアのリスト
  static
  void
  make_good_cnf(
    SatSolver& solver,
    const vector<pair<const TpgNode*, SatLiteral>>& input_list,
    const vector<pair<const TpgNode*, SatLiteral>>& output_list
  );

  /// @brief FFR に対する故障回路を作る．
  /// @param[in] solver SATソルバ
  /// @param[in] network 対象のネットワーク
  /// @param[in] input_list 入力のノードと対応するSATのリテラルのペアのリスト
  /// @param[in] onode 出力のノード
  /// @param[in] olit 出力のノードに対応するSATのリテラル
  /// @param[in] fault_list 代表故障と対応するSATのリテラルのペアのリスト
  static
  void
  make_faulty_FFR(
    SatSolver& solver,
    const TpgNetwork& network,
    const vector<pair<const TpgNode*, SatLiteral>>& input_list,
    const TpgNode* onode,
    SatLiteral olit,
    const vector<pair<const TpgFault*, SatLiteral>>& fault_list
  );

  /// @brief 部分回路に対する故障回路を作る．
  /// @param[in] solver SATソルバ
  /// @param[in] network 対象のネットワーク
  /// @param[in] input_list 入力のノードと対応するSATのリテラルのペアのリスト
  /// @param[in] output_list 出力のノードと対応するSATのリテラルのペアのリスト
  /// @param[in] fault_list 代表故障と対応するSATのリテラルのペアのリスト
  static
  void
  make_faulty_cnf(
    SatSolver& solver,
    const TpgNetwork& network,
    const vector<pair<const TpgNode*, SatLiteral>>& input_list,
    const vector<pair<const TpgNode*, SatLiteral>>& output_list,
    const vector<pair<const TpgFault*, SatLiteral>>& fault_list
  );

};

END_NAMESPACE_DRUID

#endif // MF_ENC_H
