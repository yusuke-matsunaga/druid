#ifndef DICHOMGR_H
#define DICHOMGR_H

/// @file DichoMgr.h
/// @brief DichoMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqGroupMgr.h"
#include "types/TpgFaultList.h"
#include "DichoGroup.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DichoMgr DichoMgr.h "DichoMgr.h"
/// @brief 二分法を用いた EqGroupMgr
//////////////////////////////////////////////////////////////////////
class DichoMgr :
  public EqGroupMgr
{
public:

  /// @brief コンストラクタ
  DichoMgr(
    FaultInfo& fault_info,    ///< [in] 故障の情報
    Fsim& fsim,               ///< [in] 故障シミュレータ
    const ConfigParam& option ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DichoMgr();


private:
  //////////////////////////////////////////////////////////////////////
  // CandMgr の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 更新処理
  bool
  update(
    const std::vector<DPat>& dpat_array ///< [in] 故障の検出状況のピットパタンの配列
  ) override;

#if EQDOMCAND
  /// @brief 終了処理
  std::unique_ptr<EqDomCand>
  end(
    bool reduce ///< [in] 推移簡約を行う時 true
  ) const override;
#endif

  /// @brief 等価故障グループ数を返す．
  SizeType
  group_num() const override;

  /// @brief 等価故障グループ番号を返す．
  SizeType
  group_id(
    const TpgFault& fault ///< [in] 対象の故障
  ) const override;

  /// @brief 等価故障グループの故障リストを返す．
  TpgFaultList
  fault_list(
    SizeType group_id ///< [in] 故障グループ番号 ( 0 <= group_id < group_num() )
  ) const override;

  /// @brief 後続グループ番号のリスト返す．
  std::vector<SizeType>
  succ_list(
    SizeType group_id ///< [in] 故障グループ番号 ( 0 <= group_id < group_num() )
  ) const override;

  /// @brief 先行グループ番号のリスト返す．
  std::vector<SizeType>
  pred_list(
    SizeType group_id ///< [in] 故障グループ番号 ( 0 <= group_id < group_num() )
  ) const override;

  /// @brief 順序関係の要素数を返す．
  SizeType
  domcand_num() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief mGroupMap を作る．
  void
  _fix_group_map();

  /// @brief グループ番号をチェックする．
  void
  _check_group_id(
    SizeType group_id
  ) const
  {
    if ( group_id >= mCurGroupList.size() ) {
      throw std::out_of_range{"group_id is out of range"};
    }
  }

  /// @brief 故障グループのリスト情報を出力する．
  ///
  /// print() と似ているが中間的な状態にも対応している．
  static
  void
  print_group_list(
    std::ostream& s,                               ///< [in] 出力ストリーム
    const std::vector<DichoGroup::Ptr>& group_list ///< [in] グループのリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 現在のグループのリスト
  // Group の所有権を持つ．
  std::vector<DichoGroup::Ptr> mCurGroupList;

  // 故障番号をキーにして対象のグループを格納する配列
  std::vector<DichoGroup*> mGroupMap;

};

END_NAMESPACE_DRUID

#endif // DICHOMGR_H
