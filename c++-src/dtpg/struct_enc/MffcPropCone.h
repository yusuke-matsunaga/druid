#ifndef MFFCPROPCONE_H
#define MFFCPROPCONE_H

/// @file MffcPropCone.h
/// @brief MffcPropCone のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PropCone.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MffcPropCone MffcPropCone.h "MffcPropCone.h"
/// @brief MFFC内の故障をひとまとめに扱う PropCone
//////////////////////////////////////////////////////////////////////
class MffcPropCone :
  public PropCone
{
public:

  /// @brief コンストラクタ
  MffcPropCone(
    StructEnc& struct_enc, ///< [in] StructEnc
    const TpgMFFC& mffc,   ///< [in] MFFC の情報
    bool detect		   ///< [in] 故障を検出する時に true にするフラグ
  );

  /// @brief デストラクタ
  ~MffcPropCone();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 関係するノードの変数を作る．
  void
  make_vars(
    const vector<const TpgNode*>& node_list
  ) override;

  /// @brief 関係するノードの入出力の関係を表すCNFを作る．
  void
  make_cnf() override;

  /// @brief 故障の影響伝搬させる条件を作る．
  vector<SatLiteral>
  make_condition(
    const TpgNode* root ///< [in] 起点となるノード
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief node に関する故障伝搬条件を作る．
  void
  make_dchain_cnf(
    const TpgNode* node ///< [in] 対象のノード
  );

  /// @brief 故障挿入回路のCNFを作る．
  void
  inject_fault(
    SizeType ffr_pos, ///< [in] 要素番号
    SatLiteral ovar   ///< [in] ゲートの出力の変数
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFR の根のリスト
  // [0] は MFFC の根でもある．
  vector<const TpgNode*> mElemArray;

  // 各FFRの根に反転イベントを挿入するための変数
  // サイズは mElemArray.size()
  vector<SatLiteral> mElemVarArray;

  // ノード番号をキーにしてFFR番号を入れる連想配列
  unordered_map<SizeType, SizeType> mElemPosMap;

};

END_NAMESPACE_DRUID

#endif // MFFCPROPCONE_H
