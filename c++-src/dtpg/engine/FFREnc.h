#ifndef FFRENC_H
#define FFRENC_H

/// @file FFREnc.h
/// @brief FFREnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BoolDiffEnc.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFREnc FFREnc.h "FFREnc.h"
/// @brief FFR内の故障検出条件を扱うエンコーダ
/// @ingroup DtpgGroup
/// @sa StructEngine
///
/// 各故障ごとに検出条件を表すリテラルのリストと非検出条件を
/// あらわすリテラルを持つ．
//////////////////////////////////////////////////////////////////////
class FFREnc :
  public BoolDiffEnc
{
public:

  /// @brief コンストラクタ
  FFREnc(
    const TpgFFR& ffr,              ///< [in] FFR
    const TpgFaultList& fault_list, ///< [in] ffr の故障のリスト
    const ConfigParam& option = {}  ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FFREnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 条件の生成を行う．
  ///
  /// SubEnc の初期化の都合であとから実行する必要がある．
  void
  make_cond();

  /// @brief 故障の検出条件を表すリテラルのリストを返す．
  const std::vector<SatLiteral>&
  dlits(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief 故障の非検出条件を表すリテラルを返す．
  SatLiteral
  ulit(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // SubEnc の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 名前を返す．
  std::string
  name() override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障のローカルな番号を得る．
  SizeType
  local_id(
    const TpgFault& fault ///< [in] 対象の故障
  ) const
  {
    if ( mFaultMap.count(fault.id()) == 0 ) {
      throw std::out_of_range{"fault is not registered"};
    }
    return mFaultMap.at(fault.id());
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の FFR
  TpgFFR mFFR;

  // 故障のリスト
  TpgFaultList mFaultList;

  // 根の故障伝搬条件を表す変数
  SatLiteral mPropVar;

  // 故障番号をキーにして mFaultList 中の位置番号を格納する辞書
  std::unordered_map<SizeType, SizeType> mFaultMap;

  // mFaultList の故障の検出条件のリテラルのリストの配列
  // キーは mFaultList の位置番号
  std::vector<std::vector<SatLiteral>> mDLitsArray;

  // mFaultList の故障の非検出条件のリテラルの配列
  // キーは mFaultList の位置番号
  std::vector<SatLiteral> mULitArray;

};

END_NAMESPACE_DRUID

#endif // FFRENC_H
