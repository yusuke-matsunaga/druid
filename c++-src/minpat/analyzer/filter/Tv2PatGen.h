#ifndef TV2PATGEN_H
#define TV2PATGEN_H

/// @file Tv2PatGen.h
/// @brief Tv2PatGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "PatGen.h"
#include "ym/HeapTree.h"
#include <random>


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Tv2PatGen Tv2PatGen.h "Tv2PatGen.h"
/// @brief 未検出の故障のテストパタンを優先する PatGen の派生クラス
//////////////////////////////////////////////////////////////////////
class Tv2PatGen :
  public PatGen
{
public:

  /// @brief コンストラクタ
  Tv2PatGen(
    const FaultInfo& fault_info ///< [in] 故障情報を持つオブジェクト
  );

  /// @brief デストラクタ
  ~Tv2PatGen() = default;


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

  // ヒープに格納する情報
  struct Cell {
    TpgFault mFault;    // 故障
    SizeType mDetCount; // 検出回数

    bool
    operator<(
      const Cell& right
    ) const
    {
      return mDetCount < right.mDetCount;
    }

    bool
    operator>(
      const Cell& right
    ) const
    {
      return mDetCount > right.mDetCount;
    }

  };

  // 乱数生成器
  std::mt19937 mRandGen;

  // 検出回数をキーにした故障のヒープ木
  HeapTree<Cell> mHeap;

};

END_NAMESPACE_DRUID

#endif // TV2PATGEN_H
