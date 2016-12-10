﻿#ifndef ATPGCMD_H
#define ATPGCMD_H

/// @file AtpgCmd.h
/// @brief AtpgCmd のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "sa/sa_nsdef.h"
#include "td/td_nsdef.h"
#include "ym/TclCmd.h"
#include "ym/USTime.h"


BEGIN_NAMESPACE_YM_SATPG

// 前方参照のためのクラス宣言
class AtpgMgr;

//////////////////////////////////////////////////////////////////////
// ATPG のコマンドオブジェクトの基底クラス
// おもな仕事はエラーが起きた時にメッセージをセット
// するような関数の殻をかぶせること．
// あとは変換などの便利関数を入れてある．
//////////////////////////////////////////////////////////////////////
class AtpgCmd :
  public TclCmd
{
public:

  /// @brief コンストラクタ
  AtpgCmd(AtpgMgr* mgr);

  /// @brief デストラクタ
  virtual
  ~AtpgCmd();


protected:

  /// @brief ネットワークをセットした後に呼ぶ関数
  void
  after_set_network();

  /// @brief 故障リストを更新した後に呼ぶ関数
  void
  after_update_faults();


protected:

  /// @brief TgNetwork を取り出す．
  TpgNetwork&
  _network();

  /// @brief FaultMgr を取り出す．
  FaultMgr&
  _fault_mgr();

  /// @brief TvMgr を取り出す．
  nsSa::TvMgr&
  _sa_tv_mgr();

  /// @brief テストベクタのリストを取り出す．
  vector<nsSa::TestVector*>&
  _sa_tv_list();

  /// @brief 2値の故障シミュレータを取り出す．
  nsSa::Fsim&
  _sa_fsim();

  /// @brief 3値の故障シミュレータを返す．
  nsSa::Fsim&
  _sa_fsim3();

  /// @brief TvMgr を取り出す．
  nsTd::TvMgr&
  _td_tv_mgr();

  /// @brief テストベクタのリストを取り出す．
  vector<nsTd::TestVector*>&
  _td_tv_list();

  /// @brief 遷移故障用の2値の故障シミュレータを取り出す．
  nsTd::Fsim&
  _td_fsim();

  /// @brief ファイル読み込みに関わる時間を得る．
  USTime
  read_time() const;

  /// @brief DTPG に関わる時間を得る．
  USTime
  dtpg_time() const;

  /// @brief 故障シミュレーションに関わる時間を得る．
  USTime
  fsim_time() const;

  /// @brief SAT に関わる時間を得る．
  USTime
  sat_time() const;

  /// @brief その他の時間を得る．
  USTime
  misc_time() const;


private:
  //////////////////////////////////////////////////////////////////////
  // メンバ変数
  //////////////////////////////////////////////////////////////////////

  // 共通のデータを管理するオブジェクト
  AtpgMgr* mMgr;

};

END_NAMESPACE_YM_SATPG

#endif // ATPGCMD_H
