#ifndef EXPR2AIG_H
#define EXPR2AIG_H

/// @file Expr2Aig.h
/// @brief Expr2Aig のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/Expr.h"
#include "ym/AigMgr.h"
#include "ym/AigHandle.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Expr2Aig Expr2Aig.h "Expr2Aig.h"
/// @brief 複数の Expr から共有した AIG を作るためのクラス
//////////////////////////////////////////////////////////////////////
class Expr2Aig
{
public:

  /// @brief コンストラクタ
  Expr2Aig(
    AigMgr& mgr ///< [in] AIGマネージャ
  ) : mMgr{mgr}
  {
  }

  /// @brief デストラクタ
  ~Expr2Aig() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief Expr を AIG に変換する．
  AigHandle
  conv_to_aig(
    const Expr& expr ///< [in] 論理式
  );

  /// @brief 複数の Expr を AIG に変換する
  vector<AigHandle>
  conv_to_aig(
    const vector<Expr>& expr_list ///< [in] 論理式のリスト
  )
  {
    vector<AigHandle> aig_list;
    aig_list.reserve(expr_list.size());
    for ( auto& expr: expr_list ) {
      auto aig = conv_to_aig(expr);
      aig_list.push_back(aig);
    }
    return aig_list;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief AND木を作る．
  AigHandle
  make_and_tree(
    const vector<AigHandle>& aig_list, ///< [in] 対象のAIGのリスト
    SizeType begin,                    ///< [in] リスト中の開始位置
    SizeType end                       ///< [in] リスト中の終了位置
  );

  /// @brief OR木を作る．
  AigHandle
  make_or_tree(
    const vector<AigHandle>& aig_list, ///< [in] 対象のAIGのリスト
    SizeType begin,                    ///< [in] リスト中の開始位置
    SizeType end                       ///< [in] リスト中の終了位置
  );

  /// @brief XOR木を作る．
  AigHandle
  make_xor_tree(
    const vector<AigHandle>& aig_list, ///< [in] 対象のAIGのリスト
    SizeType begin,                    ///< [in] リスト中の開始位置
    SizeType end                       ///< [in] リスト中の終了位置
  );

  /// @brief ANDを作る．
  AigHandle
  make_and(
    AigHandle aig0,
    AigHandle aig1
  )
  {
    if ( aig0 == aig1 ) {
      return aig0;
    }
    if ( aig0 > aig1 ) {
      std::swap(aig0, aig1);
    }
    auto aig = mMgr.and_op({aig0, aig1});
    return aig;
  }

  /// @brief ORを作る．
  AigHandle
  make_or(
    AigHandle aig0,
    AigHandle aig1
  )
  {
    return ~make_and(~aig0, ~aig1);
  }

  /// @brief xORを作る．
  AigHandle
  make_xor(
    AigHandle aig0,
    AigHandle aig1
  )
  {
    if ( aig0 == aig1 ) {
      return mMgr.make_zero();
    }
    if ( ~aig0 == aig1 ) {
      return mMgr.make_one();
    }
    auto tmp0 = make_and(aig0, ~aig1);
    auto tmp1 = make_and(~aig0, aig1);
    return make_or(tmp0, tmp1);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // AIGマネージャ
  AigMgr& mMgr;

};

END_NAMESPACE_DRUID

#endif // EXPR2AIG_H
