#ifndef FSIMRESULTS_H
#define FSIMRESULTS_H

/// @file FsimResults.h
/// @brief FsimResults のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgBase.h"
#include "types/TpgFaultList.h"
#include "fsim/DiffBits.h"


BEGIN_NAMESPACE_DRUID

class ResultsRep;

//////////////////////////////////////////////////////////////////////
/// @class FsimResults FsimResults.h "fsim/FsimResults.h"
/// @brief 故障シミュレーションの結果を表すクラス
///
/// 具体的には以下の情報のリストを持つ．
/// - テストベクタ番号
/// - 検出された故障(TpgFault)
/// - 出力の故障伝搬状態(DiffBits)
///
/// ただし，場合によっては出力の故障伝搬状態を持たないこともある．
/// このクラスの公開インターフェイスはすべて読みだし専用である．
///
/// 見せかけのコピーを効率よく行うため実体クラスを別に用意する．
/// 通常のコピー/代入では同じ実体を共有する．
//////////////////////////////////////////////////////////////////////
class FsimResults:
  public TpgBase
{
public:

  /// @brief 空のコンストラクタ
  FsimResults() = default;

  /// @brief 内容を指定するコンストラクタ
  explicit
  FsimResults(
    const std::shared_ptr<NetworkRep>& impl,
    const std::shared_ptr<ResultsRep>& rep
  );

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

  /// @brief 指定されたテストベクタ番号で検出された故障数を返す．
  SizeType
  fault_num(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const;

  /// @brief 指定されたテストベクタ番号で検出された故障を返す．
  TpgFault
  fault(
    SizeType tv_id, ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
    SizeType pos    ///< [in] 位置番号 ( 0 <= pos < fault_num(tv_id) )
  ) const;

  /// @brief 指定されたテストベクタ番号で検出された故障のリストを返す．
  TpgFaultList
  fault_list(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const;

  /// @brief 指定されたテストベクタ番号で検出された故障の出力ごとの故障伝搬状態を返す．
  ///
  /// fault は fault_list(tv_id) に含まれていなければならない．
  DiffBits
  diffbits(
    SizeType tv_id,       ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief mRep が適正かチェックする．
  void
  _check_rep() const
  {
    if ( mRep == nullptr ) {
      throw std::logic_error{"invalid data"};
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 本体
  std::shared_ptr<ResultsRep> mRep;

};

END_NAMESPACE_DRUID

#endif // FSIMRESULTS_H
