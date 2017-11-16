﻿#ifndef MINPATBASE_H
#define MINPATBASE_H

/// @file MinPatBase.h
/// @brief MinPatBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2013-2014, 2015 Yusuke Matsunaga
/// All rights reserved.


#include "sa/MinPat.h"
#include "FaultAnalyzer.h"


BEGIN_NAMESPACE_YM_SATPG_SA

class FgMgr;

//////////////////////////////////////////////////////////////////////
/// @class MinPatBase MinPatBase.h "MinPatBase.h"
/// @brief MinPat の実装クラス
//////////////////////////////////////////////////////////////////////
class MinPatBase :
  public MinPat
{
public:

  /// @brief コンストラクタ
  /// @param[in] group_dominance グループ支配を計算する．
  MinPatBase(bool group_dominance = false);

  /// @brief デストラクタ
  virtual
  ~MinPatBase();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストベクタの最小化を行なう．
  /// @param[in] network 対象のネットワーク
  /// @param[in] fmgr 故障マネージャ
  /// @param[in] tvmgr テストベクタマネージャ
  /// @param[in] fsim2 2値の故障シミュレータ(検証用)
  /// @param[in] fsim3 3値の故障シミュレータ
  /// @param[in] exact 故障グループの両立性判定を厳密に行うときに true とする．
  /// @param[in] compaction 最後に圧縮を行うときに true とする．
  /// @param[in] fast_compaction 最後に高速圧縮を行うときに true とする．
  /// @param[in] mc_compaction 最後に最小被覆圧縮を行うときに true とする．
  /// @param[out] tv_list テストベクタのリスト
  /// @param[out] stats 実行結果の情報を格納する変数
  virtual
  void
  run(TpgNetwork& network,
      TpgFaultMgr& fmgr,
      TvMgr& tvmgr,
      Fsim& fsim2,
      Fsim& fsim3,
      bool exact,
      bool compaction,
      bool fast_compaction,
      bool mc_compaction,
      bool has_thval,
      ymuint thval,
      vector<const TestVector*>& tv_list,
      USTime& time);

  /// @brief verbose フラグをセットする．
  virtual
  void
  set_verbose(int verbose);

  /// @brief verbose フラグを得る．
  int
  verbose() const;


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  /// @param[in] fid_list 故障番号のリスト
  /// @param[in] tvmgr テストベクタマネージャ
  /// @param[in] fsim2 2値の故障シミュレータ(検証用)
  virtual
  void
  init(const vector<ymuint>& fault_list,
       TvMgr& tvmgr,
       Fsim& fsim2) = 0;

  /// @brief 対象の全故障数を返す．
  virtual
  ymuint
  fault_num() = 0;

  /// @brief 故障番号のリストを返す．
  virtual
  const vector<ymuint>&
  fid_list() = 0;

  /// @brief 最初の故障を選ぶ．
  virtual
  ymuint
  get_first_fault() = 0;

  /// @brief 次に処理すべき故障を選ぶ．
  /// @param[in] fgmgr 故障グループを管理するオブジェクト
  /// @param[in] group_list 現在のグループリスト
  virtual
  ymuint
  get_next_fault(FgMgr& fgmgr,
		 const vector<ymuint>& group_list) = 0;

  /// @brief 故障を追加するグループを選ぶ．
  /// @param[in] fgmgr 故障グループを管理するオブジェクト
  /// @param[in] fid 故障番号
  /// @param[in] group_list 現在のグループリスト
  ///
  /// グループが見つからなければ fgmgr.group_num() を返す．
  /// デフォルト実装では group_list のなかで最初にヒットしたものを返す．
  virtual
  ymuint
  find_group(FgMgr& fgmgr,
	     ymuint fid,
	     const vector<ymuint>& group_list);

  /// @brief 故障解析器を返す．
  FaultAnalyzer&
  analyzer();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で使われる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンを作る．
  /// @param[in] network ネットワーク
  /// @param[in] suf_list 十分割当リスト
  /// @param[in] tv テストベクタ
  void
  make_testvector(TpgNetwork& network,
		  const NodeValList& suf_list,
		  TestVector* tv);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号の最大値＋１
  ymuint mMaxNodeId;

  // verbose フラグ
  int mVerbose;

  // group dominance フラグ
  bool mGroupDominance;

  // fast フラグ
  bool mFast;

  // 故障解析器
  FaultAnalyzer mAnalyzer;

};

END_NAMESPACE_YM_SATPG_SA

#endif // MINPATBASE_H
