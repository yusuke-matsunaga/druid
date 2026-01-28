#ifndef DETCOND_H
#define DETCOND_H

/// @file DetCond.h
/// @brief DetCond のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/Literal.h"
#include "types/TpgNode.h"
#include "types/TpgNodeList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DetCond DetCond.h "DetCond.h"
/// @brief 故障検出条件を表すクラス
/// @ingroup CondgenGroup
///
/// mandatory_condition & (cube_1 | cube_2 | ... | cube_n)
/// の形で表される．
/// mandatory_condition, cube_i はそれぞれ AssignList で表される．
//////////////////////////////////////////////////////////////////////
class DetCond
{
public:

  /// @brief 結果の種類
  enum Type: std::uint8_t {
    Undetected,      ///< 検出できなかった．
    Detected,        ///< 検出された．
    PartialDetected, ///< 部分的に検出された．
    Overflow         ///< 繰り返し回数をオーバーした．
  };

  /// @brief 条件を表す構造体
  struct CondData {
    std::vector<Literal> mand_cond;         ///< 必要条件
    std::vector<std::vector<Literal>> cube_list; ///< 十分条件のリスト

    void
    print(
      std::ostream& s
    ) const
    {
      s << "  mand_cond:";
      for ( auto lit: mand_cond ) {
	s << " " << lit;
      }
      s << std::endl;
      auto n = cube_list.size();
      for ( SizeType i = 0; i < n; ++ i ) {
	s << "  cube#" << i << ":";
	for ( auto lit: cube_list[i] ) {
	  s << " " << lit;
	}
	s << std::endl;
      }
    }

  };


public:

  /// @brief 空のコンストラクタ
  DetCond() = default;

  /// @brief Undetected を返すクラスメソッド
  static
  DetCond
  undetected(
    SizeType id,        ///< [in] FFR 番号
    const TpgNode& root ///< [in] 根のノード
  )
  {
    return DetCond(id, root);
  }

  /// @brief Detected タイプを返すクラスメソッド
  static
  DetCond
  detected(
    SizeType id,         ///< [in] FFR 番号
    const TpgNode& root, ///< [in] 根のノード
    const CondData& cond ///< [in] 全体の条件
  )
  {
    return DetCond(id, root, cond);
  }

  /// @brief PartialDetected を返すクラスメソッド
  static
  DetCond
  partial_detected(
    SizeType id,                       ///< [in] FFR 番号
    const TpgNode& root,               ///< [in] 根のノード
    const std::vector<CondData>& cond_list, ///< [in] 個々の出力ごとの条件のリスト
    const TpgNodeList& output_list = {}     ///< [in] オーバーフローした出力のリスト
  )
  {
    return DetCond(id, root, cond_list, output_list);
  }

  /// @brief Overflow を返すクラスメソッド
  static
  DetCond
  overflow(
    SizeType id,                   ///< [in] FFR 番号
    const TpgNode& root,           ///< [in] 根のノード
    const TpgNodeList& output_list ///< [in] オーバーフローした出力のリスト
  )
  {
    return DetCond(id, root, output_list);
  }

  /// @brief デストラクタ
  ~DetCond() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFR 番号を返す．
  SizeType
  ffr_id() const
  {
    return mId;
  }

  /// @brief 根のノードを返す．
  const TpgNode&
  root() const
  {
    return mRoot;
  }

  /// @brief タイプを返す．
  Type
  type() const
  {
    return mType;
  }

  /// @brief 全体の条件を得る．
  const CondData&
  cond() const
  {
    return mCond;
  }

  /// @brief 個々の出力ごとの条件を得る．
  const std::vector<CondData>&
  cond_list() const
  {
    return mCondList;
  }

  /// @brief オーバーフローした出力のリスト
  const TpgNodeList&
  output_list() const
  {
    return mOutputList;
  }

  /// @brief 内容を出力する．
  void
  print(
    std::ostream& s
  ) const
  {
    s << "FFR#" << ffr_id() << ": ";
    switch ( type() ) {
    case DetCond::Undetected:
      s << "Undetected" << std::endl;
      break;

    case DetCond::Detected:
      s << "Detected:" << std::endl;
      mCond.print(s);
      break;

    case DetCond::PartialDetected:
      s << "PartialDetected:" << std::endl;
      {
	auto nc = mCondList.size();
	for ( SizeType i = 0; i < nc; ++ i ) {
	  s << "  cond#" << i << std::endl;
	  mCondList[i].print(s);
	}
	auto no = mOutputList.size();
	for ( SizeType i = 0; i < no; ++ i ) {
	  s << "  output#" << i << ": " << mOutputList[i].id() << std::endl;
	}
      }
      break;

    case DetCond::Overflow:
      s << "Overflow:" << std::endl;
      {
	auto no = mOutputList.size();
	for ( SizeType i = 0; i < no; ++ i ) {
	  s << "  output#" << i << ": " << mOutputList[i].id() << std::endl;
	}
      }
      break;
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief Undetected 用のコンストラクタ
  DetCond(
    SizeType id,
    const TpgNode& root
  ) : mId{id},
      mRoot{root},
      mType{Undetected}
  {
  }

  /// @brief Detected 用のコンストラクタ
  DetCond(
    SizeType id,
    const TpgNode& root,
    const CondData& cond
  ) : mId{id},
      mRoot{root},
      mType{Detected},
      mCond{cond}
  {
  }

  /// @brief PartialDetected 用のコンストラクタ
  DetCond(
    SizeType id,
    const TpgNode& root,
    const std::vector<CondData>& cond_list,
    const TpgNodeList& output_list
  ) : mId{id},
      mRoot{root},
      mType{PartialDetected},
      mCondList{cond_list},
      mOutputList{output_list}
  {
  }

  /// @brief Overflow 用のコンストラクタ
  DetCond(
    SizeType id,
    const TpgNode& root,
    const TpgNodeList& output_list
  ) : mId{id},
      mRoot{root},
      mType{Overflow},
      mOutputList{output_list}
  {
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFR 番号
  SizeType mId;

  // 根のノード
  TpgNode mRoot;

  // タイプ
  Type mType{Undetected};

  // 条件
  CondData mCond;

  // 個々の出力ごとの条件
  std::vector<CondData> mCondList;

  // overflow した出力のリスト
  TpgNodeList mOutputList;

};

/// @brief ストリーム出力
inline
std::ostream&
operator<<(
  std::ostream& s,
  const DetCond& cond
)
{
  cond.print(s);
  return s;
}

END_NAMESPACE_DRUID

#endif // DETCOND_H
