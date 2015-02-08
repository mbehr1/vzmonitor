#include <json-c/json.h>
#include "gtest/gtest.h"

#include "rules.hpp"
#include "channel_data.hpp"

TEST(ut_rules, generate_cond_const)
{
	struct json_object *jso = json_tokener_parse
			("true");

	Condition *cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_TRUE(cond->evaluate());
	delete cond;

	jso = json_tokener_parse("false");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_FALSE(cond->evaluate());
	delete cond;
}

TEST(ut_rules, generate_cond_not)
{
	struct json_object *jso = json_tokener_parse
			("{\"NOT\":[true]}");

	Condition *cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_FALSE(cond->evaluate());
	delete cond;

	jso = json_tokener_parse("{\"NOT\":[false]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_TRUE(cond->evaluate());
	delete cond;
}


TEST(ut_rules, generate_cond_and)
{
	struct json_object *jso = json_tokener_parse
			("{\"AND\":[]}");

	Condition *cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_FALSE(cond->evaluate()); // empty AND shall return false
	delete cond;

	jso = json_tokener_parse("{\"AND\":[true]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_TRUE(cond->evaluate()); // unary AND -> expression (here true)
	delete cond;

	jso = json_tokener_parse("{\"AND\":[false]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_FALSE(cond->evaluate()); // unary AND -> expression (here false)
	delete cond;

	jso = json_tokener_parse("{\"AND\":[true, true]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	EXPECT_TRUE(cond->evaluate()); // binary AND
	delete cond;

	jso = json_tokener_parse("{\"AND\":[true, false]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	EXPECT_FALSE(cond->evaluate()); // binary AND
	delete cond;

	jso = json_tokener_parse("{\"AND\":[false, true]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	EXPECT_FALSE(cond->evaluate()); // binary AND
	delete cond;

	jso = json_tokener_parse("{\"AND\":[true, true, true]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	EXPECT_TRUE(cond->evaluate()); // binary AND
	delete cond;

	jso = json_tokener_parse("{\"AND\":[true, true, false]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	EXPECT_FALSE(cond->evaluate()); // binary AND
	delete cond;

}

TEST(ut_rules, generate_cond_or)
{
	struct json_object *jso = json_tokener_parse
			("{\"OR\":[]}");

	Condition *cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_FALSE(cond->evaluate()); // empty OR shall return false
	delete cond;

	jso = json_tokener_parse("{\"OR\":[true]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_TRUE(cond->evaluate()); // unary OR -> expression (here true)
	delete cond;

	jso = json_tokener_parse("{\"OR\":[false]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_FALSE(cond->evaluate()); // unary OR -> expression (here false)
	delete cond;

	jso = json_tokener_parse("{\"OR\":[true, true]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	EXPECT_TRUE(cond->evaluate()); // binary OR
	delete cond;

	jso = json_tokener_parse("{\"OR\":[true, false]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	EXPECT_TRUE(cond->evaluate()); // binary OR
	delete cond;

	jso = json_tokener_parse("{\"OR\":[false, true]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	EXPECT_TRUE(cond->evaluate()); // binary OR
	delete cond;

	jso = json_tokener_parse("{\"OR\":[true, true, true]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	EXPECT_TRUE(cond->evaluate()); // binary
	delete cond;

	jso = json_tokener_parse("{\"OR\":[true, true, false]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	EXPECT_TRUE(cond->evaluate()); // binary
	delete cond;

	jso = json_tokener_parse("{\"OR\":[false, false, false]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	EXPECT_FALSE(cond->evaluate()); // binary
	delete cond;

}

TEST(ut_rules, generate_cond_gt)
{
	struct json_object *jso = json_tokener_parse
			("{\"GT\":[1.0, 2.0]}");

	Condition *cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_FALSE(cond->evaluate());
	delete cond;

	jso = json_tokener_parse("{\"GT\":[2.0, 1.0]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_TRUE(cond->evaluate());
	delete cond;
}

TEST(ut_rules, generate_cond_lte)
{
	struct json_object *jso = json_tokener_parse
			("{\"LTE\":[1.0, 2.0]}");

	Condition *cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_TRUE(cond->evaluate());
	delete cond;

	jso = json_tokener_parse("{\"LTE\":[2.0, 1.0]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_FALSE(cond->evaluate());
	delete cond;

	jso = json_tokener_parse("{\"LTE\":[-2.0, -2]}"); // here with int
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_TRUE(cond->evaluate());
	delete cond;

}

TEST(ut_rules, generate_cond_plus)
{
	struct json_object *jso = json_tokener_parse
			("{\"EQ\":[2, {\"+\":[1, 1.0]}]}");

	Condition *cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	EXPECT_TRUE(cond->evaluate());
	delete cond;

}


TEST(ut_rules, generate_cond_now)
{
	struct json_object *jso;

	Condition *cond;

	jso = json_tokener_parse("{\"LTE\":[\"NOW\", \"NOW\"]}"); // first call is early. so first call <= 2nd call
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_TRUE(cond->evaluate());
	delete cond;
}

MAP_ChannelDataList gChannelData;

TEST(ut_rules, generate_cond_lasttime)
{
	struct json_object *jso;
	Condition *cond;

	gChannelData["Ch1"].push_back(ChannelData(100, -5.0));

	jso = json_tokener_parse("{\"EQ\":[100, {\"LASTTIME\":\"Ch1\"}]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_TRUE(cond->evaluate());
	delete cond;
	gChannelData.clear();
}
