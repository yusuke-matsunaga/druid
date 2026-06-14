#ifndef DICHOCANDMGR_H
#define DICHOCANDMGR_H

/// @file DichoCandMgr.h
/// @brief DichoCandMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DichoCandMgr DichoCandMgr.h "DichoCandMgr.h"
/// @brief 二分法を用いた CandMgr
//////////////////////////////////////////////////////////////////////
class DichoCandMgr :
  public CandMgr
{
public:

  /// @brief 故障グループを表すクラス
  class Group
  {
  public:

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

    /// @brief サブグループの情報を初期化する．
    void
    clear_subgroup()
    {
      mDPatList.clear();
      mSubGroupList.clear();
    }

    /// @brief 細分化のパタンリストを返す．
    const std::vector<PackedVal>&
    dpat_list() const
    {
      return mDPatList;
    }

    /// @brief サブグループのリストを返す．
    const std::vector<Group*>&
    subgroup_list() const
    {
      return mSubGroupList;
    }

    /// @brief サブグループを追加する．
    void
    add_subgroup(
      PackedVal dpat,
      Group* group
    )
    {
      mDPatList.push_back(dpat);
      mSubGroupList.push_back(group);
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

    // 細分化用のパタンリスト
    std::vector<PackedVal> mDPatList;

    // サブグループのリスト
    std::vector<Group*> mSubGroupList;

  };


public:

  /// @brief コンストラクタ
  DichoCandMgr(
    const TpgFaultList& fault_list ///< [in] 対象の故障リスト
  );

  /// @brief デストラクタ
  ~DichoCandMgr();


private:
  //////////////////////////////////////////////////////////////////////
  // CandMgr の仮想関数
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


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変化があったか調べる．
  bool
  check(
    const std::vector<std::unique_ptr<Group>>& new_group_list
  ) const;

  /// @brief 故障番号の昇順にソートする．
  std::vector<Group*>
  sort(
    std::unordered_map<SizeType, SizeType>& id_map
  ) const;

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

#endif // DICHOCANDMGR_H
