#ifndef BGNODE_INT_H
#define BGNODE_INT_H

/// @file BgNode_int.h
/// @brief BgNode_int のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "BgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class BgNode_LIT BgNode_int.h "BgNode_int.h"
/// @brief PLIT|NLIT タイプの共通の基底クラス
///
/// 実は DEC0|DEC1|DEC2 タイプの基底でもある．
//////////////////////////////////////////////////////////////////////
class BgNode_LIT :
  public BgNode
{
public:

  /// @brief コンストラクタ
  BgNode_LIT(
    SizeType varid ///< [in] 変数番号
  ) : mVarId{varid}
  {
  }

  /// @brief デストラクタ
  ~BgNode_LIT() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数番号を返す．
  ///
  /// PLIT|NLIT|DEC0|DEC1|DEC2 の時のみ有効
  SizeType
  varid() const override;

  /// @brief このノードを CNF に変換する際のサイズを返す．
  CnfSize
  cnf_size() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数番号
  SizeType mVarId;

};


//////////////////////////////////////////////////////////////////////
/// @class BgNode_PLIT BgNode_int.h "BgNode_int.h"
/// @brief PLIT タイプのノード
//////////////////////////////////////////////////////////////////////
class BgNode_PLIT :
  public BgNode_LIT
{
public:

  /// @brief コンストラクタ
  BgNode_PLIT(
    SizeType varid ///< [in] 変数番号
  ) : BgNode_LIT{varid}
  {
  }

  /// @brief デストラクタ
  ~BgNode_PLIT() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 種類を返す．
  Type
  type() const override;

  /// @brief PLIT 型の時に true を返す．
  bool
  is_PLIT() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BgNode_NLIT BgNode_int.h "BgNode_int.h"
/// @brief NLIT タイプのノード
//////////////////////////////////////////////////////////////////////
class BgNode_NLIT :
  public BgNode_LIT
{
public:

  /// @brief コンストラクタ
  BgNode_NLIT(
    SizeType varid ///< [in] 変数番号
  ) : BgNode_LIT{varid}
  {
  }

  /// @brief デストラクタ
  ~BgNode_NLIT() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 種類を返す．
  Type
  type() const override;

  /// @brief NLIT 型の時に true を返す．
  bool
  is_NLIT() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BgNode_OP BgNode_int.h "BgNode_int.h"
/// @brief AND|OR タイプの共通な基底クラス
//////////////////////////////////////////////////////////////////////
class BgNode_OP :
  public BgNode
{
public:

  /// @brief コンストラクタ
  BgNode_OP(
    const vector<const BgNode*>& operand_list, ///< [in] オペランドのリスト
    const BgNode* cofactor                     ///< [in] コファクターノード
  ) : mOperandList{operand_list},
      mCofactor{cofactor}
  {
  }

  /// @brief デストラクタ
  ~BgNode_OP();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief オペランドの数を返す．
  ///
  /// AND|OR の時のみ有効
  SizeType
  operand_num() const override;

  /// @brief オペランドを返す．
  ///
  /// AND|OR の時のみ有効
  const BgNode*
  operand(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < operand_num() )
  ) const override;

  /// @brief オペランドのリストを返す．
  ///
  /// AND|OR の時のみ有効
  vector<const BgNode*>
  operand_list() const override;

  /// @brief コファクターを返す．
  const BgNode*
  cofactor() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // オペランドリスト
  vector<const BgNode*> mOperandList;

  // コファクターノード
  const BgNode* mCofactor;

};


//////////////////////////////////////////////////////////////////////
/// @class BgNode_AND BgNode_int.h "BgNode_int.h"
/// @brief AND タイプのノード
//////////////////////////////////////////////////////////////////////
class BgNode_AND :
  public BgNode_OP
{
public:

  /// @brief コンストラクタ
  BgNode_AND(
    const vector<const BgNode*>& operand_list, ///< [in] オペランドのリスト
    const BgNode* cofactor                     ///< [in] コファクターノード
  ) : BgNode_OP{operand_list, cofactor}
  {
  }

  /// @brief デストラクタ
  ~BgNode_AND() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 種類を返す．
  Type
  type() const override;

  /// @brief AND 型の時に true を返す．
  bool
  is_AND() const override;

  /// @brief このノードを CNF に変換する際のサイズを返す．
  CnfSize
  cnf_size() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BgNode_OR BgNode_int.h "BgNode_int.h"
/// @brief OR タイプのノード
//////////////////////////////////////////////////////////////////////
class BgNode_OR :
  public BgNode_OP
{
public:

  /// @brief コンストラクタ
  BgNode_OR(
    const vector<const BgNode*>& operand_list, ///< [in] オペランドのリスト
    const BgNode* cofactor                     ///< [in] コファクターノード
  ) : BgNode_OP{operand_list, cofactor}
  {
  }

  /// @brief デストラクタ
  ~BgNode_OR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 種類を返す．
  Type
  type() const override;

  /// @brief OR 型の時に true を返す．
  bool
  is_OR() const override;

  /// @brief このノードを CNF に変換する際のサイズを返す．
  CnfSize
  cnf_size() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BgNode_DEC BgNode_int.h "BgNode_int.h"
/// @brief DEC2|DEC1|DEC0 タイプの共通な基底クラス
//////////////////////////////////////////////////////////////////////
class BgNode_DEC :
  public BgNode_LIT
{
public:

  /// @brief コンストラクタ
  BgNode_DEC(
    SizeType varid,          ///< [in] 変数番号
    const BgNode* cofactor0, ///< [in] 負のコファクター
    const BgNode* cofactor1  ///< [in] 正のコファクター
  ) : BgNode_LIT{varid},
      mCofactor0{cofactor0},
      mCofactor1{cofactor1}
  {
  }

  /// @brief デストラクタ
  ~BgNode_DEC() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 負のコファクターを返す．
  ///
  /// DEC2|DEC1|DEC0 の時のみ有効
  const BgNode*
  cofacotr0() const override;

  /// @brief 正のコファクターを返す．
  ///
  /// DEC2|DEC1|DEC0 の時のみ有効
  const BgNode*
  cofacotr1() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 負のコファクター
  const BgNode* mCofactor0;

  // 正のコファクター
  const BgNode* mCofactor1;

};


//////////////////////////////////////////////////////////////////////
/// @class BgNode_DEC2 BgNode_int.h "BgNode_int.h"
/// @brief DEC2 タイプのノード
//////////////////////////////////////////////////////////////////////
class BgNode_DEC2 :
  public BgNode_DEC
{
public:

  /// @brief コンストラクタ
  BgNode_DEC2(
    SizeType varid,          ///< [in] 変数番号
    const BgNode* cofactor0, ///< [in] 負のコファクター
    const BgNode* cofactor1  ///< [in] 正のコファクター
  ) : BgNode_DEC{varid, cofactor0, cofactor1}
  {
  }

  /// @brief デストラクタ
  ~BgNode_DEC2() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 種類を返す．
  Type
  type() const override;

  /// @brief DEC2 型の時に true を返す．
  bool
  is_DEC2() const override;

  /// @brief このノードを CNF に変換する際のサイズを返す．
  CnfSize
  cnf_size() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BgNode_DEC1 BgNode_int.h "BgNode_int.h"
/// @brief DEC1 タイプのノード
//////////////////////////////////////////////////////////////////////
class BgNode_DEC1 :
  public BgNode_DEC
{
public:

  /// @brief コンストラクタ
  BgNode_DEC1(
    SizeType varid,          ///< [in] 変数番号
    const BgNode* cofactor0, ///< [in] 負のコファクター
    const BgNode* cofactor1  ///< [in] 正のコファクター
  ) : BgNode_DEC{varid, cofactor0, cofactor1}
  {
  }

  /// @brief デストラクタ
  ~BgNode_DEC1() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 種類を返す．
  Type
  type() const override;

  /// @brief DEC1 型の時に true を返す．
  bool
  is_DEC1() const override;

  /// @brief このノードを CNF に変換する際のサイズを返す．
  CnfSize
  cnf_size() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class BgNode_DEC0 BgNode_int.h "BgNode_int.h"
/// @brief DEC0 タイプのノード
//////////////////////////////////////////////////////////////////////
class BgNode_DEC0 :
  public BgNode_DEC
{
public:

  /// @brief コンストラクタ
  BgNode_DEC0(
    SizeType varid,          ///< [in] 変数番号
    const BgNode* cofactor0, ///< [in] 負のコファクター
    const BgNode* cofactor1  ///< [in] 正のコファクター
  ) : BgNode_DEC{varid, cofactor0, cofactor1}
  {
  }

  /// @brief デストラクタ
  ~BgNode_DEC0() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 種類を返す．
  Type
  type() const override;

  /// @brief DEC0 型の時に true を返す．
  bool
  is_DEC0() const override;

  /// @brief このノードを CNF に変換する際のサイズを返す．
  CnfSize
  cnf_size() const override;

};

END_NAMESPACE_DRUID

#endif // BGNODE_INT_H
