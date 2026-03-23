#ifndef CONFIGPARAM_H
#define CONFIGPARAM_H

/// @file ConfigParam.h
/// @brief ConfigParam のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ConfigParam ConfigParam.h "ConfigParam.h"
/// @brief JsonValue を用いた設定パラメータを表すクラス
///
/// 基本的には JsonValue だが，階層的な指定がされていた場合に
/// ワイルドカードを用いたマッチングを行えるように工夫したもの
///
/// mValueList には object 型の JsonValue のみが格納される．
/// なので空の場合も null 型ではなく要素を持たない object 型となる．
//////////////////////////////////////////////////////////////////////
class ConfigParam
{
public:

  /// @brief 結果を表す型
  struct Result {
    bool ans{false}; ///< 結果を持つ時 true
    JsonValue value; ///< 値

    /// @brief ans = false の時に JsonValue::null() に変換するアダプタ
    operator JsonValue() const
    {
      if ( ans ) {
	return value;
      }
      return JsonValue::null();
    }
  };


public:

  /// @brief 空のコンストラクタ
  ///
  /// 空の object 型となる．
  ConfigParam()
  {
    mValueList.push_back(JsonValue::object());
  }

  /// @brief 値を持つコンストラクタ
  ///
  /// explicit を付けていないので暗黙の変換コンストラクタになる．
  ConfigParam(
    const JsonValue& json_obj ///< [in] 元となる JSON オブジェクト
  )
  {
    if ( json_obj.is_object() ) {
      mValueList.push_back(json_obj);
    }
    else {
      // object 型でない場合は無視する．
      mValueList.push_back(JsonValue::object());
    }
  }

  /// @brief デストラクタ
  ~ConfigParam() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief key という属性の値を返す．
  Result
  get_value(
    const std::string& key
  ) const
  {
    for ( auto& value: mValueList ) {
      if ( value.has_key(key) ) {
	return Result{true, value.at(key)};
      }
    }
    return Result();
  }

  /// @brief key_list に合致するの値を返す．
  ///
  /// key_list に含まれるキーを順に試す．
  Result
  get_value(
    const std::vector<std::string>& key_list
  ) const
  {
    for ( auto& key: key_list ) {
      auto res = get_value(key);
      if ( res.ans ) {
	return res;
      }
    }
    return Result();
  }

  /// @brief key に合致する bool 値を返す．
  bool
  get_bool_elem(
    const std::string& key,        ///< [in] キー
    const bool default_val = false ///< [in] デフォルト値
  ) const
  {
    auto res = get_value(key);
    if ( res.ans ) {
      if ( res.value.is_bool() ) {
	return res.value.get_bool();
      }
      std::ostringstream buf;
      buf << key << " should be a bool";
      throw std::invalid_argument{buf.str()};
    }
    return default_val;
  }

  /// @brief key に合致する int 値を返す．
  int
  get_int_elem(
    const std::string& key,    ///< [in] キー
    const int default_val = {} ///< [in] デフォルト値
  ) const
  {
    auto res = get_value(key);
    if ( res.ans ) {
      if ( res.value.is_int() ) {
	return res.value.get_int();
      }
      std::ostringstream buf;
      buf << key << " should be an int";
      throw std::invalid_argument{buf.str()};
    }
    return default_val;
  }

  /// @brief key に合致する string 値を返す．
  std::string
  get_string_elem(
    const std::string& key,             ///< [in] キー
    const std::string& default_val = {} ///< [in] デフォルト値
  ) const
  {
    auto res = get_value(key);
    if ( res.ans ) {
      if ( res.value.is_string() ) {
	return res.value.get_string();
      }
      std::ostringstream buf;
      buf << key << " should be a string";
      throw std::invalid_argument{buf.str()};
    }
    return default_val;
  }

  /// @brief key に合致するパラメータを返す．
  ConfigParam
  get_param(
    const std::string& key ///< [in] キー
  ) const
  {
    return get_param(std::vector<std::string>{key});
  }

  /// @brief key_list に合致するパラメータを返す．
  ConfigParam
  get_param(
    const std::vector<std::string>& key_list ///< [in] キーのリスト
  ) const
  {
    std::vector<JsonValue> value_list;
    for ( auto& key: key_list ) {
      _get_param(key, value_list);
    }
    // 最後はワイルドカード
    _get_param("*", value_list);
    return ConfigParam(value_list);
  }

  /// @brief 内容を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const
  {
    for ( auto& value: mValueList ) {
      s << value.to_json() << std::endl;
    }
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 要素を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief オブジェクト型に要素を追加する．
  void
  add(
    const std::string& key, ///< [in] キー
    const JsonValue& value  ///< [in] 追加する要素
  )
  {
    for ( auto& js_value: mValueList ) {
      js_value.add(key, value);
    }
  }

  /// @brief オブジェクト型に要素を追加する．
  void
  add(
    const std::string& key,  ///< [in] キー
    const std::string& value ///< [in] 追加する要素
  )
  {
    add(key, JsonValue(value));
  }

  /// @brief オブジェクト型に要素を追加する．
  void
  add(
    const std::string& key, ///< [in] キー
    int value               ///< [in] 追加する要素
  )
  {
    add(key, JsonValue(value));
  }

  /// @brief オブジェクト型に要素を追加する．
  void
  add(
    const std::string& key, ///< [in] キー
    double value            ///< [in] 追加する要素
  )
  {
    add(key, JsonValue(value));
  }

  /// @brief オブジェクト型に要素を追加する．
  void
  add(
    const std::string& key, ///< [in] キー
    bool value              ///< [in] 追加する要素
  )
  {
    add(key, JsonValue(value));
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を指定したコンストラクタ
  ConfigParam(
    const std::vector<JsonValue>& value_list ///< [in] 値のリスト
  ) : mValueList{value_list}
  {
  }

  /// @brief get_param() の下請け関数
  void
  _get_param(
    const std::string& key,            ///< [in] キー
    std::vector<JsonValue>& value_list ///< [out] 結果を格納する変数
  ) const
  {
    for ( auto& value: mValueList ) {
      if ( value.has_key(key) ) {
	auto param = value.at(key);
	if ( param.is_object() ) {
	  value_list.push_back(param);
	}
      }
      else {
	value_list.push_back(value);
      }
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // マッチする可能性のある値のリスト
  std::vector<JsonValue> mValueList;

};

END_NAMESPACE_DRUID

#endif // CONFIGPARAM_H
