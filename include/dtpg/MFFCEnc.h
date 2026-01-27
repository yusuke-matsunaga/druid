#ifndef MFFCENC_H
#define MFFCENC_H

/// @file MFFCEnc.h
/// @brief MFFCEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "types/TpgMFFC.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MFFCEnc MFFCEnc.h "MFFCEnc.h"
/// @brief MFFC 内の故障伝搬条件を生成するクラス
//////////////////////////////////////////////////////////////////////
class MFFCEnc :
  public SubEnc
{
public:

  /// @brief コンストラクタ
  MFFCEnc(
    const TpgMFFC& mffc   ///< [in] 対象の MFFC
  );

  /// @brief デストラクタ
  ~MFFCEnc() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬条件を表す変数を返す．
  SatLiteral
  prop_var()
  {
    return mPropVar;
  }

  /// @brief 故障検出用の制御変数の割り当てを返す．
  std::vector<SatLiteral>
  cvar_assumptions(
    const TpgFault& fault ///< [in] 対象の故障
  );

  /// @brief 直前の check() が成功したときの十分条件を求める．
  AssignList
  extract_sufficient_condition(
    const TpgFault& fault ///< [in] 対象の故障
  );


private:
  //////////////////////////////////////////////////////////////////////
  // SubEnc の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief データ構造の初期化を行う．
  void
  init() override;

  /// @brief 必要な変数を割り当て CNF 式を作る．
  void
  make_cnf() override;

  /// @brief 関連するノードのリストを返す．
  const TpgNodeList&
  node_list() const override;


private:

  // FFR に関する情報
  struct FFRInfo {
    TpgFFR mFFR;
    SatLiteral mCvar;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障挿入回路のCNFを作る．
  void
  inject_fault(
    const FFRInfo& info,
    SatLiteral ovar
  );

  /// @brief 正常値を表す変数を返す．
  SatLiteral
  gvar(
    const TpgNode& node ///< [in] 対象のノード
  ) const
  {
    return engine().gvar(node);
  }

  /// @brief 故障値を表す変数を返す．
  SatLiteral
  fvar(
    const TpgNode& node ///< [in] 対象のノード
  ) const
  {
    return mFvarMap(node);
  }

  /// @brief extract_sufficient_condition() の下請け関数
  void
  ex_sub(
    const TpgNode& node,          ///< [in] 対象のノード
    const TpgNode& end_node,      ///< [in] 終点のノード
    const std::unordered_set<SizeType>& fmark,
    AssignList& assign_list,      ///< [out] 割り当て結果を格納するオブジェクト
    std::unordered_set<SizeType>& mark ///< [inout] 処理済みの印
  );

  /// @brief 故障伝搬ノードの時 true を返す．
  bool
  is_in_fcone(
    const TpgNode& node ///< [in] 対象のノード
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の MFFC
  TpgMFFC mMFFC;

  // FFR番号をローカルな番号に変換する辞書
  std::unordered_map<SizeType, SizeType> mFFRIdMap;

  // ノード番号をキーにしてローカルなFFR番号を保持する辞書
  // FFRの根のノード以外では意味を持たない．
  std::unordered_map<SizeType, SizeType> mRootIdMap;

  // FFR 用の作業領域
  std::vector<FFRInfo> mFFRInfoArray;

  // 関係するノードのリスト
  TpgNodeList mNodeList;

  // 故障値を表す変数マップ
  VidMap mFvarMap;

  // MFFC 内の故障伝搬条件を表す変数
  SatLiteral mPropVar;

};

END_NAMESPACE_DRUID

#endif // MFFCENC_H
