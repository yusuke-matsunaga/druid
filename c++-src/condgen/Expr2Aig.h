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
  // AigHandle の構造を表す構造体
  struct AigStr {
    AigHandle h0;
    AigHandle h1;

    // ハッシュ値
    SizeType
    hash() const
    {
      return h0.hash() + h1.hash() * 1048573;
    }

    // 等価比較演算
    bool
    operator==(
      const AigStr& other
    ) const
    {
      return h0 == other.h0 && h1 == other.h1;
    }

    // 非等価比較演算
    bool
    operator!=(
      const AigStr& other
    ) const
    {
      return !operator==(other);
    }

  };

  // AigHandle の構造を考慮したハッシュ関数
  struct AigStrHash {
    SizeType
    operator()(
      const AigStr& aig_str
    ) const
    {
      return aig_str.hash();
    }
  };

  // AigHandle の構造を考慮した辞書の型
  using AigStrDict = std::unordered_map<AigStr, AigHandle, AigStrHash>;

public:

  /// @brief コンストラクタ
  Expr2Aig(
    AigMgr& mgr, ///< [in] AIGマネージャ
    bool sharing ///< [in] 共有を行う時 true にするフラグ
  ) : mMgr{mgr},
      mSharing{sharing}
  {
  }

  /// @brief デストラクタ
  ~Expr2Aig() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 複数の Expr を AIG に変換する
  vector<AigHandle>
  conv_to_aig(
    const vector<Expr>& expr_list ///< [in] 論理式のリスト
  )
  {
    vector<AigHandle> aig_list;
    aig_list.reserve(expr_list.size());
    for ( auto& expr: expr_list ) {
      auto aig = _conv_to_aig(expr);
      aig_list.push_back(aig);
    }
    return aig_list;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief Expr を AIG に変換する．
  AigHandle
  _conv_to_aig(
    const Expr& expr ///< [in] 論理式
  );

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
    auto key = AigStr{aig0, aig1};
    if ( mSharing && mStrDict.count(key) > 0 ) {
      return mStrDict.at(key);
    }
    auto aig = mMgr.and_op({aig0, aig1});
    if ( mSharing ) {
      mStrDict.emplace(key, aig);
    }
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

  // AIG の構造に基づく共有を行う時に true にするフラグ
  bool mSharing;

  // AIG の構造に基づくハッシュ表
  AigStrDict mStrDict;

};

END_NAMESPACE_DRUID

#endif // EXPR2AIG_H
