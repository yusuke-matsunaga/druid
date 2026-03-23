
/// @file ConfigParamTest.cc
/// @brief ConfigParamTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

TEST(ConfigParamTest, null1)
{
  auto config = ConfigParam();

  auto res1 = config.get_value("key1");
  EXPECT_FALSE( res1.ans );

  EXPECT_FALSE( config.get_bool_elem("abc", false) );
  EXPECT_TRUE( config.get_bool_elem("abc", true) );

  EXPECT_EQ( 0, config.get_int_elem("xyz", 0) );
  EXPECT_EQ( 1, config.get_int_elem("xyz", 1) );

  EXPECT_EQ( "", config.get_string_elem("def", "") );
  EXPECT_EQ( "123", config.get_string_elem("def", "123") );
}

TEST(ConfigParamTest, null2)
{
  auto json_obj = JsonValue::null();
  auto config = ConfigParam(json_obj);

  auto res1 = config.get_value("key1");
  EXPECT_FALSE( res1.ans );

  EXPECT_FALSE( config.get_bool_elem("abc", false) );
  EXPECT_TRUE( config.get_bool_elem("abc", true) );

  EXPECT_EQ( 0, config.get_int_elem("xyz", 0) );
  EXPECT_EQ( 1, config.get_int_elem("xyz", 1) );

  EXPECT_EQ( "", config.get_string_elem("def", "") );
  EXPECT_EQ( "123", config.get_string_elem("def", "123") );
}

TEST(ConfigParamTest, simple1)
{
  auto json_obj = JsonValue::object();
  json_obj.add("abc", true);
  json_obj.add("def", 123);
  json_obj.add("ghi", std::string{"xyz"});

  auto config = ConfigParam(json_obj);

  auto res1 = config.get_value("abc");
  ASSERT_TRUE( res1.ans );
  EXPECT_EQ( true, res1.value.get_bool() );
  EXPECT_EQ( true, config.get_bool_elem("abc", false) );

  auto res2 = config.get_value("def");
  ASSERT_TRUE( res2.ans );
  EXPECT_EQ( 123, res2.value.get_int() );
  EXPECT_EQ( 123, config.get_int_elem("def", 0) );

  auto res3 = config.get_value("ghi");
  ASSERT_TRUE( res3.ans );
  EXPECT_EQ( "xyz", res3.value.get_string() );
  EXPECT_EQ( "xyz", config.get_string_elem("ghi", "") );
}

TEST(ConfigParamTest, layered)
{
  auto child_obj = JsonValue::object();
  child_obj.add("abc", true);
  child_obj.add("def", 123);
  child_obj.add("ghi", std::string{"xyz"});

  auto default_obj = JsonValue::object();
  default_obj.add("jkl", std::string{"default_val"});

  auto json_obj = JsonValue::object();
  json_obj.add("child", child_obj);
  json_obj.add("*", default_obj);

  auto config = ConfigParam(json_obj);

  auto child_config = config.get_param("child");

  auto res1 = child_config.get_value("abc");
  ASSERT_TRUE( res1.ans );
  EXPECT_EQ( true, res1.value.get_bool() );
  EXPECT_EQ( true, child_config.get_bool_elem("abc", false) );

  auto res2 = child_config.get_value("def");
  ASSERT_TRUE( res2.ans );
  EXPECT_EQ( 123, res2.value.get_int() );
  EXPECT_EQ( 123, child_config.get_int_elem("def", 0) );

  auto res3 = child_config.get_value("ghi");
  ASSERT_TRUE( res3.ans );
  EXPECT_EQ( "xyz", res3.value.get_string() );
  EXPECT_EQ( "xyz", child_config.get_string_elem("ghi", "") );

  auto res4 = child_config.get_value("jkl");
  ASSERT_TRUE( res4.ans );
  EXPECT_EQ( "default_val", res4.value.get_string() );
  EXPECT_EQ( "default_val", child_config.get_string_elem("jkl") );
}

END_NAMESPACE_DRUID
