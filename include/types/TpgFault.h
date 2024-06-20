#ifndef TPGFAULT_H
#define TPGFAULT_H

/// @file TpgFault.h
/// @brief TpgFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "NodeTimeValList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgFault TpgFault.h "TpgFault.h"
/// @brief 故障を表すクラス
///
/// 意味的には故障検出条件を表している．
/// - origin_node(): 故障差の現れる起点となるノード
/// - excitation_condition(): origin_node() に故障差が現れる条件
/// - has_previous_state(): 条件として1時刻前の値を用いている時 true になる．
///
/// 故障自体はゲートを対象に定義されるが，TpgNetwork では複合ゲートを
/// 複数の TpgNode に分解して表している．そのため，origin_node(),
/// excitation_condition() は TpgNode を用いて表される．
///
/// TpgFault は TpgNetwork(Impl) が所有する．
/// TpgNetwork が破壊された場合，TpgFault* へのアクセスは不正となる．
//////////////////////////////////////////////////////////////////////
class TpgFault
{
public:

  /// @brief ステムの故障を生成する．
  static
  TpgFault*
  new_fault(
    SizeType id,         ///< [in] ID番号
    const TpgGate* gate, ///< [in] 対象のゲート
    Fval2 fval,          ///< [in] 故障値
    FaultType fault_type ///< [in] 故障の種類
  );

  /// @brief ブランチの故障を生成する．
  static
  TpgFault*
  new_fault(
    SizeType id,         ///< [in] ID番号
    const TpgGate* gate, ///< [in] 対象のゲート
    SizeType ipos,       ///< [in] 入力番号
    Fval2 fval,          ///< [in] 故障値
    FaultType fault_type ///< [in] 故障の種類
  );

  /// @brief ゲート網羅故障を生成する．
  static
  TpgFault*
  new_fault(
    SizeType id,              ///< [in] ID番号
    const TpgGate* gate,      ///< [in] 対象のゲート
    const vector<bool>& ivals ///< [in] 入力値のベクトル
  );

  /// @brief デストラクタ
  ~TpgFault() = default;


protected:

  /// @brief 内容を指定したコンストラクタ
  explicit
  TpgFault(
    SizeType id,        ///< [in] ID番号
    const TpgGate* gate ///< [in] 対象のゲート
  ) : mId{id},
      mGate{gate}
  {
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 故障の情報を返す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 対象のゲートを返す．
  const TpgGate*
  gate() const
  {
    return mGate;
  }

  /// @brief 故障の種類を返す．
  virtual
  FaultType
  fault_type() const = 0;

  /// @brief 故障値を返す．
  ///
  /// 網羅故障の場合は意味を持たない．
  virtual
  Fval2
  fval() const = 0;

  /// @brief ステムの故障の時 true を返す．
  ///
  /// 網羅故障の場合は意味を持たない．
  virtual
  bool
  is_stem() const;

  /// @brief ブランチの故障の時 true を返す．
  ///
  /// 網羅故障の場合は意味を持たない．
  bool
  is_branch() const
  {
    return !is_stem();
  }

  /// @brief ブランチの故障の時の入力位置を返す．
  ///
  /// is_branch() == false の場合は意味を持たない．
  virtual
  SizeType
  branch_pos() const;

  /// @brief 網羅故障の場合の入力値のベクトルを返す．
  ///
  /// 網羅故障以外では意味を持たない．
  virtual
  vector<bool>
  input_vals() const;

  /// @brief 代表故障を返す．
  ///
  /// 自身が代表故障の場合は自身を返す．
  const TpgFault*
  rep_fault() const
  {
    return mRepFault;
  }

  /// @brief 故障の内容を表す文字列を返す．
  virtual
  string
  str() const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件を返す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬の起点となるノードを返す．
  virtual
  const TpgNode*
  origin_node() const = 0;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  virtual
  NodeTimeValList
  excitation_condition() const = 0;

  /// @brief origin_node を含む FFR の根のノードを返す．
  const TpgNode*
  ffr_root() const;

  /// @brief 故障が励起してFFRの根まで伝搬する条件を求める．
  NodeTimeValList
  ffr_propagate_condition() const;


public:
  //////////////////////////////////////////////////////////////////////
  // その他の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 代表故障をセットする．
  void
  set_rep_fault(
    const TpgFault* rep_fault ///< [in] 代表故障
  )
  {
    mRepFault = rep_fault;
  }

  /// @brief ハッシュ用の値を返す．
  SizeType
  hash() const;

  /// @brief 等価比較演算
  bool
  operator==(
    const TpgFault& right
  ) const
  {
    return mId == right.mId;
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

  // ID番号
  SizeType mId;

  // 対象のゲート
  const TpgGate* mGate{nullptr};

  // 代表故障
  const TpgFault* mRepFault{nullptr};

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

// TpgFault をキーにしたハッシュ関数クラスの定義
template <>
struct hash<DRUID_NAMESPACE::TpgFault>
{
  SizeType
  operator()(
    const DRUID_NAMESPACE::TpgFault& fault
  ) const
  {
    return fault.hash();
  }
};

END_NAMESPACE_STD

#endif // TPGFAULT_H
