#ifndef TPGGATEINFO_H
#define TPGGATEINFO_H

/// @file TpgGateInfo.h
/// @brief TpgGateInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"
#include "ym/logic.h"
#include "ym/HashMap.h"
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
  ~TpgGateInfo() { }


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
  GateType
  gate_type() const = 0;

  /// @brief 論理式を返す．
  ///
  /// 論理式タイプ(is_complex() = true)のときのみ意味を持つ．
  virtual
  Expr
  expr() const = 0;

  /// @brief 追加ノード数を返す．
  virtual
  int
  extra_node_num() const = 0;

  /// @brief 制御値を返す．
  /// @param[in] pos 入力位置
  /// @param[in] val 値
  virtual
  Val3
  cval(int pos,
       Val3 val) const = 0;

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

  /// @brief 組み込み型のオブジェクトを返す．
  /// @param[in] gate_type ゲートタイプ
  const TpgGateInfo*
  simple_type(GateType gate_type);

  /// @brief 複合型のオブジェクトを返す．
  /// @param[in] ni 入力数
  /// @param[in] expr 論理式
  const TpgGateInfo*
  complex_type(int ni,
	       const Expr& expr);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


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
