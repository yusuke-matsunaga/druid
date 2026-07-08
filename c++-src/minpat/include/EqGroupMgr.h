#ifndef EQGROUPMGR_H
#define EQGROUPMGR_H

/// @file EqGroupMgr.h
/// @brief EqGroupMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#define EQDOMCAND 0

#include "druid.h"
#include "RedMgr.h"
#if EQDOMCAND
#include "EqDomCand.h"
#endif
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

class EqGroupState;

//////////////////////////////////////////////////////////////////////
/// @class EqGroupMgr EqGroupMgr.h "EqGroupMgr.h"
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
class EqGroupMgr :
  public RedMgr
{
public:

  /// @brief コンストラクタ
  EqGroupMgr(
    FaultInfo& fault_info,    ///< [in] 故障の情報
    Fsim& fsim,               ///< [in] 故障シミュレータ
    const ConfigParam& option ///< [in] オプション
  );

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<EqGroupMgr>
  new_obj(
    FaultInfo& fault_info,    ///< [in] 故障の情報
    Fsim& fsim,               ///< [in] 故障シミュレータ
    const ConfigParam& option ///< [in] オプション
  );

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<EqGroupMgr>
  new_naive_mgr(
    FaultInfo& fault_info,    ///< [in] 故障の情報
    Fsim& fsim,               ///< [in] 故障シミュレータ
    const ConfigParam& option ///< [in] オプション
  );

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<EqGroupMgr>
  new_dichotomy_mgr(
    FaultInfo& fault_info,    ///< [in] 故障の情報
    Fsim& fsim,               ///< [in] 故障シミュレータ
    const ConfigParam& option ///< [in] オプション
  );

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<EqGroupMgr>
  new_dichotomy_mgr2(
    FaultInfo& fault_info,    ///< [in] 故障の情報
    Fsim& fsim,               ///< [in] 故障シミュレータ
    const ConfigParam& option ///< [in] オプション
  );

  /// @brief デストラクタ
  virtual
  ~EqGroupMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障シミュレーションを行って故障グループを細分化する．
  /// @return 変化があったら true を返す．
  bool
  subdivide(
    const std::vector<TestVector>& tv_list, ///< [in] テストパタンのリスト
    std::function<void(const FsimResults&)> callback ///< [in] コールバック関数
    = [](const FsimResults&) { }
  );

  /// @brief 現在の状態を取り出す．
  EqGroupState
  cur_state() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 更新処理
  virtual
  bool
  update(
    const std::vector<DPat>& dpat_array ///< [in] 故障の検出状況のピットパタンの配列
  ) = 0;

#if EQDOMCAND
  /// @brief 結果を返す．
  virtual
  std::unique_ptr<EqDomCand>
  end(
    bool reduce ///< [in] 推移簡約を行う時 true
  ) const = 0;
#endif

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
  pred_list(
    SizeType group_id ///< [in] 故障グループ番号 ( 0 <= group_id < group_num() )
  ) const = 0;

  /// @brief 順序関係の要素数を返す．
  virtual
  SizeType
  domcand_num() const = 0;

};

END_NAMESPACE_DRUID

#endif // EQGROUPMGR_H
