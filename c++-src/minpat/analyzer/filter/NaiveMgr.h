#ifndef NAIVEMGR_H
#define NAIVEMGR_H

/// @file NaiveMgr.h
/// @brief NaiveMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqGroupMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NaiveMgr NaiveMgr.h "NaiveMgr.h"
/// @brief 単純な EqGroupMgr
//////////////////////////////////////////////////////////////////////
class NaiveMgr :
  public EqGroupMgr
{
public:

  /// @brief コンストラクタ
  NaiveMgr(
    FaultInfo& fault_info,    ///< [in] 故障の情報
    Fsim& fsim,               ///< [in] 故障シミュレータ
    const ConfigParam& option ///< [in] オプション
  );

  /// @brief デストラクタ
  ~NaiveMgr();


private:
  //////////////////////////////////////////////////////////////////////
  // Filter の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 更新処理
  bool
  update(
    const std::vector<DPat>& dpat_array ///< [in] 故障の検出状況のピットパタンの配列
  ) override;

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

  /// @brief 等価グループを求める．
  void
  _make_group() const;

  /// 故障対に対するインデックスを計算する．
  SizeType
  _index(
    const TpgFault& fault1,
    const TpgFault& fault2
  ) const
  {
    return fault1.id() * mSize + fault2.id();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障グループの情報を表す構造体
  struct Group {
    // グループ番号
    SizeType mId;
    // 故障リスト
    TpgFaultList mFaultList;
    // 後続グループのリスト
    std::vector<SizeType> mSuccList;
    // 先行グループのリスト
    std::vector<SizeType> mPredList;
  };

  // 故障番号の最大値
  SizeType mSize;

  // 2つの故障間の関係を表すビットパタンの配列
  // サイズは mSize * mSize
  // 0: 故障1が故障2を支配する可能性なし(1, 0 のパタンあり)
  std::vector<bool> mArray;

  // 支配する故障候補のリストの配列
  // サイズは mSize
  std::vector<TpgFaultList> mDomCandListArray;

  // 等価故障グループの配列
  mutable
  std::vector<Group> mGroupArray;

  // 故障番号をキーにしてグループ番号を格納する配列
  mutable
  std::vector<SizeType> mIdMap;

  // mDomCandListArray が初期化されていたら true となるフラグ
  bool mInitialized{false};

  // 等価グループが計算済みなら true となるフラグ
  mutable
  bool mHasGroup{false};

};

END_NAMESPACE_DRUID

#endif // NAIVEMGR_H
