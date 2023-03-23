#ifndef TPGGATE_H
#define TPGGATE_H

/// @file TpgGate.h
/// @brief TpgGate のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class TpgNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgGate TpgGate.h "TpgGate.h"
/// @brief ゲートの情報を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgGate
{
public:

  /// @brief ブランチの情報を表す構造体
  struct BranchInfo
  {
    const TpgNode* node{nullptr}; ///< ノード
    SizeType ipos{0};             ///< 入力位置
  };


public:

  /// @brief 空のコンストラクタ
  TpgGate() = default;

  /// @brief コンストラクタ
  TpgGate(
    const TpgNetworkImpl* network, ///< [in] ネットワーク
    SizeType id                    ///< [in] ID番号
  ) : mNetwork{network},
      mId{id}
  {
  }

  /// @brief デストラクタ
  ~TpgGate() = default;


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

  /// @brief 名前を返す．
  string
  name() const;

  /// @brief 出力に対応するノードを返す．
  const TpgNode*
  output_node() const;

  /// @brief 入力数を返す．
  SizeType
  input_num() const;

  /// @brief ブランチの情報を返す．
  BranchInfo
  branch_info(
    SizeType pos ///< [in] 入力位置
  ) const;

  /// @brief 制御値を返す．
  ///
  /// 具体的には pos に値val を与えた時の
  /// 出力値を返す．
  Val3
  cval(
    SizeType pos, ///< [in] 入力位置
    Val3 val      ///< [in] 値
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  const TpgNetworkImpl* mNetwork;

  // ID番号
  SizeType mId;

};

END_NAMESPACE_DRUID

#endif // TPGGATE_H
