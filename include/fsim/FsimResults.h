#ifndef FSIMRESULTS_H
#define FSIMRESULTS_H

/// @file FsimResults.h
/// @brief FsimResults のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "fsim/DiffBits.h"


BEGIN_NAMESPACE_DRUID

class FsimResultsRep;

//////////////////////////////////////////////////////////////////////
/// @class FsimResults FsimResults.h "fsim/FsimResults.h"
/// @brief 故障シミュレーションの結果を表すクラス
///
/// 具体的には以下の情報のリストを持つ．
/// - テストベクタ番号
/// - 検出された故障番号
/// - 出力の故障伝搬状態
///
/// 見せかけのコピーを効率よく行うため実体クラスを別に用意する．
/// 通常のコピー/代入では同じ実体を共有する．
/// ただし，内容を変更する場合には別の実体に複製してから変更を行う．
///
/// 内容を設定する際には現在のテストベクタ番号を内部に保持している．
/// 常に現在のテストベクタに対してのみ要素を追加を行う．
//////////////////////////////////////////////////////////////////////
class FsimResults
{
public:

  /// @brief 空のコンストラクタ
  FsimResults();

  /// @brief 内容を指定するコンストラクタ
  explicit
  FsimResults(
    const std::shared_ptr<FsimResultsRep>& src
  );

  /// @brief コピーコンストラクタ
  FsimResults(
    const FsimResults& src
  ) = default;

  /// @brief 代入演算子
  FsimResults&
  operator=(
    const FsimResults& src
  ) = default;

  /// @brief デストラクタ
  ~FsimResults() = default;


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 内容を取得する関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief テストベクタの総数を返す．
  SizeType
  tv_num() const;

  /// @brief 指定されたテストベクタ番号で検出された故障番号のリストを返す．
  std::vector<SizeType>
  fault_list(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const;

  /// @brief 出力の故障伝搬状態を返す．
  DiffBits
  diffbits(
    SizeType tv_id,   ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
    SizeType fault_id ///< [in] 故障番号
  ) const;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実体を指す共有ポインタ
  std::shared_ptr<FsimResultsRep> mPtr;

};

END_NAMESPACE_DRUID

#endif // FSIMRESULTS_H
