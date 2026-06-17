#ifndef DICHOCANDMGR2_H
#define DICHOCANDMGR2_H

/// @file DichoCandMgr2.h
/// @brief DichoCandMgr2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"
#include "types/TpgFaultList.h"
#include "POSet.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DichoCandMgr2 DichoCandMgr2.h "DichoCandMgr2.h"
/// @brief 二分法を用いた CandMgr
//////////////////////////////////////////////////////////////////////
class DichoCandMgr2 :
  public CandMgr
{
public:

  /// @brief 故障グループを表すクラス
  class Group
  {
  public:

    /// @brief コンストラクタ
    Group(
      SizeType id                   ///< [in] ID番号
    ) : mId{id}
    {
    }

    /// @brief コンストラクタ
    Group(
      SizeType id,                   ///< [in] ID番号
      const TpgFaultList& fault_list ///< [in] 故障のリスト
    ) : mId{id},
	mFaultList{fault_list}
    {
    }

    /// @brief デストラクタ
    ~Group() = default;


  public:
    //////////////////////////////////////////////////////////////////////
    // 外部インターフェイス
    //////////////////////////////////////////////////////////////////////

    /// @brief ID番号を返す．
    SizeType
    id() const
    {
      return mId;
    }

    /// @brief 故障のリストを返す．
    const TpgFaultList&
    fault_list() const
    {
      return mFaultList;
    }

    /// @brief 故障を追加する．
    void
    add_fault(
      const TpgFault& fault ///< [in] 追加する故障
    )
    {
      mFaultList.push_back(fault);
    }

    /// @brief 後続グループのリストを返す．
    const std::vector<Group*>&
    succ_list() const
    {
      return mSuccList;
    }

    /// @brief succ_list を設定する．
    void
    set_succ_list(
      std::vector<Group*>&& src_list ///< [in] 設定するグループのリストの右辺値
    )
    {
      std::swap(mSuccList, src_list);
    }


  private:
    //////////////////////////////////////////////////////////////////////
    // データメンバ
    //////////////////////////////////////////////////////////////////////

    // ID番号
    SizeType mId;

    // 故障のリスト
    TpgFaultList mFaultList;

    // 後続グループのリスト
    std::vector<Group*> mSuccList;

  };


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

  /// @brief 細分化したサブグループの情報
  struct SubGroup {
    SizeType id;
    Group* group;
    TpgFaultList fault_list;
    std::vector<SubGroup*> succ_list;
  };

  /// @brief subgroup から到達可能な Group のリストを求める．
  static
  void
  dfs(
    const SubGroup* subgroup,
    Group* from,
    std::unordered_set<SizeType>& mark,
    POSet::Builder& builder
  );

  /// @brief 故障グループのリスト情報を出力する．
  ///
  /// print() と似ているが中間的な状態にも対応している．
  static
  void
  print_group_list(
    std::ostream& s,                                      ///< [in] 出力ストリーム
    const std::vector<std::unique_ptr<Group>>& group_list ///< [in] グループのリスト
  );

  /// @brief 故障グループの情報を出力する．
  static
  void
  print_group(
    std::ostream& s, ///< [in] 出力ストリーム
    Group* group     ///< [in] グループ
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
  // Group の所有権を持つ．
  std::vector<std::unique_ptr<Group>> mCurGroupList;

};

END_NAMESPACE_DRUID

#endif // DICHOCANDMGR2_H
