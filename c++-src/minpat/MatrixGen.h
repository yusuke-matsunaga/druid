#ifndef MATRIXGEN_H
#define MATRIXGEN_H

/// @file MatrixGen.h
/// @brief MatrixGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Fsim.h"
#include "ym/McMatrix.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MatrixGen MatrixGen.h "MatrixGen.h"
/// @brief 被覆行列を作るクラス．
//////////////////////////////////////////////////////////////////////
class MatrixGen
{
public:

  /// @brief コンストラクタ
  MatrixGen(
    const vector<const TpgFault*>& fault_list, ///< [in] 故障のリスト
    const vector<TestVector>& tv_list,	       ///< [in] テストパタンのリスト
    const TpgNetwork& network,		       ///< [in] ネットワーク
    FaultType fault_type		       ///< [in] 故障の種類
  );

  /// @brief デストラクタ
  ~MatrixGen();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 被覆行列を作る．
  McMatrix
  generate();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障シミュレーションを行い，被覆行列に要素を設定する．
  void
  do_fsim(
    McMatrix& matrix, ///< [in] 対象の行列
    int tv_base,      ///< [in] テストベクタ番号の基点
    int num	      ///< [in] セットしたパタン数
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障リスト
  const vector<const TpgFault*>& mFaultList;

  // テストベクタのリスト
  const vector<TestVector>& mTvList;

  // 故障番号から行番号への写像
  // サイズは network.max_fault_id()
  vector<int> mRowIdMap;

  // 故障シミュレータ
  Fsim mFsim;

};

END_NAMESPACE_DRUID

#endif // MATRIXGEN_H
