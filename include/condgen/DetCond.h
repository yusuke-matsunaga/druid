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
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DetCond DetCond.h "DetCond.h"
/// @brief 故障検出条件を表すクラス
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
    vector<Literal> mand_cond;         ///< 必要条件
    vector<vector<Literal>> cube_list; ///< 十分条件のリスト

    void
    print(
      ostream& s
    ) const
    {
      s << "  mand_cond:";
      for ( auto lit: mand_cond ) {
	s << " " << lit;
      }
      s << endl;
      auto n = cube_list.size();
      for ( SizeType i = 0; i < n; ++ i ) {
	s << "  cube#" << i << ":";
	for ( auto lit: cube_list[i] ) {
	  s << " " << lit;
	}
	s << endl;
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
    const TpgNode* root ///< [in] 根のノード
  )
  {
    return DetCond(root);
  }

  /// @brief Detected タイプを返すクラスメソッド
  static
  DetCond
  detected(
    const TpgNode* root, ///< [in] 根のノード
    const CondData& cond ///< [in] 全体の条件
  )
  {
    return DetCond(root, cond);
  }

  /// @brief PartialDetected を返すクラスメソッド
  static
  DetCond
  partial_detected(
    const TpgNode* root,                           ///< [in] 根のノード
    const vector<CondData>& cond_list,             ///< [in] 個々の出力ごとの条件のリスト
    const vector<const TpgNode*>& output_list = {} ///< [in] オーバーフローした出力のリスト
  )
  {
    return DetCond(root, cond_list, output_list);
  }

  /// @brief Overflow を返すクラスメソッド
  static
  DetCond
  overflow(
    const TpgNode* root,                      ///< [in] 根のノード
    const vector<const TpgNode*>& output_list ///< [in] オーバーフローした出力のリスト
  )
  {
    return DetCond(root, output_list);
  }

  /// @brief デストラクタ
  ~DetCond() = default;


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
  const vector<CondData>&
  cond_list() const
  {
    return mCondList;
  }

  /// @brief オーバーフローした出力のリスト
  const vector<const TpgNode*>&
  output_list() const
  {
    return mOutputList;
  }

  /// @brief 内容を出力する．
  void
  print(
    ostream& s
  ) const
  {
    switch ( type() ) {
    case DetCond::Undetected:
      s << "Undetected" << endl;
      break;

    case DetCond::Detected:
      s << "Detected:" << endl;
      mCond.print(s);
      break;

    case DetCond::PartialDetected:
      s << "PartialDetected:" << endl;
      {
	auto nc = mCondList.size();
	for ( SizeType i = 0; i < nc; ++ i ) {
	  s << "  cond#" << i << endl;
	  mCondList[i].print(s);
	}
	auto no = mOutputList.size();
	for ( SizeType i = 0; i < no; ++ i ) {
	  s << "  output#" << i << ": " << mOutputList[i]->id() << endl;
	}
      }
      break;

    case DetCond::Overflow:
      s << "Overflow:" << endl;
      {
	auto no = mOutputList.size();
	for ( SizeType i = 0; i < no; ++ i ) {
	  s << "  output#" << i << ": " << mOutputList[i]->id() << endl;
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
    const TpgNode* root
  ) : mRoot{root},
      mType{Undetected}
  {
  }

  /// @brief Detected 用のコンストラクタ
  DetCond(
    const TpgNode* root,
    const CondData& cond
  ) : mRoot{root},
      mType{Detected},
      mCond{cond}
  {
  }

  /// @brief PartialDetected 用のコンストラクタ
  DetCond(
    const TpgNode* root,
    const vector<CondData>& cond_list,
    const vector<const TpgNode*>& output_list
  ) : mRoot{root},
      mType{PartialDetected},
      mCondList{cond_list},
      mOutputList{output_list}
  {
  }

  /// @brief Overflow 用のコンストラクタ
  DetCond(
    const TpgNode* root,
    const vector<const TpgNode*>& output_list
  ) : mRoot{root},
      mType{Overflow},
      mOutputList{output_list}
  {
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 根のノード
  const TpgNode* mRoot{nullptr};

  // タイプ
  Type mType{Undetected};

  // 条件
  CondData mCond;

  // 個々の出力ごとの条件
  vector<CondData> mCondList;

  // overflow した出力のリスト
  vector<const TpgNode*> mOutputList;

};

/// @brief ストリーム出力
inline
ostream&
operator<<(
  ostream& s,
  const DetCond& cond
)
{
  cond.print(s);
  return s;
}

END_NAMESPACE_DRUID

#endif // DETCOND_H
