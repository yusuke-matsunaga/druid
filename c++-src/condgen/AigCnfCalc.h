#ifndef AIGCNFCALC_H
#define AIGCNFCALC_H

/// @file AigCnfCalc.h
/// @brief AigCnfCalc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/AigHandle.h"
#include "ym/CnfSize.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class AigCnfCalc AigCnfCalc.h "AigCnfCalc.h"
/// @brief AIG を CNF に変換した時のサイズを見積もるクラス
//////////////////////////////////////////////////////////////////////
class AigCnfCalc
{
public:

  /// @brief コンストラクタ
  AigCnfCalc()
  {
    mSize = CnfSize::zero();
  }

  /// @brief デストラクタ
  ~AigCnfCalc() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief AIG を CNF に変換した時のサイズを計算する．
  /// @return aig に対応するリテラル数を返す．
  SizeType
  calc_size(
    const AigHandle& aig
  );

  /// @brief 結果のサイズを得る．
  CnfSize
  size() const
  {
    return mSize;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 現在のサイズ
  CnfSize mSize;

  // AigHandle をキーにして対応するリテラル数を記憶する辞書
  std::unordered_map<AigHandle, SizeType> mAigDict;

};

END_NAMESPACE_DRUID

#endif // AIGCNFCALC_H
