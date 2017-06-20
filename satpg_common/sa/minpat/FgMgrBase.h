#ifndef FGMGRBASE_H
#define FGMGRBASE_H

/// @file FgMgrBase.h
/// @brief FgMgrBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "FgMgr.h"

#include "NodeValList.h"

#include "FaultAnalyzer.h"
#include "FaultInfo.h"

#include "ym/HashSet.h"
#include "ym/USTime.h"


BEGIN_NAMESPACE_YM_SATPG_SA

//////////////////////////////////////////////////////////////////////
/// @class FgMgrBase FgMgrBase.h "FgMgrBase.h"
/// @brief fault group manager
///
/// 故障グループを管理するクラス
//////////////////////////////////////////////////////////////////////
class FgMgrBase :
  public FgMgr
{
private:

  class FaultGroup;

public:

  /// @brief コンストラクタ
  /// @param[in] max_node_id ノード番号の最大値 + 1
  /// @param[in] analyzer 故障解析器
  FgMgrBase(ymuint max_node_id,
	    const FaultAnalyzer& analyzer);

  /// @brief デストラクタ
  ~FgMgrBase();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief クリアする．
  virtual
  void
  clear();

  /// @brief 現在のグループ数を返す．
  virtual
  ymuint
  group_num() const;

  /// @brief 新しいグループを作る．
  /// @param[in] fid 故障番号
  /// @return グループ番号を返す．
  ///
  /// fid のみを要素に持つ．
  virtual
  ymuint
  new_group(ymuint fid);

  /// @brief グループを複製する．
  /// @param[in] src_gid 複製元のグループ番号
  /// @return 新しいグループ番号を返す．
  virtual
  ymuint
  duplicate_group(ymuint src_gid);

  /// @brief グループを置き換える．
  /// @param[in] old_gid 置き換え対象のグループ番号
  /// @param[in] new_gid 置き換えるグループ番号
  ///
  /// new_gid は削除される．
  virtual
  void
  replace_group(ymuint old_gid,
		ymuint new_gid);

  /// @brief グループを削除する．
  /// @param[in] gid グループ番号
  virtual
  void
  delete_group(ymuint gid);

  /// @brief 新たな条件なしで追加できる既存グループを見つける．
  /// @param[in] fid 対象の故障番号
  /// @param[in] group_list 探索最小のグループ番号のリスト
  /// @param[in] first_hit 最初のグループのみを求めるとき true にするフラグ
  /// @param[out] gid_list 対象のグループ番号を収めるリスト
  /// @return 最初のグループ番号を返す．
  ///
  /// 見つからない場合は group_num() を返す．
  /// gid_list は first_hit == true の時，意味を持たない．
  virtual
  ymuint
  find_dom_group(ymuint fid,
		 const vector<ymuint>& group_list,
		 bool first_hit,
		 vector<ymuint>& gid_list);

  /// @brief 追加できる既存グループを見つける．
  /// @param[in] fid 対象の故障番号
  /// @param[in] group_list 探索最小のグループ番号のリスト
  /// @param[in] fast 高速ヒューリスティック
  /// @param[in] first_hit 最初のグループのみを求めるとき true にするフラグ
  /// @param[out] gid_list 対象のグループ番号を収めるリスト
  /// @return 最初のグループ番号を返す．
  ///
  /// 見つからない場合は group_num() を返す．
  /// gid_list は first_hit == true の時，意味を持たない．
  virtual
  ymuint
  find_group(ymuint fid,
	     const vector<ymuint>& group_list,
	     bool fast,
	     bool first_hit,
	     vector<ymuint>& gid_list);

  /// @brief 追加できる既存グループを見つけて追加する．
  /// @param[in] fid 対象の故障番号
  /// @param[in] group_list 探索最小のグループ番号のリスト
  /// @param[in] fast 高速ヒューリスティック
  /// @return 見つかったグループ番号を返す．
  ///
  /// 見つからない場合は group_num() を返す．
  virtual
  ymuint
  find_group2(ymuint fid,
	      const vector<ymuint>& group_list,
	      bool fast);

  /// @brief 故障を取り除く
  /// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
  /// @param[in] fid_list 削除する故障番号のリスト
  virtual
  void
  delete_faults(ymuint gid,
		const vector<ymuint>& fid_list);

  /// @brief グループの故障数を返す．
  /// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
  virtual
  ymuint
  fault_num(ymuint gid) const;

  /// @brief グループの故障を返す．
  /// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
  /// @param[in] pos ( 0 <= pos < fault_num(gid) )
  /// @return 故障番号を返す．
  virtual
  ymuint
  fault_id(ymuint gid,
	   ymuint pos) const;

  /// @brief 十分割当リストを返す．
  /// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
  virtual
  const NodeValList&
  sufficient_assignment(ymuint gid) const;

  /// @brief 必要割当リストを返す．
  /// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
  virtual
  const NodeValList&
  mandatory_assignment(ymuint gid) const;

  /// @brief 複数故障の検出検査回数
  ymuint
  mfault_num() const;

  /// @brief 複数故障の平均多重度
  double
  mfault_avg() const;

  /// @brief 複数故障の最大値
  ymuint
  mfault_max() const;

  /// @brief チェック回数
  ymuint
  check_count() const;

  /// @brief チェック時間
  USTime
  check_time() const;

  /// @brief 成功回数
  ymuint
  found_count() const;

  /// @brief 統計データをクリアする．
  void
  clear_count();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード番号の最大値を返す．
  ymuint
  max_node_id() const;

  /// @brief 新しいグループを作る．
  /// @return グループを返す．
  FaultGroup*
  _new_group();

  /// @brief 故障を返す．
  /// @param[in] fid 故障番号
  const TpgFault*
  _fault(ymuint fid) const;

  /// @brief 故障の解析情報を返す．
  /// @param[in] fid 故障番号
  const FaultInfo&
  _fault_info(ymuint fid) const;

  /// @brief 故障グループを返す．
  /// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
  FaultGroup*
  _fault_group(ymuint gid);

  /// @brief 故障グループを返す．
  /// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
  const FaultGroup*
  _fault_group(ymuint gid) const;

  /// @brief 衝突キャッシュに登録する
  void
  add_conflict_cache(ymuint gid,
		     ymuint fid);

  /// @brief 衝突キャッシュを調べる．
  bool
  check_conflict_cache(ymuint gid,
		       ymuint fid);

  /// @brief [デバッグ用] sufficient_assignment が正しいか調べる．
  bool
  check_sufficient_assignment(ymuint gid);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  // 故障グループを表す構造体
  class FaultGroup
  {
  public:

    /// @brief コンストラクタ
    /// @param[in] id ID番号
    FaultGroup(ymuint id);

    /// @brief デストラクタ
    ~FaultGroup();


  public:
    //////////////////////////////////////////////////////////////////////
    // 外部インターフェイス
    //////////////////////////////////////////////////////////////////////

    /// @brief グループID番号を返す．
    ymuint
    id() const;

    /// @brief 故障数を返す．
    ymuint
    fault_num() const;

    /// @brief single cube でない故障数を返す．
    ymuint
    complex_fault_num() const;

    /// @brief 故障番号を返す．
    ymuint
    fault_id(ymuint pos) const;

    /// @brief 十分割当を返す．
    const NodeValList&
    sufficient_assignment() const;

    /// @brief 必要割当を返す．
    const NodeValList&
    mandatory_assignment() const;

    /// @brief 衝突キャッシュに登録する．
    void
    add_conflict_cache(ymuint fid);

    /// @brief 衝突キャッシュを調べる．
    bool
    check_conflict_cache(ymuint fid) const;

    /// @brief ID番号以外の内容をコピーする
    void
    copy(const FaultGroup& dst);

    /// @brief ID番号をセットする．
    void
    set_id(ymuint id);

    /// @brief 故障を追加する．
    void
    add_fault(ymuint fid,
	      const NodeValList& suf_list,
	      const NodeValList& ma_list);

    /// @brief 故障を削除する．
    void
    delete_faults(const vector<ymuint>& fid_list);

    /// @brief 故障の十分割当リストを設定する．
    void
    set_suf_list(ymuint pos,
		 const NodeValList& suf_list);

    /// @brief 故障リストが変更された時の更新処理を行う．
    void
    update();


  private:
    //////////////////////////////////////////////////////////////////////
    // 内部で用いられるデータ構造
    //////////////////////////////////////////////////////////////////////

    // 故障ごとの情報を表す構造体
    struct FaultData
    {
      // コンストラクタ
      FaultData(ymuint fid,
		const NodeValList& suf_list,
		const NodeValList& ma_list);

      // 故障番号
      ymuint mFaultId;

      // 十分割当リスト
      NodeValList mSufList;

      // 必要割当リスト
      NodeValList mMaList;

    };


  private:
    //////////////////////////////////////////////////////////////////////
    // データメンバ
    //////////////////////////////////////////////////////////////////////

    // グループ番号
    ymuint mId;

    // 故障データのリスト
    vector<FaultData> mFaultDataList;

    // single cube でない条件を持つ故障数
    ymuint mCplxNum;

    // 十分割当リスト
    NodeValList mSufList;

    // 必要割当リスト
    NodeValList mMaList;

    // 外部入力の十分割当リスト
    NodeValList mPiSufList;

    // 衝突する故障の集合
    HashSet<ymuint> mConflictCache;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号の最大値
  ymuint mMaxNodeId;

  // 故障解析器
  const FaultAnalyzer& mAnalyzer;

  // 故障グループの配列
  vector<FaultGroup*> mGroupList;

  ymuint mMnum;

  ymuint mFsum;

  ymuint mFmax;

  ymuint mCheckCount;

  ymuint mFoundCount;

  USTime mCheckTime;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ノード番号の最大値を返す．
inline
ymuint
FgMgrBase::max_node_id() const
{
  return mMaxNodeId;
}

// @brief 故障を取り除く
// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
// @param[in] fid_list 削除する故障番号のリスト
inline
void
FgMgrBase::delete_faults(ymuint gid,
			 const vector<ymuint>& fid_list)
{
  FaultGroup* fg = _fault_group(gid);
  fg->delete_faults(fid_list);
}

// @brief グループの故障数を返す．
// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
inline
ymuint
FgMgrBase::fault_num(ymuint gid) const
{
  const FaultGroup* fg = _fault_group(gid);
  return fg->fault_num();
}

// @brief グループの故障を返す．
// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
// @param[in] pos ( 0 <= pos < fault_num(gid) )
inline
ymuint
FgMgrBase::fault_id(ymuint gid,
		    ymuint pos) const
{
  const FaultGroup* fg = _fault_group(gid);
  return fg->fault_id(pos);
}

// @brief 十分割当リストを返す．
// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
inline
const NodeValList&
FgMgrBase::sufficient_assignment(ymuint gid) const
{
  const FaultGroup* fg = _fault_group(gid);
  return fg->sufficient_assignment();
}

// @brief 必要割当リストを返す．
// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
inline
const NodeValList&
FgMgrBase::mandatory_assignment(ymuint gid) const
{
  const FaultGroup* fg = _fault_group(gid);
  return fg->mandatory_assignment();
}

// @brief 衝突キャッシュに登録する
inline
void
FgMgrBase::add_conflict_cache(ymuint gid,
			      ymuint fid)
{
  FaultGroup* fg = _fault_group(gid);
  fg->add_conflict_cache(fid);
}

// @brief 衝突キャッシュを調べる．
inline
bool
FgMgrBase::check_conflict_cache(ymuint gid,
				ymuint fid)
{
  FaultGroup* fg = _fault_group(gid);
  return fg->check_conflict_cache(fid);
}

// @brief 故障を返す．
// @param[in] fid 故障番号
inline
const TpgFault*
FgMgrBase::_fault(ymuint fid) const
{
  return _fault_info(fid).fault();
}

// @brief 故障の解析情報を返す．
// @param[in] fid 故障番号
inline
const FaultInfo&
FgMgrBase::_fault_info(ymuint fid) const
{
  return mAnalyzer.fault_info(fid);
}

// @brief 故障グループを返す．
// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
inline
FgMgrBase::FaultGroup*
FgMgrBase::_fault_group(ymuint gid)
{
  ASSERT_COND( gid < group_num() );
  FaultGroup* fg = mGroupList[gid];
  ASSERT_COND( fg != nullptr );
  return fg;
}

// @brief 故障グループを返す．
// @param[in] gid グループ番号 ( 0 <= gid < group_num() )
inline
const FgMgrBase::FaultGroup*
FgMgrBase::_fault_group(ymuint gid) const
{
  ASSERT_COND( gid < group_num() );
  const FaultGroup* fg = mGroupList[gid];
  ASSERT_COND( fg != nullptr );
  return fg;
}

END_NAMESPACE_YM_SATPG_SA

#endif // FGMGRBASE_H
