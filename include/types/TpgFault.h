#ifndef TPGFAULT_H
#define TPGFAULT_H

/// @file TpgFault.h
/// @brief TpgFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgObjBase.h"
#include "types/AssignList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgFault TpgFault.h "TpgFault.h"
/// @brief 故障を表すクラス
///
/// 実体は TpgFaultImpl が表す．
/// このクラスは NetworkRep への共有ポインタと故障番号のみを持つ．
/// TpgFaultImpl は NetworkRep に故障番号を与えることで取得することができる．
//////////////////////////////////////////////////////////////////////
class TpgFault:
  public TpgObjBase
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgFault() = default;

  /// @brief 内容を指定したコンストラクタ
  explicit
  TpgFault(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    SizeType id                                 ///< [in] ID番号
  ) : TpgObjBase(network, id)
  {
  }

  /// @brief デストラクタ
  ~TpgFault() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障の情報を返す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象のゲートを返す．
  TpgGate
  gate() const;

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const;

  /// @brief 故障値を返す．
  ///
  /// 網羅故障の場合は意味を持たない．
  Fval2
  fval() const;

  /// @brief ステムの故障の時 true を返す．
  ///
  /// 網羅故障の場合は意味を持たない．
  bool
  is_stem() const;

  /// @brief ブランチの故障の時 true を返す．
  ///
  /// 網羅故障の場合は意味を持たない．
  bool
  is_branch() const;

  /// @brief ブランチの故障の時の入力位置を返す．
  ///
  /// is_branch() == false の場合は意味を持たない．
  SizeType
  branch_pos() const;

  /// @brief 網羅故障の場合の入力値のベクトルを返す．
  ///
  /// 網羅故障以外では意味を持たない．
  std::vector<bool>
  input_vals() const;

  /// @brief 代表故障を返す．
  ///
  /// 自身が代表故障の場合は自身を返す．
  TpgFault
  rep_fault() const;

  /// @brief 故障の内容を表す文字列を返す．
  std::string
  str() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件を返す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬の起点となるノードを返す．
  TpgNode
  origin_node() const;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  AssignList
  excitation_condition() const;

  /// @brief origin_node を含む FFR の根のノードを返す．
  TpgNode
  ffr_root() const;

  /// @brief 故障が励起してFFRの根まで伝搬する条件を求める．
  AssignList
  ffr_propagate_condition() const;


public:
  //////////////////////////////////////////////////////////////////////
  // その他の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ハッシュ用の値を返す．
  SizeType
  hash() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief FaultRep のポインタを取り出す．
  const FaultRep*
  _fault() const
  {
    return TpgBase::_fault(id());
  }

};

/// @relates TpgFault
/// @brief ストリーム出力演算子
inline
std::ostream&
operator<<(
  std::ostream& s,  ///< [in] 出力先のストリーム
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
