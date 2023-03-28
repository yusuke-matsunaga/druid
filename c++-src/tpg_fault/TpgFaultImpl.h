#ifndef TPGFAULTIMPL_H
#define TPGFAULTIMPL_H

/// @file TpgFaultImpl.h
/// @brief TpgFaultImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgFaultImpl TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief TpgFault の実装クラス
//////////////////////////////////////////////////////////////////////
class TpgFaultImpl
{
public:

  /// @brief コンストラクタ
  TpgFaultImpl(
    const TpgNode* node, ///< [in] 故障位置のノード
    const string& str 	 ///< [in] 故障を表す文字列
  ) : mNode{node},
      mStr{str}
  {
  }

  /// @brief デストラクタ
  virtual
  ~TpgFaultImpl() = default;


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

  /// @brief 故障伝搬の起点となるノードを返す．
  const TpgNode*
  origin_node() const
  {
    return mNode;
  }

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  virtual
  NodeValList
  excitation_condition() const = 0;

  /// @brief 故障の内容を表す文字列を返す．
  string
  str() const
  {
    return mStr;
  }

  /// @brief 支配故障のリストを返す．
  const vector<TpgFaultImpl*>&
  dom_fault_list() const
  {
    return mDomFaultList;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 値を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号をセットする．
  void
  set_id(
    SizeType id ///< [in] ID番号
  )
  {
    mId = id;
  }

  /// @brief 支配故障を追加する．
  void
  add_dom_fault(
    TpgFaultImpl* dom ///< [in] 支配故障
  )
  {
    mDomFaultList.push_back(dom);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

  // 故障伝搬の起点ノード
  const TpgNode* mNode;

  // 故障を表す文字列
  string mStr;

  // 支配故障のリスト
  vector<TpgFaultImpl*> mDomFaultList;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_SaStem TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief 出力の縮退故障を表す基底クラス
//////////////////////////////////////////////////////////////////////
class TpgFault_SaStem :
  public TpgFaultImpl
{
public:

  /// @brief コンストラクタ
  TpgFault_SaStem(
    const TpgNode* node, ///< [in] 故障位置のノード
    const string& str,	 ///< [in] 故障を表す文字列
    Fval2 val            ///< [in] 故障値
  ) : TpgFaultImpl{node, str},
      mVal{val}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_SaStem() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  NodeValList
  excitation_condition() const override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const
  {
    return mVal;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障値
  Fval2 mVal;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_SaBranch TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief 入力の縮退故障を表す基底クラス
///
/// 名前が似ているが TpgFault_SaStem とは直接の関係はない．
//////////////////////////////////////////////////////////////////////
class TpgFault_SaBranch :
  public TpgFaultImpl
{
public:

  /// @brief コンストラクタ
  TpgFault_SaBranch(
    const TpgNode* node, ///< [in] 故障位置のノード
    const string& str,	 ///< [in] 故障を表す文字列
    SizeType ipos,       ///< [in] 入力位置
    Fval2 val            ///< [in] 故障値
  ) : TpgFaultImpl{node, str},
      mIposVal{(ipos << 1) | static_cast<SizeType>(val)}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_SaBranch() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  NodeValList
  excitation_condition() const override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力位置を得る．
  SizeType
  ipos() const
  {
    return mIposVal >> 1;
  }

  /// @brief 故障値を得る．
  Fval2
  fval() const
  {
    return static_cast<Fval2>(mIposVal & 1);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力位置 + 故障値
  SizeType mIposVal;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_TdStem TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief 出力の縮退故障を表す基底クラス
//////////////////////////////////////////////////////////////////////
class TpgFault_TdStem :
  public TpgFault_SaStem
{
public:

  /// @brief コンストラクタ
  TpgFault_TdStem(
    const TpgNode* node, ///< [in] 故障位置のノード
    const string& str,	 ///< [in] 故障を表す文字列
    Fval2 val            ///< [in] 故障値
  ) : TpgFault_SaStem{node, str, val}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_TdStem() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  NodeValList
  excitation_condition() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_TdBranch TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief 入力の縮退故障を表す基底クラス
///
/// 名前が似ているが TpgFault_TdStem とは直接の関係はない．
//////////////////////////////////////////////////////////////////////
class TpgFault_TdBranch :
  public TpgFault_SaBranch
{
public:

  /// @brief コンストラクタ
  TpgFault_TdBranch(
    const TpgNode* node, ///< [in] 故障位置のノード
    const string& str,	 ///< [in] 故障を表す文字列
    SizeType ipos,       ///< [in] 入力位置
    Fval2 val            ///< [in] 故障値
  ) : TpgFault_SaBranch{node, str, ipos, val}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_TdBranch() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  NodeValList
  excitation_condition() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFault_Ex TpgFaultImpl.h "TpgFaultImpl.h"
/// @brief ゲート網羅故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgFault_Ex :
  public TpgFaultImpl
{
public:

  /// @brief コンストラクタ
  TpgFault_Ex(
    const TpgNode* node,      ///< [in] 故障位置のノード
    const string& str,	      ///< [in] 故障を表す文字列
    const vector<bool>& ivals ///< [in] 入力の値のリスト
  ) : TpgFaultImpl{node, str},
      mIvals{ivals}
  {
  }

  /// @brief デストラクタ
  ~TpgFault_Ex() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  NodeValList
  excitation_condition() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力値のリスト
  vector<bool> mIvals;

};

END_NAMESPACE_DRUID

#endif // TPGFAULTIMPL_H
