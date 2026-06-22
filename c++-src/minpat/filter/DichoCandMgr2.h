#ifndef DICHOCANDMGR2_H
#define DICHOCANDMGR2_H

/// @file DichoCandMgr2.h
/// @brief DichoCandMgr2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqDomCandMgr.h"
#include "types/TpgFaultList.h"
#include "DichoGroup.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DichoCandMgr2 DichoCandMgr2.h "DichoCandMgr2.h"
/// @brief 二分法を用いた EqDomCandMgr
//////////////////////////////////////////////////////////////////////
class DichoCandMgr2 :
  public EqDomCandMgr
{
public:

  /// @brief コンストラクタ
  DichoCandMgr2(
    const TpgFaultList& fault_list ///< [in] 対象の故障リスト
  );

  /// @brief デストラクタ
  ~DichoCandMgr2();


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

  /// @brief 等価故障グループの候補を返す．
  TpgFaultList
  eqcand(
    const TpgFault& fault ///< [in] 対象の故障
  ) const override;


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

#endif // DICHOCANDMGR2_H
