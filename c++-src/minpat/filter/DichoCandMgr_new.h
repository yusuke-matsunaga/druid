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
      SizeType id,                            ///< [in] ID番号
      SizeType rank,                          ///< [in] ランク
      const std::vector<PackedVal>& pat_list, ///< [in] パタンのリスト
      const TpgFaultList& fault_list          ///< [in] 故障のリスト
    ) : mId{id},
	mRank{rank},
	mPatList{pat_list},
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

    /// @brief ランクを返す．
    SizeType
    rank() const
    {
      return mRank;
    }

    /// @brief パタンのリストを返す．
    const std::vector<PackedVal>&
    pat_list() const
    {
      return mPatList;
    }

    /// @brief 故障のリストを返す．
    const TpgFaultList&
    fault_list() const
    {
      return mFaultList;
    }

    /// @brief 直接の後続グループのリストを返す．
    const std::vector<Group*>&
    immediate_succ_list() const
    {
      return mImmSuccList;
    }

    /// @brief 全ての後続グループのリストを返す．
    const std::vector<Group*>&
    transitive_succ_list() const
    {
      return mTranSuccList;
    }

    /// @brief immediate_succ_list を設定する．
    void
    set_immediate_succ_list(
      std::vector<Group*>&& src_list ///< [in] 設定するグループのリストの右辺値
    )
    {
      std::swap(mImmSuccList, src_list);
    }

    /// @brief transitive_succ_list を設定する．
    void
    set_transitive_succ_list(
      std::vector<Group*>&& src_list ///< [in] 設定するグループのリストの右辺値
    )
    {
      std::swap(mTranSuccList, src_list);
    }

    /// @brief サブグループの情報を初期化する．
    void
    clear_subgroup()
    {
      mDPatList.clear();
      mSubGroupDict.clear();
    }

    /// @brief 細分化のパタンリストを返す．
    const std::vector<PackedVal>&
    dpat_list() const
    {
      return mDPatList;
    }

    /// @brief パタンに対応したサブグループを持っている時 true を返す．
    bool
    has_subgroup(
      PackedVal dpat ///< [in] 細分化するパタン
    ) const
    {
      return mSubGroupDict.count(dpat) > 0;
    }

    /// @brief 細分化したサブグループを返す．
    ///
    /// has_subgroup(dpat) == true の時のみ有効
    Group*
    subgroup(
      PackedVal dpat ///< [in] 細分化するパタン (dpat_list() の要素)
    ) const
    {
      return mSubGroupDict.at(dpat);
    }

    /// @brief サブグループを追加する．
    void
    add_subgroup(
      PackedVal dpat,
      Group* group
    )
    {
      mDPatList.push_back(dpat);
      mSubGroupDict.emplace(dpat, group);
    }


  private:
    //////////////////////////////////////////////////////////////////////
    // データメンバ
    //////////////////////////////////////////////////////////////////////

    // ID番号
    SizeType mId;

    // ランク
    SizeType mRank;

    // パタンのリスト
    std::vector<PackedVal> mPatList;

    // 故障のリスト
    TpgFaultList mFaultList;

    // 直接の後続グループのリスト
    std::vector<Group*> mImmSuccList;

    // 推移的な後続グループのリスト
    std::vector<Group*> mTranSuccList;

    // 細分化用のパタンリスト
    // mSubGroupDict のキーに一致する．
    std::vector<PackedVal> mDPatList;

    // 細分化したサブグループの辞書
    std::unordered_map<PackedVal, Group*> mSubGroupDict;

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

  /// @brief グループ数を返す．
  SizeType
  group_num() const
  {
    return mCurGroupList.size();
  }

  /// @brief グループを返す．
  const Group*
  group(
    SizeType id ///< [in] グループ番号
  ) const
  {
    if ( id >= group_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
    return mCurGroupList[id].get();
  }


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
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
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障グループのリストの情報を出力する．
  ///
  /// print() と似ているが中間的な状態にも対応している．
  static
  void
  print_group_list(
    std::ostream& s,                                      ///< [in] 出力ストリーム
    const std::vector<std::unique_ptr<Group>>& group_list ///< [in] グループのリスト
  );

  /// @brief 故障グループの情報を出力する．
  ///
  /// print() と似ているが中間的な状態にも対応している．
  static
  void
  print_group(
    std::ostream& s,   ///< [in] 出力ストリーム
    const Group* group ///< [in] グループのリスト
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 現在のグループのリスト
  // Group の所有権を持つ．
  std::vector<std::unique_ptr<Group>> mCurGroupList;

};

END_NAMESPACE_DRUID

#endif // DICHOCANDMGR_H
