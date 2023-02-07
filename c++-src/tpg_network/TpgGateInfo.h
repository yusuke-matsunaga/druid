#ifndef TPGGATEINFO_H
#define TPGGATEINFO_H

/// @file TpgGateInfo.h
/// @brief TpgGateInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/logic.h"
#include "ym/TvFunc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgGateInfo TpgGateInfo.h "TpgGateInfo.h"
/// @brief TpgNode の論理関数の情報を格納するクラス
///
/// - 追加ノード数
/// - 制御値
/// の情報を持つ．
//////////////////////////////////////////////////////////////////////
class TpgGateInfo
{
public:

  /// @brief デストラクタ
  virtual
  ~TpgGateInfo() = default;


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
  gate_type() const = 0;

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
/// @class TpgGateInfoMgr TpgGateInfoMgr.h "TpgGateInfoMgr.h"
/// @brief TpgGateInfo を管理するクラス
//////////////////////////////////////////////////////////////////////
class TpgGateInfoMgr
{
public:

  /// @brief コンストラクタ
  TpgGateInfoMgr();

  /// @brief デストラクタ
  ~TpgGateInfoMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief TpgGateInfo を登録する．
  const TpgGateInfo*
  new_info(
    SizeType ni,     ///< [in] 入力数
    const Expr& expr ///< [in] 論理式
  );

  /// @brief 組み込み型のオブジェクトを返す．
  const TpgGateInfo*
  simple_type(
    PrimType gate_type ///< [in] ゲートタイプ
  );

  /// @brief 複合型のオブジェクトを返す．
  const TpgGateInfo*
  complex_type(
    SizeType ni,     ///< [in] 入力数
    const Expr& expr ///< [in] 論理式
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 組み込み型のオブジェクトの配列
  TpgGateInfo* mSimpleType[10];

  // 普通のオブジェクトのリスト
  vector<TpgGateInfo*> mList;

};

END_NAMESPACE_DRUID

#endif // TPGGATEINFO_H
