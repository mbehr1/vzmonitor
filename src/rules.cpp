#include <json-c/json.h>
#include <stdio.h>
#include <string.h>
#include <functional>
#include "rules.hpp"
#include "condition.hpp"
#include "functions.hpp"
#include "config_options.hpp"

void generate_values_lhs_rhs(struct json_object *cond, Value *&lhs, Value *&rhs);

std::list<Condition *> *generate_condlist(struct json_object *cond)
{
    // here we assume a json_type_array
    if (!cond) return 0;
    if (json_object_get_type(cond)!=json_type_array) throw "generate_condlist not an array!";

    std::list<Condition *> *list = new std::list<Condition *>();

	int nrconds;
	if ((nrconds=json_object_array_length(cond))>=1) {
		// for each object:
		for (int i = 0; i < nrconds; i++) {
			struct json_object *jb = json_object_array_get_idx(cond, i);
			list->push_back(generate_condition(jb));
		}
	}
	return list;
}

Value *generate_value(struct json_object *cond)
{
	try{
		if (!cond) throw "generate_value: cond missing";
		switch (json_object_get_type(cond))
		{
		case json_type_double:
			return new ConstValue(json_object_get_double(cond));
			break;
		case json_type_int:
			return new ConstValue(static_cast<double>(json_object_get_int(cond)));
			break;
		case json_type_string: // for NULLARY functions like "NOW" and channel aliases
		{
			const char *str = json_object_get_string(cond);
			if (!strcmp("NOW", str)){
				return new NullAryFct("NOW", f_now);
			}
			// lookup alias from channels:

			print(LOG_ERROR, "generate_value: unknown fct/alias: '%s'", str);
			// return 0 throw "generate_value: unknown fct/alias";
		}
		break;
		case json_type_object: {
			// key is the operand/function
			struct json_object_iterator it, itEnd;
			it = json_object_iter_begin(cond);
			itEnd = json_object_iter_end(cond);
			if (json_object_iter_equal(&it, &itEnd)) throw "object without content!";
			const char *key = json_object_iter_peek_name(&it);

            if (!strcmp(key, "+")) {
                Value *lhs, *rhs;
                generate_values_lhs_rhs(json_object_iter_peek_value(&it), lhs, rhs);
                json_object_iter_next(&it);
                if (!json_object_iter_equal(&it, &itEnd)) throw "object contains more than one op/fct!";
                return new BinaryFct("+", std::plus<double>(), lhs, rhs );
            }
            if (!strcmp(key, "LASTTIME")) {
                // unary functions have just a "variable" (=alias) as parameter:
                struct json_object *param = json_object_iter_peek_value(&it);
                if (json_object_get_type(param) != json_type_string) throw "generate_value param not a string";
                json_object_iter_next(&it);
                if (!json_object_iter_equal(&it, &itEnd)) throw "object contains more than one op/fct!";
                return new UnaryFct("LASTTIME", f_lasttime, json_object_get_string(param));
            }
            if (!strcmp(key, "AVGVALUE")) {
                // binary functions have an array as parameter: here alias, cond for duration
                struct json_object *param = json_object_iter_peek_value(&it);
                if (json_object_get_type(param) != json_type_array) throw "generate_value param not an array";
                json_object_iter_next(&it);
                if (!json_object_iter_equal(&it, &itEnd)) throw "object contains more than one op/fct!";
                // parameter 1 alias (string)
                const char *p1 = json_object_get_string(json_object_array_get_idx(param, 0));
                json_object *jp2 = json_object_array_get_idx(param, 1);
                if (!jp2) throw "AVGVALUE: empty p2";
                double p2;
                switch (json_object_get_type(jp2)){
                case json_type_double: p2 = json_object_get_double(jp2); break;
                case json_type_int: p2 = (double) json_object_get_int(jp2); break;
                default:
                    throw "AVGVALUE: wrong p2 type"; break;
                }
                if (!p1) throw "parameter for AVGVALUE missing/wrong";
                return new BinaryChannelFct("AVGVALUE", f_avgvalue, p1, p2);
            }
            if (!strcmp(key, "MINVALUE")) {
                // binary functions have an array as parameter: here alias, cond for duration
                struct json_object *param = json_object_iter_peek_value(&it);
                if (json_object_get_type(param) != json_type_array) throw "generate_value param not an array";
                json_object_iter_next(&it);
                if (!json_object_iter_equal(&it, &itEnd)) throw "object contains more than one op/fct!";
                // parameter 1 alias (string)
                const char *p1 = json_object_get_string(json_object_array_get_idx(param, 0));
                json_object *jp2 = json_object_array_get_idx(param, 1);
                if (!jp2) throw "MINVALUE: empty p2";
                double p2;
                switch (json_object_get_type(jp2)){
                case json_type_double: p2 = json_object_get_double(jp2); break;
                case json_type_int: p2 = (double) json_object_get_int(jp2); break;
                default:
                    throw "MINVALUE: wrong p2 type"; break;
                }
                if (!p1) throw "parameter for MINVALUE missing/wrong";
                return new BinaryChannelFct("MINVALUE", f_minvalue, p1, p2);
            }

			print(LOG_ERROR, "generate_value: got unknown fct '%s'", key);
		}
		break;
		default:
			throw "generate_value: wrong type!";
			break;
		}
	} catch (const char *err) {
		print(LOG_ERROR, "generate_value: exc %s!", err);
		throw;
	}

	return 0;
}

void generate_values_lhs_rhs(struct json_object *cond, Value *&lhs, Value *&rhs)
{
    // here we assume a json_type_array
    if (!cond) throw "generate_values_lhs_rhs: empty cond!";
    if (json_object_get_type(cond)!=json_type_array) throw "generate_values_lhs_rhs not an array!";

	int nrconds;
	if ((nrconds=json_object_array_length(cond))==2) {
			struct json_object *jb = json_object_array_get_idx(cond, 0);
			lhs = generate_value(jb);
			jb = json_object_array_get_idx(cond, 1);
			rhs = generate_value(jb);
			if (!lhs || !rhs) throw "generate_values_lhs_rhs error creating lhs/rhs!";
	} else throw "generate_values_lhs_rhs !=2 parameter!";
}


Condition *generate_condition(struct json_object *cond)
{
    try{
    if (!cond) throw "generate_condition: cond missing";
    switch (json_object_get_type(cond))
    {
    case json_type_boolean:
        return new ConstCondition (json_object_get_boolean(cond));
        break;
    case json_type_object: {
        // key is the operand/function
        struct json_object_iterator it, itEnd;
        it = json_object_iter_begin(cond);
        itEnd = json_object_iter_end(cond);
        if (json_object_iter_equal(&it, &itEnd)) throw "object without content!";
        const char *key = json_object_iter_peek_name(&it);

        if (!strcmp(key, "AND")) {
            std::list<Condition *> *cond_list=generate_condlist(json_object_iter_peek_value(&it));
            json_object_iter_next(&it);
            if (!json_object_iter_equal(&it, &itEnd)) throw "object contains more than one op/fct!";
            return new LogicalCondition("AND", std::logical_and<bool>(), cond_list );
        }
        if (!strcmp(key, "OR")) {
            std::list<Condition *> *cond_list=generate_condlist(json_object_iter_peek_value(&it));
            json_object_iter_next(&it);
            if (!json_object_iter_equal(&it, &itEnd)) throw "object contains more than one op/fct!";
            return new LogicalCondition("OR", std::logical_or<bool>(), cond_list );
        }
        if (!strcmp(key, "NOT")) {
            std::list<Condition *> *cond_list=generate_condlist(json_object_iter_peek_value(&it));
            json_object_iter_next(&it);
            if (!json_object_iter_equal(&it, &itEnd)) throw "object contains more than one op/fct!";
            return new UnaryCondition("NOT", std::logical_not<bool>(), cond_list );
        }
        if (!strcmp(key, "GT")) {
            Value *lhs, *rhs;
            generate_values_lhs_rhs(json_object_iter_peek_value(&it), lhs, rhs);
            json_object_iter_next(&it);
            if (!json_object_iter_equal(&it, &itEnd)) throw "object contains more than one op/fct!";
            return new BinaryComparison(">", std::greater<double>(), lhs, rhs );
        }
        if (!strcmp(key, "LTE")) {
            Value *lhs, *rhs;
            generate_values_lhs_rhs(json_object_iter_peek_value(&it), lhs, rhs);
            json_object_iter_next(&it);
            if (!json_object_iter_equal(&it, &itEnd)) throw "object contains more than one op/fct!";
            return new BinaryComparison("<=", std::less_equal<double>(), lhs, rhs );
        }
        if (!strcmp(key, "EQ")) {
            Value *lhs, *rhs;
            generate_values_lhs_rhs(json_object_iter_peek_value(&it), lhs, rhs);
            json_object_iter_next(&it);
            if (!json_object_iter_equal(&it, &itEnd)) throw "object contains more than one op/fct!";
            return new BinaryComparison("==", std::equal_to<double>(), lhs, rhs );
        }


        print(LOG_ERROR, "got unknown op/fct '%s'", key);

    }
    break;
    default:
        throw "generate_condition: wrong type!";
        break;
    }
    } catch (const char *err) {
        print(LOG_ERROR, "generate_condition: exc %s!", err);
        throw;
    }

    return 0;
}

Rule::Rule(std::string &name, struct json_object *cond, struct json_object *action) :
    _last_state(false), _name(name), _cond(0), _action(0)
{
    _action = new Action(action);
    _cond = generate_condition(cond);
    if (!_cond) throw "Rule::Rule no cond!";
}

void Rule::check()
{
    bool cond=false;
    if (_cond) cond = _cond->evaluate();
    if (cond!=_last_state && _action) _action->fire(cond, this);
    _last_state = cond;
    print(LOG_VERBOSE, "Rule '%s' %s", _name.c_str(), cond ? "true" : "false" );
}

std::ostream &operator<<(std::ostream &os, Rule const &r)
{
    os << std::string("rule '") << r._name << std::string("': ") << *(r._cond) << std::string(" ?-> ") << *(r._action);
    return os;
}

