#ifndef MATRIXGEN_H
#define MATRIXGEN_H

/// @file MatrixGen.h
/// @brief MatrixGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
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
  /// @param[in] fault_list 故障のリスト
  /// @param[in] tv_list テストパタンのリスト
  /// @param[in] network ネットワーク
  /// @param[in] fault_type 故障の種類
  MatrixGen(const vector<const TpgFault*>& fault_list,
	    const vector<TestVector>& tv_list,
	    const TpgNetwork& network,
	    FaultType fault_type);

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
  /// @param[in] matrix 対象の行列
  /// @param[in] tv_base テストベクタ番号の基点
  /// @param[in] num セットしたパタン数
  void
  do_fsim(McMatrix& matrix,
	  int tv_base,
	  int num);


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


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_DRUID

#endif // MATRIXGEN_H
