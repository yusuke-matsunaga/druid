#ifndef ASSIGN_H
#define ASSIGN_H

/// @file Assign.h
/// @brief Assign のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgBase.h"
#include "types/TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Assign Assign.h "td/Assign.h"
/// @brief ノードに対する値の割当を表すクラス
///
/// ノード番号と時刻と値を持つ．
/// なお，縮退故障モードのときは時刻は 1 となる．
//////////////////////////////////////////////////////////////////////
class Assign:
  public TpgBase
{
  friend class AssignList;
  friend class AssignIter;

public:

  /// @brief 空のコンストラクタ
  ///
  /// 内容は不定
  Assign() = default;

  /// @brief 値を指定したコンストラクタ
  Assign(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    SizeType node_id,                           ///< [in] ノード番号
    int time,	                                ///< [in] 時刻 ( 0 or 1 )
    bool val	                                ///< [in] 値
  ) : TpgBase(network),
      mPackVal{encode(node_id, time, val)}
  {
  }

  /// @grief ノードを指定したコンストラクタ
  Assign(
    const TpgNode& node, ///< [in] ノード
    int time,	         ///< [in] 時刻 ( 0 or 1 )
    bool val	         ///< [in] 値
  ) : TpgBase(node),
      mPackVal{encode(node.id(), time, val)}
  {
  }

  /// @brief コピーコンストラクタ
  Assign(const Assign& src) = default;

  /// @brief コピー代入演算子
  Assign&
  operator=(const Assign& src) = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード番号を返す．
  SizeType
  node_id() const
  {
    _check_valid();
    return decode_node_id(mPackVal);
  }

  /// @brief ノードを返す．
  TpgNode
  node() const
  {
    return TpgNode(_network(), node_id());
  }

  /// @brief 時刻を返す．
  ///
  /// 返される値は 0 か 1
  int
  time() const
  {
    return static_cast<int>((mPackVal >> 1) & 1U);
  }

  /// @brief ノードと時刻をパックした値を返す．
  ///
  /// 結果は等価比較のみに用いる．
  SizeType
  node_time() const
  {
    return mPackVal >> 1;
  }

  /// @brief 値を返す．
  bool
  val() const
  {
    return static_cast<bool>(mPackVal & 1UL);
  }

  /// @brief 反対の割り当てを返す．
  Assign
  operator~() const
  {
    return Assign{_network(), mPackVal ^ 1UL};
  }

  /// @brief ハッシュ値を返す．
  SizeType
  hash() const
  {
    return static_cast<SizeType>(mPackVal);
  }

  /// @brief 等価関係の比較関数
  bool
  operator==(
    const Assign& right
  ) const
  {
    return mPackVal == right.mPackVal;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const Assign& right
  ) const
  {
    return !operator==(right);
  }

  /// @brief 大小関係の比較関数
  bool
  operator<(
    const Assign& right
  ) const;

  /// @brief 大小関係の比較関数
  bool
  operator>(
    const Assign& right
  ) const
  {
    return right.operator<(*this);
  }

  /// @brief 大小関係の比較関数
  bool
  operator<=(
    const Assign& right
  ) const
  {
    return !right.operator<(*this);
  }

  /// @brief 大小関係の比較関数
  bool
  operator>=(
    const Assign& right
  ) const
  {
    return !operator<(right);
  }

  /// @brief mPackVal に符号化する．
  static
  SizeType
  encode(
    SizeType node_id, ///< [in] ノード番号
    int time,	      ///< [in] 時刻 ( 0 or 1 )
    bool val	      ///< [in] 値
  )
  {
    return (node_id << 2) | (time << 1) | val;
  }

  /// @brief mPackVal からノード番号を取り出す．
  static
  SizeType
  decode_node_id(
    SizeType pack_val
  )
  {
    return pack_val >> 2;
  }

  /// @brief mPackVal から時刻を取り出す．
  static
  int
  decode_time(
    SizeType pack_val
  )
  {
    return static_cast<int>((pack_val >> 1) & 1U);
  }

  /// @brief mPackVal から値を取り出す．
  static
  bool
  decode_val(
    SizeType pack_val
  )
  {
    return static_cast<bool>(pack_val & 1UL);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を指定したコンストラクタ
  Assign(
    const std::shared_ptr<NetworkRep>& network,
    SizeType val
  ) : TpgBase(network),
      mPackVal{val}
  {
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パックした値
  SizeType mPackVal;

};

/// @brief 割当の内容を出力する．
/// @return s を返す．
std::ostream&
operator<<(
  std::ostream& s, ///< [in] 出力先のストリーム
  Assign nv        ///< [in] 値の割り当て
);

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_STD

// Assign をキーにしたハッシュ関数クラスの定義
template <>
struct hash<DRUID_NAMESPACE::Assign>
{
  SizeType
  operator()(
    const DRUID_NAMESPACE::Assign& nv
  ) const
  {
    return nv.hash();
  }
};

END_NAMESPACE_STD

#endif // ASSIGN_H
