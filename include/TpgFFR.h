#ifndef TPGFFR_H
#define TPGFFR_H

/// @file TpgFFR.h
/// @brief TpgFFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/Array.h"


BEGIN_NAMESPACE_DRUID

class TpgFaultBase;

//////////////////////////////////////////////////////////////////////
/// @class TpgFFR TpgFFR.h "TpgFFR.h"
/// @brief TpgNetwork の FFR の情報を表すクラス
/// @sa TpgNetwork
/// @sa TpgNode
/// @sa TpgFault
/// @sa TpgMFFC
///
/// 具体的には以下の情報を持つ．
/// - FFR の根のノード
/// - FFR に含まれる代表故障のリスト
/// 一度設定された不変のオブジェクトとなる．
//////////////////////////////////////////////////////////////////////
class TpgFFR
{
public:

  /// @brief コンストラクタ
  TpgFFR() = default;

  /// @brief デストラクタ
  ~TpgFFR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 根のノードを返す．
  const TpgNode*
  root() const
  {
    return mRoot;
  }

  /// @brief 葉(FFRの入力)の数を返す．
  SizeType
  input_num() const
  {
    return mInputList.size();
  }

  /// @brief 葉(FFRの入力)を返す．
  const TpgNode*
  input(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < input_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < input_num() );

    return mInputList[pos];
  }

  /// @brief 葉(FFRの入力)のリストを返す．
  const vector<const TpgNode*>&
  input_list() const
  {
    return mInputList;
  }

  /// @brief このFFRに含まれる代表故障の数を返す．
  SizeType
  fault_num() const
  {
    return mFaultList.size();
  }

  /// @brief このFFRに含まれる代表故障を返す．
  const TpgFault*
  fault(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < fault_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < fault_num() );

    return mFaultList[pos];
  }

  /// @brief このFFRに含まれる代表故障のリストを返す．
  const vector<const TpgFault*>&
  fault_list() const
  {
    return mFaultList;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を設定する．
  ///
  /// input_list, fault_list の所有権は移譲しない．
  /// 生成/解放の責任は親の TpgNetwork にある．
  void
  set(
    const TpgNode* root,                      ///< [in] 根のノード
    const vector<const TpgNode*>& input_list, ///< [in] 入力のノードのリスト(配列)
    const vector<const TpgFault*>& fault_list ///< [in] 故障のリスト(配列)
  )
  {
    mRoot = root;
    mInputList = input_list;
    mFaultList = fault_list;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 根のノード
  const TpgNode* mRoot;

  // 葉のノードの配列
  vector<const TpgNode*> mInputList;

  // 故障の配列
  vector<const TpgFault*> mFaultList;

};

END_NAMESPACE_DRUID

#endif // TPGFFR_H
