#ifndef LUADRUID_H
#define LUADRUID_H

/// @file LuaDruid.h
/// @brief LuaDruid のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/Luapp.h"



BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class LuaDruid LuaDruid.h "LuaDruid.h"
/// @brief Druid 用の Luapp 拡張
//////////////////////////////////////////////////////////////////////
class LuaDruid :
  public Luapp
{
public:

  /// @brief 空のコンストラクタ
  LuaDruid() = default;

  /// @brief 通常のコンストラクタ
  LuaDruid(
    lua_Alloc f, ///< [in] アロケーター
    void* ud      ///< [in] ユーザーデータ
  ) : Luapp{f, ud}
  {
  }

  /// @brief すでに生成済みのインタプリタを用いるコンストラクタ
  LuaDruid(
    lua_State* L ///< [in] lua インタープリタ
  ) : Luapp{L}
  {
  }

  /// @brief デストラクタ
  ~LuaDruid() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief Druid 拡張に関する初期化を行う．
  ///
  /// この関数を呼ばないと Luapp と同等になる．
  void
  open_Druid();

  /// @brief 対象が TpgNetwork の時 true を返す．
  bool
  is_tpgnetwork(
    int idx ///< [in] スタック上のインデックス
  );

  /// @brief 対象を TpgNetwork として取り出す．
  ///
  /// TpgNetwork でない時は nullptr を返す．
  TpgNetwork*
  to_tpgnetwork(
    int idx ///< [in] スタック上のインデックス
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief TpgNetwork 関係の初期化を行う．
  void
  init_TpgNetwork(
    vector<struct luaL_Reg>& mylib ///< [out] モジュールに登録する関数のリスト
  );

};

END_NAMESPACE_DRUID

#endif // LUADRUID_H
