#ifndef DICHOMGR2_H
#define DICHOMGR2_H

/// @file DichoMgr2.h
/// @brief DichoMgr2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqDomMgr.h"
#include "types/TpgFaultList.h"
#include "DichoGroup.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DichoMgr2 DichoMgr2.h "DichoMgr2.h"
/// @brief 二分法を用いた EqDomMgr
//////////////////////////////////////////////////////////////////////
class DichoMgr2 :
  public EqDomMgr
{
public:

  /// @brief コンストラクタ
  DichoMgr2(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const ConfigParam& option       ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DichoMgr2();


private:
  //////////////////////////////////////////////////////////////////////
  // Filter の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 更新処理
  bool
  update(
    const std::vector<PackedVal>& dpat_array ///< [in] 故障の検出状況のピットパタン
  ) override;

  /// @brief 終了処理
  std::unique_ptr<EqDomCand>
  end(
    bool reduce ///< [in] 推移簡約を行う時 true
  ) const override;

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
  prev_list(
    SizeType group_id ///< [in] 故障グループ番号 ( 0 <= group_id < group_num() )
  ) const override;

  /// @brief set_rep() に関連した処理を行う．
  void
  after_set_rep(
    const TpgFault& fault ///< [in] 変化した故障
  ) override;

  /// @brief 順序関係の要素数を返す．
  SizeType
  domcand_num() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障グループのリスト情報を出力する．
  ///
  /// print() と似ているが中間的な状態にも対応している．
  static
  void
  print_group_list(
    std::ostream& s,                               ///< [in] 出力ストリーム
    const std::vector<DichoGroup::Ptr>& group_list ///< [in] グループのリスト
  );

  /// @brief パタンを文字列にする．
  static
  std::string
  pat_str(
    PackedVal pat
  );

  /// @brief パタンのリストを文字列にする．
  static
  std::string
  pat_list_str(
    const std::vector<PackedVal>& pat_list
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 現在のグループのリスト
  // グループの所有権を持つ．
  std::vector<DichoGroup::Ptr> mCurGroupList;

};

END_NAMESPACE_DRUID

#endif // DICHOMGR2_H
