#ifndef JUSTIMPL_H
#define JUSTIMPL_H

/// @file JustImpl.h
/// @brief JustImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNetwork.h"
#include "Assign.h"
#include "ym/SatModel.h"


BEGIN_NAMESPACE_DRUID

class JustData;

//////////////////////////////////////////////////////////////////////
/// @class JustImpl JustImpl.h "JustImpl.h"
/// @brief Justifier の実装クラス
//////////////////////////////////////////////////////////////////////
class JustImpl
{
public:

  /// @brief コンストラクタ
  JustImpl(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  ) : mNetwork{network}
  {
  }

  /// @brief デストラクタ
  virtual
  ~JustImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める(縮退故障用)．
  /// @return 外部入力上の値の割当リスト
  AssignList
  justify(
    const AssignList& assign_list, ///< [in] 値の割り当てリスト
    const VidMap& var_map,	        ///< [in] 変数番号のマップ
    const SatModel& model	        ///< [in] SAT問題の解
  );

  /// @brief 正当化に必要な割当を求める(遷移故障用)．
  /// @return 外部入力上の値の割当リスト
  AssignList
  justify(
    const AssignList& assign_list, ///< [in] 値の割り当てリスト
    const VidMap& var1_map,	        ///< [in] 1時刻目の変数番号のマップ
    const VidMap& var2_map,	        ///< [in] 2時刻目の変数番号のマップ
    const SatModel& model	        ///< [in] SAT問題の解
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 1時刻前の値を持つ時に true を返す．
  bool
  has_prev_state() const
  {
    return network().has_prev_state();
  }

  /// @brief JustData を得る．
  const JustData&
  just_data()
  {
    return *mJustDataPtr;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが実装する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief justify の実際の処理
  virtual
  AssignList
  _justify(
    const AssignList& assign_list ///< [in] 割当リスト
  ) = 0;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // JustData
  const JustData* mJustDataPtr{nullptr};

};

END_NAMESPACE_DRUID

#endif // JUSTIMPL_H
