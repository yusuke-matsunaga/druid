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

  /// @brief コンストラクタ
  SimThrFunc(
    SizeType id,                          ///< [in] ID番号
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const TpgFaultList& fault_list,       ///< [in] 対象の故障リスト
    const std::vector<SizeType>& ffr_list ///< [in] 担当する FFR の番号のリスト
    = {}
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
  ///
  /// 結果は mDetListArray[0] に格納される．
  void
  sppfp();

  /// @brief PPSFP 法のシミュレーションを行う．
  ///
  /// 結果は mDetListArray に格納される．
  void
  ppsfp(
    SizeType tv_num ///< [in] テストベクタ数
  );

  /// @brief SPPFP2 法のシミュレーションを行う．
  ///
  /// 結果は mResArray[0] に格納される．
  void
  sppfp2();

  /// @brief PPSFP2 法のシミュレーションを行う．
  ///
  /// 結果は mResArray に格納される．
  void
  ppsfp2(
    SizeType tv_num ///< [in] テストベクタ数
  );

  /// @brief SPPFP の結果を得る．
  const std::vector<SizeType>&
  det_list() const
  {
    return mDetListArray[0];
  }

  /// @brief PPSFP の結果を得る．
  const std::vector<SizeType>&
  det_list(
    SizeType tv_id ///< [in] テストベクタ番号
  ) const
  {
    return mDetListArray[tv_id];
  }

  /// @brief SPPFP2 の結果を得る．
  FsimResultsRep*
  diffbits_list() const
  {
    return mResArray[0];
  }

  /// @brief PPSFP2 の結果を得る．
  FsimResultsRep*
  diffbits_list(
    SizeType tv_id ///< [in] テストベクタ番号
  ) const
  {
    return mResArray[tv_id];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  SizeType mId;

  // シミュレーションエンジン
  SimEngine mEngine;

  // SPPFP/PPSFP の結果
  std::vector<std::vector<SizeType>> mDetListArray;

  // SPPFP2/PPSFP2 の結果
  std::vector<FsimResultsRep*> mResArray;

};

END_NAMESPACE_DRUID_FSIM

#endif // SIMTHRFUNC_H
