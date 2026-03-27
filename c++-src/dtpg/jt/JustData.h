#ifndef JUSTDATA_H
#define JUSTDATA_H

/// @file JustData.h
/// @brief JustData のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgNetwork.h"
#include "types/TpgNode.h"
#include "dtpg/VidMap.h"
#include "types/AssignList.h"
#include "types/Val3.h"
#include "ym/SatModel.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class JustData JustData.h "JustData.h"
/// @brief Justify 用の情報を保持するクラス
//////////////////////////////////////////////////////////////////////
class JustData
{
public:

  /// @brief コンストラクタ(縮退故障用)
  JustData(
    const TpgNetwork& network,    ///< [in] 対象のネットワーク
    const VidMap& var_map,        ///< [in] 変数番号のマップ
    const SatModel& model,        ///< [in] SATソルバの作ったモデル
    const AssignList& assign_list ///< [in] 事前の値割り当て
  ) : JustData(network, var_map, var_map, model, assign_list)
  {
  }

  /// @brief コンストラクタ(遷移故障用)
  JustData(
    const TpgNetwork& network,    ///< [in] 対象のネットワーク
    const VidMap& var1_map,       ///< [in] 1時刻目の変数番号のマップ
    const VidMap& var2_map,       ///< [in] 2時刻目の変数番号のマップ
    const SatModel& model,        ///< [in] SATソルバの作ったモデル
    const AssignList& assign_list ///< [in] 事前の値割り当て
  );

  /// @brief デストラクタ
  ~JustData() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの値を返す．
  Val3
  val(
    const TpgNode& node, ///< [in] ノード
    int time		 ///< [in] 時刻 ( 0 or 1 )
  ) const
  {
    auto l = lit(node, time);
    if ( l == SatLiteral::X ) {
      return Val3::_X;
    }
    return bool3_to_val3(mSatModel[l]);
  }

  /// @brief 入力ノードの値を記録する．
  void
  record_value(
    const TpgNode& node,    ///< [in] 対象の外部入力ノード
    int time,	  	    ///< [in] 時刻 ( 0 or 1 )
    AssignList& assign_list ///< [out] 値の割当リスト
  ) const
  {
    auto v = val(node, time);
    if ( v != Val3::_X ) {
      bool bval = (v == Val3::_1);
      assign_list.add(node, time, bval);
    }
  }

  /// @brief fixed マークを得る．
  bool
  fixed_mark(
    const TpgNode& node, ///< [in] 対象のノード
    int time             ///< [in] 時刻 ( 0 or 1 )
  ) const
  {
    return mFixedMark[node.id() * 2 + time];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 値が確定しているか調べる．
  bool
  check_fixed(
    const TpgNode& node, ///< [in] 対象のノード
    int time             ///< [in] 時刻 ( 0 or 1 )
  );

  /// @brief fixed マークを付ける．
  void
  set_fixed(
    const TpgNode& node, ///< [in] 対象のノード
    int time             ///< [in] 時刻 ( 0 or 1 )
  )
  {
    mFixedMark[node.id() * 2 + time] = true;
  }

  /// @brief 値に対応するリテラルを返す．
  SatLiteral
  lit(
    const TpgNode& node, ///< [in] 対象のノード
    int time             ///< [in] 時刻 ( 0 or 1 )
  ) const
  {
    auto& varmap = (time == 0) ? mVar1Map : mVar2Map;
    return varmap(node);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 1時刻目の変数マップ
  const VidMap& mVar1Map;

  // 2時刻目の変数マップ
  const VidMap& mVar2Map;

  // SAT ソルバの解
  const SatModel& mSatModel;

  // fixedマーク
  std::vector<bool> mFixedMark;

};

END_NAMESPACE_DRUID

#endif // JUSTDATA_H
