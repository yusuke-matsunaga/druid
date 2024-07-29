#ifndef OPBASE_H
#define OPBASE_H

/// @file OpBase.h
/// @brief OpBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class OpBase OpBase.h "OpBase.h"
/// @brief JsonValue をオプションとして受け取るクラス用の便利関数
///
/// 実はクラスメソッドしか持たないのでこのクラスを継承してもよいし，
/// 関数として呼び出してもよい．
//////////////////////////////////////////////////////////////////////
class OpBase
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief オプション中から "debug" 属性を取り出す．
  ///
  /// * "debug" の値を持たなければ 0 を返す．
  /// * "debug" の値が int ならそのまま返す．
  /// * "debug" の値が bool なら 0/1 に変換する．
  /// * それ以外は例外を送出する．
  static
  int
  get_debug(
    const JsonValue& option ///< [in] オプション
  )
  {
    if ( option.is_object() && option.has_key("debug") ) {
      auto val = option.get("debug");
      if ( val.is_bool() ) {
	if ( val.get_bool() ) {
	  return 1;
	}
	else {
	  return 0;
	}
      }
      return val.get_int();
    }
    return 0;
  }

};

END_NAMESPACE_DRUID

#endif // OPBASE_H
