#ifndef TPGFAULT_H
#define TPGFAULT_H

/// @file TpgFault.h
/// @brief TpgFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class TpgFaultMgrImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgFault TpgFault.h "TpgFault.h"
/// @brief 故障を表すクラス
///
/// 正確には故障検出条件を表している．
/// - origin_node(): 故障差の現れる起点となるノード
/// - excitation_condition(): origin_node() に故障差が現れる条件
///
/// 縮退故障，遷移故障，ゲート網羅故障の区別はない．
//////////////////////////////////////////////////////////////////////
class TpgFault
{
public:

  /// @brief 空のコンストラクタ
  TpgFault() = default;

  /// @brief 内容を指定したコンストラクタ
  TpgFault(
    TpgFaultMgrImpl* mgr, ///< [in] 故障マネージャ
    SizeType id           ///< [in] ID番号
  ) : mMgr{mgr},
      mId{id}
  {
  }

  /// @brief デストラクタ
  ~TpgFault() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // read-only のメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 故障伝搬の起点となるノードを返す．
  const TpgNode*
  origin_node() const;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  NodeValList
  excitation_condition() const;

  /// @brief origin_node を含む FFR の根のノードを返す．
  const TpgNode*
  ffr_root() const;

  /// @brief 故障が励起してFFRの根まで伝搬する条件を求める．
  NodeValList
  ffr_propagate_condition() const;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const;

  /// @brief 等価比較演算
  bool
  operator==(
    const TpgFault& right
  ) const
  {
    return mMgr == right.mMgr && mId == right.mId;
  }

  /// @brief 非等価比較演算
  bool
  operator!=(
    const TpgFault& right
  ) const
  {
    return !operator==(right);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障マネージャ
  TpgFaultMgrImpl* mMgr{nullptr};

  // ID番号
  SizeType mId{0};

};

/// @relates TpgFault
/// @brief ストリーム出力演算子
inline
ostream&
operator<<(
  ostream& s,       ///< [in] 出力先のストリーム
  const TpgFault& f ///< [in] 故障
)
{
  return s << f.str();
}

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_STD

// TpgFault キーにしたハッシュ関数クラスの定義
template <>
struct hash<DRUID_NAMESPACE::TpgFault>
{
  SizeType
  operator()(
    const DRUID_NAMESPACE::TpgFault& fault
  ) const
  {
    return fault.id();
  }
};

END_NAMESPACE_STD

#endif // TPGFAULT_H
