#ifndef TPGFAULTMGRIMPL_H
#define TPGFAULTMGRIMPL_H

/// @file TpgFaultMgrImpl.h
/// @brief TpgFaultMgrImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultType.h"
#include "FaultStatus.h"


BEGIN_NAMESPACE_DRUID

class TpgFaultImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgFaultMgrImpl TpgFaultMgrImpl.h "TpgFaultMgrImpl.h"
/// @brief TpgFaultMgr の実装クラス
//////////////////////////////////////////////////////////////////////
class TpgFaultMgrImpl
{
public:

  /// @brief 派生クラスを生成するクラスメソッド
  static
  unique_ptr<TpgFaultMgrImpl>
  new_obj(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    FaultType fault_type       ///< [in] 故障の種類
  );

  /// @brief コンストラクタ
  TpgFaultMgrImpl(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief デストラクタ
  virtual
  ~TpgFaultMgrImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障のタイプを返す．
  virtual
  FaultType
  fault_type() const = 0;

  /// @brief 全代表故障のリストを返す．
  const vector<SizeType>&
  rep_fault_list() const
  {
    return mRepFaultList;
  }

  /// @brief ノードに関係する故障のリストを返す．
  const vector<SizeType>&
  node_fault_list(
    SizeType node_id ///< [in] ノード番号
  ) const
  {
    return mNodeFaultList[node_id];
  }

  /// @brief FFR に関係する故障のリストを返す．
  const vector<SizeType>&
  ffr_fault_list(
    SizeType ffr_id ///< [in] FFR番号
  ) const
  {
    return mFFRFaultList[ffr_id];
  }

  /// @brief MFFC に関係する故障のリストを返す．
  const vector<SizeType>&
  mffc_fault_list(
    SizeType mffc_id ///< [in] FFR番号
  ) const
  {
    return mMFFCFaultList[mffc_id];
  }

  /// @brief 故障の状態をセットする．
  void
  set(
    SizeType id,       ///< [in] 故障番号
    FaultStatus status ///< [in] 故障の状態
  )
  {
    ASSERT_COND( 0 <= id && id < fault_num() );

    mStatusArray[id] = status;
  }

  /// @brief 故障の状態を得る．
  FaultStatus
  get(
    SizeType id ///< [in] 故障番号
  ) const
  {
    ASSERT_COND( 0 <= id && id < fault_num() );

    return mStatusArray[id];
  }

  /// @brief 全故障数を得る．
  SizeType
  fault_num() const
  {
    return mFaultArray.size();
  }

  /// @brief 故障を得る．
  TpgFaultImpl*
  _fault(
    SizeType id ///< [in] 故障番号
  )
  {
    ASSERT_COND( 0 <= id && id < fault_num() );

    return mFaultArray[id];
  }


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障リストを作る．
  void
  gen_all_faults(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief 故障を登録する．
  void
  reg_fault(
    TpgFaultImpl* fault, ///< [in] 故障
    bool rep             ///< [in] 代表故障の時 true にする．
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが実装する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障リストを作る．
  virtual
  void
  _gen_all_faults(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  ) = 0;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 全故障のリスト
  vector<TpgFaultImpl*> mFaultArray;

  // 全代表故障のリスト
  vector<SizeType> mRepFaultList;

  // ノード番号をキーにした故障リストの配列
  vector<vector<SizeType>> mNodeFaultList;

  // FFR番号をキーにした故障リストの配列
  vector<vector<SizeType>> mFFRFaultList;

  // MFFC番号をキーにした故障リストの配列
  vector<vector<SizeType>> mMFFCFaultList;

  // 各故障の状態を保持する配列
  // サイズは mFaultArray と同一
  vector<FaultStatus> mStatusArray;

};

END_NAMESPACE_DRUID

#endif // TPGFAULTMGRIMPL_H
