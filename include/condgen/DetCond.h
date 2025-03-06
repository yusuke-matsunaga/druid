#ifndef DETCOND_H
#define DETCOND_H

/// @file DetCond.h
/// @brief DetCond のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/Literal.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DetCond DetCond.h "DetCond.h"
/// @brief 故障検出条件を表すクラス
///
/// mandatory_condition & (cube_1 | cube_2 | ... | cube_n)
/// の形で表される．
/// mandatory_condition, cube_i はそれぞれ AssignList で表される．
//////////////////////////////////////////////////////////////////////
class DetCond
{
public:

  /// @brief 結果の種類
  enum Type: std::uint8_t {
    Detected,        ///< 検出された．
    PartialDetected, ///< 部分的に検出された．
    Overflow,        ///< 繰り返し回数をオーバーした．
    Undetected       ///< 検出できなかった．
  };

  /// @brief 条件を表す構造体
  struct CondData {
    vector<Literal> mand_cond;         ///< 必要条件
    vector<vector<Literal>> cube_list; ///< 十分条件のリスト
  };


public:

  /// @brief 空のコンストラクタ
  ///
  /// Undetected になる．
  DetCond() = default;

  /// @brief Detected タイプを返すクラスメソッド
  static
  DetCond
  detected(
    const CondData& cond
  )
  {
    return DetCond(cond);
  }

  /// @brief PartialDetected を返すクラスメソッド
  static
  DetCond
  partial_detected(
    const vector<CondData>& cond_list,
    const vector<const TpgNode*>& output_list = {}
  )
  {
    return DetCond(cond_list, output_list);
  }

  /// @brief Overflow を返すクラスメソッド
  static
  DetCond
  overflow(
    const vector<const TpgNode*>& output_list
  )
  {
    return DetCond(output_list);
  }

  /// @brief Undetected を返すクラスメソッド
  static
  DetCond
  undetected()
  {
    return DetCond();
  }

  /// @brief デストラクタ
  ~DetCond() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief タイプを返す．
  Type
  type() const
  {
    return mType;
  }

  /// @brief 全体の条件を得る．
  const CondData&
  cond() const
  {
    return mCond;
  }

  /// @brief 個々の出力ごとの条件を得る．
  const vector<CondData>&
  cond_list() const
  {
    return mCondList;
  }

  /// @brief オーバーフローした出力のリスト
  const vector<const TpgNode*>&
  output_list() const
  {
    return mOutputList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief Detected 用のコンストラクタ
  DetCond(
    const CondData& cond
  ) : mType{Detected},
      mCond{cond}
  {
  }

  /// @brief PartialDetected 用のコンストラクタ
  DetCond(
    const vector<CondData>& cond_list,
    const vector<const TpgNode*>& output_list
  ) : mType{PartialDetected},
      mCondList{cond_list},
      mOutputList{output_list}
  {
  }

  /// @brief Overflow 用のコンストラクタ
  DetCond(
    const vector<const TpgNode*>& output_list
  ) : mType{Overflow},
      mOutputList{output_list}
  {
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // タイプ
  Type mType;

  // 条件
  CondData mCond;

  // 個々の出力ごとの条件
  vector<CondData> mCondList;

  // overflow した出力のリスト
  vector<const TpgNode*> mOutputList;

};

END_NAMESPACE_DRUID

#endif // DETCOND_H
