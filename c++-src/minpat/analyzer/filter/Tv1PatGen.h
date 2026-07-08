#ifndef TV1PATGEN_H
#define TV1PATGEN_H

/// @file Tv1PatGen.h
/// @brief Tv1PatGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "PatGen.h"
#include <random>


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Tv1PatGen Tv1PatGen.h "Tv1PatGen.h"
/// @brief 未検出の故障のテストパタンを優先する PatGen の派生クラス
//////////////////////////////////////////////////////////////////////
class Tv1PatGen :
  public PatGen
{
public:

  /// @brief コンストラクタ
  Tv1PatGen(
    const FaultInfo& fault_info ///< [in] 故障情報を持つオブジェクト
  );

  /// @brief デストラクタ
  ~Tv1PatGen() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief パタンを作る．
  void
  gen(
    SizeType size,                   ///< [in] パタンのサイズ
    std::vector<TestVector>& tv_buff ///< [in] 生成したパタンを格納するオブジェクト
  ) override;

  /// @brief 検出結果で更新する．
  void
  update(
    const FsimResults& res ///< [in] 故障シミュレーションの結果
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 乱数生成器
  std::mt19937 mRandGen;

  // 未検出の故障リスト
  TpgFaultList mUndetList;

};

END_NAMESPACE_DRUID

#endif // TV1PATGEN_H
