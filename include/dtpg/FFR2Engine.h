#ifndef FFR2ENGINE_H
#define FFR2ENGINE_H

/// @file FFR2Engine.h
/// @brief FFR2Engine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/Bd2Engine.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFR2Engine FFR2Engine.h "FFR2Engine.h"
/// @brief 2つのFFRの故障伝搬を調べる Bd2Engine
/// @ingroup DtpgGroup
//////////////////////////////////////////////////////////////////////
class FFR2Engine:
  public Bd2Engine
{
public:

  /// @brief コンストラクタ
  FFR2Engine(
    const TpgFFR& ffr1,              ///< [in] FFR1
    const TpgFFR& ffr2,              ///< [in] FFR2
    const TpgFaultList& fault_list1, ///< [in] FFR1 の故障のリスト
    const TpgFaultList& fault_list2, ///< [in] FFR2 の故障のリスト
    const ConfigParam& option = {}   ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FFR2Engine() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFR1 の故障のリストを返す．
  const TpgFaultList&
  fault_list1() const
  {
    return mFaultList1;
  }

  /// @brief FFR2 の故障のリストを返す．
  const TpgFaultList&
  fault_list2() const
  {
    return mFaultList2;
  }

  /// @brief 2つの故障の検出条件を調べる．
  SatBool3
  solve(
    const TpgFault& fault1, ///< [in] fault_list1 の故障
    const TpgFault& fault2, ///< [in] fault_list2 の故障
    bool det1,              ///< [in] fault1 を検出する時 true にするフラグ
    bool det2               ///< [in] fault2 を検出する時 true にするフラグ
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFR1 の故障のリスト
  TpgFaultList mFaultList1;

  // 故障番号をキーにして mFaultList1 中の位置番号を格納する辞書
  std::unordered_map<SizeType, SizeType> mFaultMap1;

  // FFR2 の故障のリスト
  TpgFaultList mFaultList2;

  // 故障番号をキーにして mFaultList2 中の位置番号を格納する辞書
  std::unordered_map<SizeType, SizeType> mFaultMap2;

  // mFaultList1 の故障の検出条件のリテラルのリストの配列
  // キーは mFaultList1 の位置番号
  std::vector<std::vector<SatLiteral>> mDLitsArray1;

  // mFaultList1 の故障の非検出条件のリテラルの配列
  // キーは mFaultList1 の位置番号
  std::vector<SatLiteral> mULitArray1;

  // mFaultList2 の故障の検出条件のリテラルのリストの配列
  // キーは mFaultList2 の位置番号
  std::vector<std::vector<SatLiteral>> mDLitsArray2;

  // mFaultList2 の故障の非検出条件のリテラルの配列
  // キーは mFaultList2 の位置番号
  std::vector<SatLiteral> mULitArray2;

};

END_NAMESPACE_DRUID

#endif // FFR2ENGINE_H
