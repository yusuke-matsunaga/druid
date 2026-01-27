#ifndef OPBASE_H
#define OPBASE_H

/// @file OpBase.h
/// @brief OpBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
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

  /// @brief キーワードに対応するオプションを取り出す．
  ///
  /// option がオブジェクト型でない場合とキーワードに対応する値がない
  /// 場合には JsonValue::null() が返される．
  static
  JsonValue
  get_option(
    const JsonValue& option, ///< [in] オプションを表すJSONオブジェクト
    const char* keyword	     ///< [in] キーワード
  )
  {
    if ( option.is_object() && option.has_key(keyword) ) {
      return option.get(keyword);
    }
    return JsonValue::null();
  }

  /// @brief bool型のオプションを取り出す．
  ///
  /// - keyword の値が存在しない場合には何もしない．
  /// - keyword の値がブール値なら value に設定する．
  /// - keyword の値がブール値以外なら std::invalid_argument 例外を送出する．
  static
  void
  get_bool(
    const JsonValue& option,
    const std::string& keyword,
    bool& value
  )
  {
    if ( option.has_key(keyword) ) {
      auto value_obj = option.at(keyword);
      if ( !value_obj.is_bool() ) {
	std::ostringstream buf;
	buf << "'" << keyword << "' should be a bool";
	throw std::invalid_argument{buf.str()};
      }
      value = value_obj.get_bool();
    }
  }

  /// @brief 文字列型のオプションを取り出す．
  ///
  /// - keyword の値が存在しない場合は何もしない．
  /// - keyword の値が文字列なら value に設定する．
  /// - keyword の値が文字列でなかったら std::invalid_argument 例外を送出する．
  static
  void
  get_string(
    const JsonValue& option,    ///< [in] オプションを表すJSONオブジェクト
    const std::string& keyword, ///< [in] キーワード
    std::string& value          ///< [out] 値を格納する変数
  )
  {
    if ( option.is_object() && option.has_key(keyword) ) {
      auto value_obj = option.at(keyword);
      if ( !value_obj.is_string() ) {
	std::ostringstream buf;
	buf << "'" << keyword << "' should be a string";
	throw std::invalid_argument{buf.str()};
      }
      value = value_obj.get_string();
    }
  }

  /// @brief 整数型のオプションを取り出す．
  ///
  /// * keyword の値を持たなければ何もしない．
  /// * keyword の値が int なら value に設定する．
  /// * それ以外は例外を送出する．
  static
  void
  get_int(
    const JsonValue& option,    ///< [in] オプション
    const std::string& keyword, ///< [in] キーワード
    int& value                  ///< [out] 値を格納する変数
  )
  {
    if ( option.is_object() && option.has_key(keyword) ) {
      auto value_obj = option.get(keyword);
      if ( !value_obj.is_int() ) {
	std::ostringstream buf;
	buf << "'" << keyword << "' should be an integer";
	throw std::invalid_argument{buf.str()};
      }
      value = value_obj.get_int();
    }
  }

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
