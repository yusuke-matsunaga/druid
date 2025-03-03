#ifndef LITMAP_H
#define LITMAP_H

/// @file LitMap.h
/// @brief LitMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class LitMap LitMap.h "LitMap.h"
/// @brief SatLiteral と変数番号の対応付けを行うクラス
///
/// SatLiteral::varid() を使ってもよいが，内部で生成されたリテラルを
/// 含むので無用な番号を多く含んでいる.
/// このクラスでは登録されたリテラルのみに番号を振っている．
//////////////////////////////////////////////////////////////////////
class LitMap
{
public:

  /// @brief コンストラクタ
  LitMap() = default;

  /// @brief デストラクタ
  ~LitMap() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief リテラルを登録する．
  /// @return 変数番号を返す．
  ///
  /// - リテラルの極性は無視する．
  /// - すでに登録済みの場合はその番号を返す．
  SizeType
  reg_lit(
    SatLiteral lit
  )
  {
    if ( mVarDict.count(lit) == 0 ) {
      auto id = mLitArray.size();
      mVarDict.emplace(lit, id);
      mLitArray.push_back(lit);
      return id;
    }
    return mVarDict.at(lit);
  }

  /// @brief 変数番号を得る．
  ///
  /// リテラルが登録されていない場合は例外を送出する．
  SizeType
  varid(
    SatLiteral lit
  ) const
  {
    if ( mVarDict.count(lit) == 0 ) {
      throw std::invalid_argument{"lit is not registered"};
    }
    return mVarDict.at(lit);
  }

  /// @brief 変数の数を得る．
  SizeType
  var_num() const
  {
    return mLitArray.size();
  }

  /// @brief リテラルを得る．
  ///
  /// 変数番号が範囲外の場合は例外を送出する．
  SatLiteral
  literal(
    SizeType varid
  ) const
  {
    if ( varid >= var_num() ) {
      throw std::out_of_range{"varid is out of range"};
    }
    return mLitArray[varid];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SatLiteral をキーにして変数番号を記録する辞書
  std::unordered_map<SatLiteral, SizeType> mVarDict;

  // 変数番号をキーにして SatLiteral を記録する配列
  std::vector<SatLiteral> mLitArray;

};

END_NAMESPACE_DRUID

#endif // LITMAP_H
