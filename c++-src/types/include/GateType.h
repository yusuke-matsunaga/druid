#ifndef GATETYPE_H
#define GATETYPE_H

/// @file GateType.h
/// @brief GateType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/logic.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class GateType GateType.h "GateType.h"
/// @brief ゲートの種類を表すクラス
///
/// - ID番号
/// - 追加ノード数
/// - 制御値
/// の情報を持つ．
///
/// ただしこのクラスはほぼインターフェイスの定義のみ
//////////////////////////////////////////////////////////////////////
class GateType
{
public:

  /// @brief コンストラクタ
  GateType(
    SizeType id ///< [in] ID番号
  ) : mId{id}
  {
  }

  /// @brief デストラクタ
  virtual
  ~GateType() = default;

  /// @brief 入力型のオブジェクトを作る．
  static
  GateType*
  new_ppi(
    SizeType id ///< [in] ID番号
  );

  /// @brief 出力型のオブジェクトを作る．
  static
  GateType*
  new_ppo(
    SizeType id ///< [in] ID番号
  );

  /// @brief 組込み型のオブジェクトを作る．
  static
  GateType*
  new_primitive(
    SizeType id,        ///< [in] ID番号
    SizeType input_num, ///< [in] 入力数
    PrimType gate_type  ///< [in] ゲートタイプ
  );

  /// @brief 複合型のオブジェクトを作る．
  ///
  /// expr の内容によっては組み込み型になる．
  static
  GateType*
  new_cplx(
    SizeType id,        ///< [in] ID番号
    SizeType input_num, ///< [in] 入力数
    const Expr& expr    ///< [in] 論理式
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 入力数を返す．
  virtual
  SizeType
  input_num() const = 0;;

  /// @brief PPI のときに true を返す．
  virtual
  bool
  is_ppi() const;

  /// @brief PPO のときに true を返す．
  virtual
  bool
  is_ppo() const;

  /// @brief 組み込みタイプのときに true を返す．
  virtual
  bool
  is_primitive() const;

  /// @brief 論理式タイプのときに true を返す．
  virtual
  bool
  is_expr() const;

  /// @brief ゲートタイプを返す．
  ///
  /// 組み込みタイプ(is_primitive() = true)のときのみ意味を持つ．
  virtual
  PrimType
  primitive_type() const;

  /// @brief 論理式を返す．
  ///
  /// 論理式タイプ(is_complex() = true)のときのみ意味を持つ．
  virtual
  Expr
  expr() const;

  /// @brief 追加ノード数を返す．
  virtual
  SizeType
  extra_node_num() const;

  /// @brief 制御値を返す．
  ///
  /// pos に val を与えた時の出力値を返す．
  /// 他の入力値に依存している場合は val3::_X を返す．
  virtual
  Val3
  cval(
    SizeType pos, ///< [in] 入力位置
    Val3 val      ///< [in] 値
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

};

END_NAMESPACE_DRUID

#endif // GATETYPE_H
