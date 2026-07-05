#ifndef DCM2HELPER_H
#define DCM2HELPER_H

/// @file DCM2Helper.h
/// @brief DCM2Helper のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "DPat.h"
#include "DichoGroup.h"
#include "DPatGraph.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DCM2Helper DCM2Helper.h "DCM2Helper.h"
/// @brief DichoCandMgr2 で用いられる補助クラス
//////////////////////////////////////////////////////////////////////
class DCM2Helper
{
public:

  // パタンをキーしたGroupの辞書
  using GroupDict = std::unordered_map<DPat, DichoGroup*>;


public:

  /// @brief コンストラクタ
  DCM2Helper() = default;

  /// @brief デストラクタ
  ~DCM2Helper() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 細分化を行う．
  /// @return 変化があったら true を返す．
  ///
  /// 結果は group_list に格納される．
  bool
  run(
    std::vector<DichoGroup::Ptr>& group_list, ///< [in] グループのリスト
    const std::vector<DPat>& dpat_array  ///< [in] 故障の検出状況のピットパタンの配列
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 後続グループに対する順序関係を求める．
  void
  add_succ_group(
    PackedVal dpat,
    const std::vector<PackedVal>& block_pats,
    const std::vector<DichoGroup*>& succ_list,
    std::vector<bool>& mark,
    std::vector<DichoGroup*>& sub_succ_list
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  /// @brief グループごとの情報
  struct GroupInfo {
    // パタンをキーにしたサブグループの辞書
    GroupDict mSubGroupDict;
    // パタンの順序関係
    DPatGraph mPatGraph;
  };

  // グループの情報
  std::vector<GroupInfo> mInfoArray;

};

END_NAMESPACE_DRUID

#endif // DCM2HELPER_H
