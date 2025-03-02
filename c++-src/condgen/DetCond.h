#ifndef DETCOND_H
#define DETCOND_H

/// @file DetCond.h
/// @brief DetCond のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "AssignList.h"


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
    Detected,
    Overflow,
    Undetected
  };


public:

  /// @brief 空のコンストラクタ
  DetCond() = default;

  /// @brief 内容を指定したコンストラクタ
  DetCond(
    const AssignList& mandatory_condition, ///< [in] 必要条件
    const vector<AssignList>& cube_list    ///< [in] キューブのリスト
    = {}
  ) : mType{Detected},
      mMandatoryCondition{mandatory_condition},
      mCubeList{cube_list}
  {
  }

  /// @brief Overflow を返すクラスメソッド
  static
  DetCond
  overflow()
  {
    return DetCond(Overflow);
  }

  /// @brief Undetected を返すクラスメソッド
  static
  DetCond
  undetected()
  {
    return DetCond(Undetected);
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

  /// @brief 条件が空の時 true を返す．
  bool
  empty() const
  {
    return mMandatoryCondition.size() == 0 && mCubeList.empty();
  }

  /// @brief 必要条件を得る．
  const AssignList&
  mandatory_condition() const
  {
    return mMandatoryCondition;
  }

  /// @brief キューブのリストを得る．
  const vector<AssignList>&
  cube_list() const
  {
    return mCubeList;
  }


private:

  /// @brief 種類を指定したコンストラクタ
  DetCond(
    Type type
  ) : mType{type}
  {
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // タイプ
  Type mType;

  // 必要条件
  AssignList mMandatoryCondition;

  // キューブのリスト
  vector<AssignList> mCubeList;

};

END_NAMESPACE_DRUID

#endif // DETCOND_H
