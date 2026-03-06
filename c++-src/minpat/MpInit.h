#ifndef MPINIT_H
#define MPINIT_H

/// @file MpInit.h
/// @brief MpInit のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpInit MpInit.h "MpInit.h"
/// @brief 最小テストパタン生成用の初期解を作るクラス
///
/// 検出された故障に関する情報も持つ．
//////////////////////////////////////////////////////////////////////
class MpInit
{
public:

  /// @brief 派生クラスのオブジェクトを生成するクラスメソッド
  static
  std::unique_ptr<MpInit>
  new_obj(
    const std::string& type,  ///< [in] 種類を表す文字列
    const TpgNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief コンストラクタ
  MpInit(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  ) : mNetwork{network}
  {
  }

  /// @brief デストラクタ
  virtual
  ~MpInit() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンのリストを求める．
  virtual
  std::vector<TestVector>
  run(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const JsonValue& option = {}    ///< [in] オプション
  ) = 0;

  /// @brief ネットワークを得る．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 検出された故障リストを得る．
  const TpgFaultList&
  fault_list() const
  {
    return mFaultList;
  }

  /// @brief fault_list() 中の位置を返す．
  SizeType
  find(
    const TpgFault& fault ///< [in] 対象の故障
  ) const
  {
    return mFidMap.at(fault.id());
  }


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 検出された故障を追加する．
  void
  add_fault(
    const TpgFault& fault ///< [in] 故障
  )
  {
    auto pos = mFaultList.size();
    mFaultList.push_back(fault);
    mFidMap.emplace(fault.id(), pos);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  TpgNetwork mNetwork;

  // 検出された故障のリスト
  TpgFaultList mFaultList;

  // 故障番号をキーとして mFaultList 中の位置を格納した辞書
  std::unordered_map<SizeType, SizeType> mFidMap;

};

END_NAMESPACE_DRUID

#endif // MPINIT_H
