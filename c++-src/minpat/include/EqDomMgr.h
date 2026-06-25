#ifndef EQDOMMGR_H
#define EQDOMMGR_H

/// @file EqDomMgr.h
/// @brief EqDomMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "EqDomCand.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "types/PackedVal.h"
#include "fsim/Fsim.h"
#include "misc/ConfigParam.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class EqDomMgr EqDomMgr.h "EqDomMgr.h"
/// @brief 等価故障と支配故障候補のマネージャクラス
///
/// f1 が検出されて f2 が検出されないパタンがあった場合，
/// f1 が f2 を支配することはない．
/// この情報を管理するためのクラス
///
/// ただし，そのままでは反対称律を満たさないので，
/// 等価故障のグループを考えてそのグループ間の順序関係として表す．
///
/// * 反対称律: a < b かつ b < a となるのは a = b の時のみ
//////////////////////////////////////////////////////////////////////
class EqDomMgr
{
public:

  /// @brief 結果を表す
public:

  /// @brief コンストラクタ
  EqDomMgr(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const ConfigParam& option       ///< [in] オプション
  );

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<EqDomMgr>
  new_obj(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const ConfigParam& option       ///< [in] オプション
  );

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<EqDomMgr>
  new_naive_mgr(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const ConfigParam& option       ///< [in] オプション
  );

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<EqDomMgr>
  new_dichotomy_mgr(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const ConfigParam& option       ///< [in] オプション
  );

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<EqDomMgr>
  new_dichotomy_mgr2(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const ConfigParam& option       ///< [in] オプション
  );

  /// @brief デストラクタ
  virtual
  ~EqDomMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象の故障リストを返す．
  TpgFaultList
  fault_list() const
  {
    return mFaultList;
  }

  /// @brief 対象のネットワークを返す．
  TpgNetwork
  network() const
  {
    return fault_list().network();
  }

  /// @brief 代表故障を返す．
  ///
  /// ない場合には不正な値を返す．
  TpgFault
  rep_fault(
    const TpgFault& fault ///< [in] 故障
  ) const
  {
    return mRepFaultArray[fault.id()];
  }

  /// @brief 自身が代表故障の時 true を返す．
  bool
  is_rep(
    const TpgFault& fault ///< [in] 故障
  ) const
  {
    return !rep_fault(fault).is_valid();
  }

  /// @brief 代表故障をセットする．
  ///
  /// この故障は代表故障ではなくなる．
  void
  set_rep(
    const TpgFault& fault, ///< [in] 対象の故障
    const TpgFault& rep_fault ///< [in] 代表故障
  )
  {
    mRepFaultArray[fault.id()] = rep_fault;
    after_set_rep(fault);
  }

  /// @brief 故障番号の最大値を返す．
  SizeType
  max_fault_size() const
  {
    return network().max_fault_id();
  }

  /// @brief 故障シミュレーションを行って故障グループを細分化する．
  /// @return 変化があったら true を返す．
  bool
  subdivide(
    const std::vector<TestVector>& tv_list, ///< [in] テストパタンのリスト
    std::function<void(const FsimResults&)> callback ///< [in] コールバック関数
    = [](const FsimResults&) { }
  );

  /// @brief 故障シミュレーションの時間を返す．
  double
  fsim_time() const
  {
    return mFsimTimer.get_time();
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 更新処理
  virtual
  bool
  update(
    const std::vector<PackedVal>& dpat_array ///< [in] 故障の検出状況のピットパタン
  ) = 0;

  /// @brief 結果を返す．
  virtual
  std::unique_ptr<EqDomCand>
  end(
    bool reduce ///< [in] 推移簡約を行う時 true
  ) const = 0;

  /// @brief 等価故障グループ数を返す．
  virtual
  SizeType
  group_num() const = 0;

  /// @brief 等価故障グループ番号を返す．
  virtual
  SizeType
  group_id(
    const TpgFault& fault ///< [in] 対象の故障
  ) const = 0;

  /// @brief 等価故障グループの故障リストを返す．
  virtual
  TpgFaultList
  fault_list(
    SizeType group_id ///< [in] 故障グループ番号 ( 0 <= group_id < group_num() )
  ) const = 0;

  /// @brief 後続グループ番号のリスト返す．
  ///
  /// 自身は含まない
  virtual
  std::vector<SizeType>
  succ_list(
    SizeType group_id ///< [in] 故障グループ番号 ( 0 <= group_id < group_num() )
  ) const = 0;

  /// @brief 先行グループ番号のリスト返す．
  ///
  /// 自身は含まない
  virtual
  std::vector<SizeType>
  prev_list(
    SizeType group_id ///< [in] 故障グループ番号 ( 0 <= group_id < group_num() )
  ) const = 0;

  /// @brief set_rep() に関連した処理を行う．
  virtual
  void
  after_set_rep(
    const TpgFault& fault ///< [in] 変化した故障
  ) = 0;

  /// @brief 順序関係の要素数を返す．
  virtual
  SizeType
  domcand_num() const = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障シミュレータ
  Fsim mFsim;

  // 対象の故障のリスト
  TpgFaultList mFaultList;

  // 故障番号キーにして代表故障を持つ配列
  std::vector<TpgFault> mRepFaultArray;

  // 故障シミュレータ用のタイマ
  Timer mFsimTimer;

};

END_NAMESPACE_DRUID

#endif // EQDOMMGR_H
