#include <json-c/json.h>
#include <cmath>
#include "gtest/gtest.h"

#include "rules.hpp"
#include "channel_data.hpp"
#include "functions.hpp"
#include "config_options.hpp"

GlobalOptions *gGlobalOptions=0;

TEST(ut_rules, generate_cond_const)
{
	struct json_object *jso = json_tokener_parse
			("true");

	Condition *cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	std::cout << *cond;
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
	std::cout << *cond;
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
	std::cout << *cond;
	ASSERT_FALSE(cond->evaluate()); // empty AND shall return false
	delete cond;

	jso = json_tokener_parse("{\"AND\":[true]}");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	std::cout << *cond;
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
	std::cout << *cond;
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
	std::cout << *cond;
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

TEST(ut_rules, generate_cond_minvalue)
{
	struct json_object *jso;
	Condition *cond;

	jso = json_tokener_parse("{\"EQ\":[-5, {\"MINVALUE\":[\"Ch1\", 10] } ] }");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_FALSE(cond->evaluate());

	double now = f_now();

	gChannelData["Ch1"].push_back(ChannelData(now-4, -5.0));

	jso = json_tokener_parse("{\"EQ\":[-5, {\"MINVALUE\":[\"Ch1\", 10] } ] }");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_TRUE(cond->evaluate());
	delete cond;

	gChannelData["Ch1"].push_back(ChannelData(now-3, -4.0));

	jso = json_tokener_parse("{\"EQ\":[-5, {\"MINVALUE\":[\"Ch1\", 10] } ] }");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_TRUE(cond->evaluate());
	delete cond;

	gChannelData["Ch1"].push_back(ChannelData(now-2.5, -6.0));

	jso = json_tokener_parse("{\"EQ\":[-6, {\"MINVALUE\":[\"Ch1\", 10] } ] }");
	cond = generate_condition(jso);
	json_object_put(jso);
	ASSERT_TRUE(cond!=0);
	ASSERT_TRUE(cond->evaluate());
	delete cond;


	gChannelData.clear();
}

TEST(ut_rules, generate_function_avgvalue)
{
	// if no data, avgvalue = NAN?
	double v = f_avgvalue("Ch1", 10);
	ASSERT_TRUE(isnan(v));

	double now = f_now(); // TODO the f_now() should use a constant value and not the real one to keep it constant between calls in one rule

	gChannelData["Ch1"].push_back(ChannelData(now-10, -5.0));

	// one data item: return this as avg
	v = f_avgvalue("Ch1", 11);
	ASSERT_EQ(v, -5.0);

	v = f_avgvalue("Ch1", 5); // even if not inside the window anymore
	ASSERT_EQ(v, -5.0);

	// two data items, same distance (5s -5, 5s 0, but window bigger (so before first data item) -> -10/3
	gChannelData["Ch1"].push_back(ChannelData(now-5, 0.0));

	v = f_avgvalue("Ch1", 15);

	ASSERT_TRUE(fabs(v+(10/3.0))<0.0001);

	// two data items, same distance but one out of averaging window (4s -5, 5s 0 -> -2.2

	v = f_avgvalue("Ch1", 9);
	double expected = ((-5*4)+(0*5))/9.0;

	ASSERT_TRUE(fabs(v-expected)<0.001) << expected;

	gChannelData.clear();
}

TEST(ut_rules, generate_function_maxvalue)
{
	// if no data, maxvalue = NAN?
	double v = f_maxvalue("Ch1", 10);
	ASSERT_TRUE(isnan(v));

	double now = f_now(); // TODO the f_now() should use a constant value and not the real one to keep it constant between calls in one rule

	gChannelData["Ch1"].push_back(ChannelData(now-10, -5.0));

	// one data item: return this as max (even if outside the window)
	v = f_maxvalue("Ch1", 11);
	ASSERT_EQ(v, -5.0);

	v = f_maxvalue("Ch1", 5); // even if not inside the window anymore
	ASSERT_EQ(v, -5.0);

	// two data items, same distance (5s -5, 5s 0, but window bigger (so before first data item) -> -10/3
	gChannelData["Ch1"].push_back(ChannelData(now-5, -6.0));

	v = f_maxvalue("Ch1", 15);

	ASSERT_EQ(v, -5.0);

	v = f_maxvalue("Ch1", 6);

	ASSERT_EQ(v, -6.0);

	gChannelData.clear();
}
