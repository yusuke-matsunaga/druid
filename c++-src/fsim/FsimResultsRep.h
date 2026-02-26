#ifndef FSIMRESULTSREP_H
#define FSIMRESULTSREP_H

/// @file FsimResultsRep.h
/// @brief FsimResultsRep のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FsimResultsRep FsimResultsRep.h "FsimResultsRep.h"
/// @brief FsimResults の実体
///
/// 意味的には
/// - テストベクタ番号
/// - 故障番号
/// - 出力の伝搬状態
/// を要素としたリストだが，
/// テストベクタ番号ごとに操作する時に効率がよくなるように実装する．
//////////////////////////////////////////////////////////////////////
class FsimResultsRep
{
public:

  /// @brief コンストラクタ
  explicit
  FsimResultsRep(
    SizeType num = 1 ///< [in] 確保するベクタ数
  )
  {
    new_tv(num);
  }

  /// @brief 複数の結果をマージした結果を作るクラスメソッド
  static
  std::shared_ptr<FsimResultsRep>
  merge(
    const std::vector<const FsimResultsRep*>& src_list
  )
  {
    SizeType n = src_list.size();
    // 常に1つは要素があるはず．
    auto src0 = src_list.front();
    SizeType m = src0->tv_num();
    auto res = std::shared_ptr<FsimResultsRep>{new FsimResultsRep(m)};
    for ( auto src: src_list ) {
      if ( src->tv_num() != m ) {
	throw std::invalid_argument{"tv_num() mismatch"};
      }
      for ( SizeType i = 0; i < m; ++ i ) {
	_merge_elem(res->mElemList[i], src->mElemList[i]);
      }
    }
    return res;
  }

  /// @brief デストラクタ
  ~FsimResultsRep() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 新しいテストベクタを追加する．
  void
  new_tv(
    SizeType num = 1 ///< [in] 追加するベクタ数
  )
  {
    for ( SizeType i = 0; i < num; ++ i ) {
      mElemList.push_back(ElemType{});
    }
  }

  /// @brief 要素を追加する．
  void
  add(
    SizeType tv_id,          ///< [in] テストベクタ番号
    SizeType fault_id,       ///< [in] 故障番号
    const DiffBits& diffbits ///< [in] 出力の故障伝搬状態
  )
  {
    _check_tv_id(tv_id);
    auto& elem = mElemList[tv_id];
    elem.fault_list.push_back(fault_id);
    elem.dbits_dict.emplace(fault_id, diffbits);
  }

  /// @brief 結果を末尾に追加する．
  void
  append(
    const FsimResultsRep* src ///< [in] 追加するオブジェクト
  )
  {
    mElemList.insert(mElemList.end(),
		     src->mElemList.begin(), src->mElemList.end());
  }

  /// @brief fault_list を整列させる．
  void
  sort()
  {
    for ( auto& elem: mElemList ) {
      auto& fault_list = elem.fault_list;
      std::sort(fault_list.begin(), fault_list.end());
    }
  }

  /// @brief テストベクタ数を返す．
  SizeType
  tv_num() const
  {
    return mElemList.size();
  }

  /// @brief 指定されたテストベクタ番号で検出された故障番号のリストを返す．
  std::vector<SizeType>
  fault_list(
    SizeType tv_id ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    _check_tv_id(tv_id);
    return mElemList[tv_id].fault_list;
  }

  /// @brief 出力の故障伝搬状態を返す．
  DiffBits
  diffbits(
    SizeType tv_id,   ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
    SizeType fault_id ///< [in] 故障番号
  ) const
  {
    _check_tv_id(tv_id);
    auto& dbits_dict = mElemList[tv_id].dbits_dict;
    if ( dbits_dict.count(fault_id) == 0 ) {
      abort();
      throw std::out_of_range{"fault_id was not found"};
    }
    return dbits_dict.at(fault_id);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  // 一つのテストベクタに対する結果を表す構造体
  struct ElemType {
    // 検出された故障番号のリスト
    std::vector<SizeType> fault_list;
    // 故障番号をキーにして DiffBits を格納した辞書
    std::unordered_map<SizeType, DiffBits> dbits_dict;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ElemType の内容をマージする．
  static
  void
  _merge_elem(
    ElemType& dst,
    const ElemType& src
  )
  {
    auto& dst_list = dst.fault_list;
    auto& src_list = src.fault_list;
    dst_list.insert(dst_list.end(),
		    src_list.begin(),
		    src_list.end());
    for ( auto fid: src_list ) {
      auto dbits = src.dbits_dict.at(fid);
      dst.dbits_dict.emplace(fid, dbits);
    }
  }

  /// @brief tv_id の範囲チェックを行う．
  void
  _check_tv_id(
    SizeType tv_id    ///< [in] テストベクタ番号 ( 0 <= tv_id < tv_num() )
  ) const
  {
    if ( tv_id >= tv_num() ) {
      throw std::out_of_range{"tv_id is out of range"};
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 本体
  std::vector<ElemType> mElemList;

};

END_NAMESPACE_DRUID

#endif // FSIMRESULTSREP_H
