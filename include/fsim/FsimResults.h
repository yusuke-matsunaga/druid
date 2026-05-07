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
class FsimResults:
  public TpgBase
{
public:

  /// @brief 空のコンストラクタ
  FsimResults();

  /// @brief 内容を指定するコンストラクタ(SPPFP用)
  ///
  /// src の所有権はこのオブジェクトに移る．
  explicit
  FsimResults(
    const std::shared_ptr<NetworkRep>& impl,
    FsimResultsRep* src
  );

  /// @brief 内容を指定するコンストラクタ(PPSFP用)
  ///
  /// src_lsit の中身の所有権はこのオブジェクトに移る．
  explicit
  FsimResults(
    const std::shared_ptr<NetworkRep>& impl,
    const std::vector<FsimResultsRep*>& src_list
  );

  /// @brief コピーコンストラクタ
  FsimResults(
    const FsimResults& src
  );

  /// @brief 代入演算子
  FsimResults&
  operator=(
    const FsimResults& src
  );

  /// @brief デストラクタ
  ~FsimResults();


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
  det_num(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const;

  /// @brief 指定されたテストベクタ番号で検出された故障を返す．
  TpgFault
  fault(
    SizeType tv_id, ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
    SizeType pos    ///< [in] 位置番号 ( 0 <= pos < det_num(tv_id) )
  ) const;

  /// @brief 指定されたテストベクタ番号で検出された故障の出力ごとの故障伝搬状態を返す．
  DiffBits
  diffbits(
    SizeType tv_id, ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
    SizeType pos    ///< [in] 位置番号 ( 0 <= pos < det_num(tv_id) )
  ) const;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で使用される関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テストベクタ番号が適正かチェックする．
  void
  _check_tv_id(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    if ( tv_id >= tv_num() ) {
      throw std::out_of_range{"tv_id is out of range"};
    }
  }

  /// @brief クリアする．
  void
  _clear();

  /// @brief コピーする
  void
  _copy(
    const FsimResults& src
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // テストベクタ番号ごとの結果のリスト
  std::vector<FsimResultsRep*> mArray;

};

END_NAMESPACE_DRUID

#endif // FSIMRESULTS_H
