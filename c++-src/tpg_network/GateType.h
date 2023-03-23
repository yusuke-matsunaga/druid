#ifndef GATETYPE_H
#define GATETYPE_H

/// @file GateType.h
/// @brief GateType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/logic.h"
#include "ym/TvFunc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class GateType GateType.h "GateType.h"
/// @brief ゲートの種類を表すクラス
///
/// - 追加ノード数
/// - 制御値
/// の情報を持つ．
///
/// ただしこのクラスはインターフェイスの定義のみ
//////////////////////////////////////////////////////////////////////
class GateType
{
public:

  /// @brief デストラクタ
  virtual
  ~GateType() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 組み込みタイプのときに true を返す．
  virtual
  bool
  is_simple() const = 0;

  /// @brief 論理式タイプのときに true を返す．
  ///
  /// = !is_simple();
  bool
  is_complex() const
  {
    return !is_simple();
  }

  /// @brief ゲートタイプを返す．
  ///
  /// 組み込みタイプ(is_simple() = true)のときのみ意味を持つ．
  virtual
  PrimType
  primitive_type() const = 0;

  /// @brief 論理式を返す．
  ///
  /// 論理式タイプ(is_complex() = true)のときのみ意味を持つ．
  virtual
  Expr
  expr() const = 0;

  /// @brief 追加ノード数を返す．
  virtual
  SizeType
  extra_node_num() const = 0;

  /// @brief 制御値を返す．
  virtual
  Val3
  cval(
    SizeType pos, ///< [in] 入力位置
    Val3 val      ///< [in] 値
  ) const = 0;

};


//////////////////////////////////////////////////////////////////////
/// @class GateType_Simple GateType_Simple.h "GateType_Simple.h"
/// @brief 組み込み型の GateType_Simple
//////////////////////////////////////////////////////////////////////
class GateType_Simple :
  public GateType
{
public:

  /// @brief コンストラクタ
  GateType_Simple(
    PrimType gate_type ///< [in] ゲートタイプ
  );

  /// @brief デストラクタ
  ~GateType_Simple() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 組み込みタイプのときに true を返す．
  bool
  is_simple() const override;

  /// @brief ゲートタイプを返す．
  PrimType
  primitive_type() const override;

  /// @brief 論理式を返す．
  Expr
  expr() const override;

  /// @brief 追加ノード数を返す．
  SizeType
  extra_node_num() const override;

  /// @brief 制御値を返す．
  Val3
  cval(
    SizeType pos, ///< [in] 入力位置
    Val3 val      ///< [in] 値
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ゲートタイプ
  PrimType mPrimType;

  // 制御値
  Val3 mCVal[2];

};


//////////////////////////////////////////////////////////////////////
/// @class GateType_Cplx GateType_Cplx.h "GateType_Cplx.h"
/// @brief 複合型の GateType
//////////////////////////////////////////////////////////////////////
class GateType_Cplx :
  public GateType
{
public:

  /// @brief コンストラクタ
  GateType_Cplx(
    SizeType ni,     ///< [in] 入力数
    const Expr& expr ///< [in] 論理式
  );

  /// @brief デストラクタ
  ~GateType_Cplx() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 組み込みタイプのときに true を返す．
  bool
  is_simple() const override;

  /// @brief ゲートタイプを返す．
  PrimType
  primitive_type() const override;

  /// @brief 論理式を返す．
  Expr
  expr() const override;

  /// @brief 追加ノード数を返す．
  SizeType
  extra_node_num() const override;

  /// @brief 制御値を返す．
  Val3
  cval(
    SizeType pos, ///< [in] 入力位置
    Val3 val      ///< [in] 値
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 論理式
  Expr mExpr;

  // 追加のノード数
  SizeType mExtraNodeNum;

  // 制御値の配列
  vector<Val3> mCVal;

};


//////////////////////////////////////////////////////////////////////
/// @class GateTypeMgr GateTypeMgr.h "GateTypeMgr.h"
/// @brief GateType を管理するクラス
//////////////////////////////////////////////////////////////////////
class GateTypeMgr
{
public:

  /// @brief コンストラクタ
  GateTypeMgr();

  /// @brief デストラクタ
  ~GateTypeMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief GateType を登録する．
  const GateType*
  new_type(
    SizeType ni,     ///< [in] 入力数
    const Expr& expr ///< [in] 論理式
  );

  /// @brief 組み込み型のオブジェクトを返す．
  const GateType*
  simple_type(
    PrimType gate_type ///< [in] ゲートタイプ
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 複合型のオブジェクトを返す．
  const GateType*
  complex_type(
    SizeType ni,     ///< [in] 入力数
    const Expr& expr ///< [in] 論理式
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 組み込み型のオブジェクトの配列
  GateType* mSimpleType[10];

  // 普通のオブジェクトのリスト
  vector<GateType*> mList;

};

END_NAMESPACE_DRUID

#endif // GATETYPE_H
