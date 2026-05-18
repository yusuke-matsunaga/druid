#ifndef SIMTHRFUNC_H
#define SIMTHRFUNC_H

/// @file SimThrFunc.h
/// @brief SimThrFunc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "FsimX.h"
#include "SimEngine.h"
#include "DiffBitsArray.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SimThrFunc SimThrFunc.h "SimThrFunc.h"
/// @brief スレッド実行の本体
///
/// 中身はほぼ SimEngine だが結果を保持しておく必要があるのでこの
/// クラスを用いる．
//////////////////////////////////////////////////////////////////////
class SimThrFunc
{
public:

  /// @brief 故障番号のリストの型
  using FidList = std::vector<SizeType>;

  /// @brief 出力ごとの故障伝搬状況の辞書の型
  using DiffBitsDict = std::unordered_map<SizeType, DiffBits>;


public:

  /// @brief コンストラクタ
  SimThrFunc(
    SizeType id,                   ///< [in] ID番号
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const TpgFaultList& fault_list ///< [in] 対象の故障リスト
  );

  /// @brief デストラクタ
  ~SimThrFunc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を得る．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief エンジンを返す．
  SimEngine&
  engine()
  {
    return mEngine;
  }

  /// @brief SPPFP 法のシミュレーションを行う．
  void
  sppfp(
    const TestVector& tv,
    FidList& fid_list
  );

  /// @brief SPPFP 法のシミュレーションを行う．
  void
  sppfp(
    const AssignList& assign_list,
    FidList& fid_list
  );

  /// @brief SPPFP 法のシミュレーションを行う．
  void
  sppfp(
    const std::vector<TestVector>& tv_list, ///< [in] テストベクタのリスト
    SizeType begin,                         ///< [in] 開始位置
    SizeType end,                           ///< [in] 終了位置
    std::vector<FidList>& det_list_array    ///< [in] 結果を格納する配列
  );

  /// @brief PPSFP 法のシミュレーションを行う．
  void
  ppsfp(
    const std::vector<TestVector>& tv_list, ///< [in] テストベクタのリスト
    SizeType begin,                         ///< [in] 開始位置
    SizeType end,                           ///< [in] 終了位置
    std::vector<FidList>& det_list_array    ///< [in] 結果を格納する配列
  );

  /// @brief SPPFP 法のシミュレーションを行う．
  void
  sppfp2(
    const TestVector& tv,    ///< [in] テストベクタ
    FidList& fid_list,       ///< [in] 検出された故障番号のリスト
    DiffBitsDict& dbits_dict ///< [in] 出力ごとの故障伝搬状況の辞書
  );

  /// @brief SPPFP 法のシミュレーションを行う．
  void
  sppfp2(
    const AssignList& assign_list, ///< [in] 値割り当てのリスト
    FidList& fid_list,             ///< [in] 検出された故障番号のリスト
    DiffBitsDict& dbits_dict       ///< [in] 出力ごとの故障伝搬状況の辞書
  );

  /// @brief PPSFP2 法のシミュレーションを行う．
  void
  sppfp2(
    const std::vector<TestVector>& tv_list,     ///< [in] テストベクタのリスト
    SizeType begin,                             ///< [in] 開始位置
    SizeType end,                               ///< [in] 終了位置
    std::vector<FidList>& fid_list_array,       ///< [in] 検出された故障番号のリストの配列
    std::vector<DiffBitsDict>& dbits_dict_array ///< [in] 出力ごとの伝搬状況の辞書の配列
  );

  /// @brief PPSFP2 法のシミュレーションを行う．
  void
  ppsfp2(
    const std::vector<TestVector>& tv_list,     ///< [in] テストベクタのリスト
    SizeType begin,                             ///< [in] 開始位置
    SizeType end,                               ///< [in] 終了位置
    std::vector<FidList>& fid_list_array,       ///< [in] 検出された故障番号のリストの配列
    std::vector<DiffBitsDict>& dbits_dict_array ///< [in] 出力ごとの伝搬状況の辞書の配列
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  SizeType mId;

  // シミュレーションエンジン
  SimEngine mEngine;

};

END_NAMESPACE_DRUID_FSIM

#endif // SIMTHRFUNC_H
