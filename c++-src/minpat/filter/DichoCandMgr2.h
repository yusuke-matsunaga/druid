#ifndef DICHOCANDMGR2_H
#define DICHOCANDMGR2_H

/// @file DichoCandMgr2.h
/// @brief DichoCandMgr2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"


BEGIN_NAMESPACE_DRUID

class DiGroup;

//////////////////////////////////////////////////////////////////////
/// @class DichoCandMgr2 DichoCandMgr2.h "DichoCandMgr2.h"
/// @brief 二分法を用いた CandMgr
//////////////////////////////////////////////////////////////////////
class DichoCandMgr2 :
  public CandMgr
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
  EqDomCand
  end() override;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief グループ数を返す．
  SizeType
  group_num() const
  {
    return mCurGroupList.size();
  }

  /// @brief グループを返す．
  const DiGroup*
  group(
    SizeType id ///< [in] グループ番号
  ) const
  {
    if ( id >= group_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
    return mCurGroupList[id].get();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変化があったか調べる．
  bool
  check(
    const std::vector<std::unique_ptr<DiGroup>>& new_group_list
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 現在のグループのリスト
  // DiGroup の所有権を持つ．
  std::vector<std::unique_ptr<DiGroup>> mCurGroupList;

};

END_NAMESPACE_DRUID

#endif // DICHOCANDMGR2_H
