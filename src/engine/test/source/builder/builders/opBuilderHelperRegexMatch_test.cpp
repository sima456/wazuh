
#include <any>
#include <gtest/gtest.h>
#include <vector>

#include <baseTypes.hpp>

#include "opBuilderHelperFilter.hpp"

using namespace base;
namespace bld = builder::internals::builders;

TEST(opBuilderHelperRegexMatch, Builds)
{
    auto tuple = std::make_tuple(std::string {"/field"},
                                 std::string {"regex_match"},
                                 std::vector<std::string> {"^regex_test 123$"});

    ASSERT_NO_THROW(bld::opBuilderHelperRegexMatch(tuple));
}

TEST(opBuilderHelperRegexMatch, Exec_match_false)
{
    auto tuple = std::make_tuple(std::string {"/field2check"},
                                 std::string {"regex_match"},
                                 std::vector<std::string> {"regex_test$"});

    auto event1 = std::make_shared<json::Json>(R"({"field2check": "regex_test 123"})");

    auto op = bld::opBuilderHelperRegexMatch(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_FALSE(result.success());
}

TEST(opBuilderHelperRegexMatch, Exec_match_true)
{
    auto tuple = std::make_tuple(std::string {"/field2check"},
                                 std::string {"regex_match"},
                                 std::vector<std::string> {"^regex_test"});

    auto event1 = std::make_shared<json::Json>(R"({"field2check": "regex_test 123"})");

    auto op = bld::opBuilderHelperRegexMatch(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_TRUE(result.success());
}

TEST(opBuilderHelperRegexMatch, Exec_match_multilevel_false)
{
    auto tuple = std::make_tuple(std::string {"/parentObjt_1/field2check"},
                                 std::string {"regex_match"},
                                 std::vector<std::string> {"regex_test$"});

    auto event1 = std::make_shared<json::Json>(R"({
                    "parentObjt_2": {
                        "field2check": 10,
                        "ref_key": 10
                    },
                    "parentObjt_1": {
                        "field2check": "regex_test 123",
                        "ref_key": 11
                    }
                    })");

    auto op = bld::opBuilderHelperRegexMatch(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_FALSE(result.success());
}

TEST(opBuilderHelperRegexMatch, Exec_match_multilevel_true)
{
    auto tuple = std::make_tuple(std::string {"/parentObjt_1/field2check"},
                                 std::string {"regex_match"},
                                 std::vector<std::string> {"^regex_test"});

    auto event1 = std::make_shared<json::Json>(R"({
                    "parentObjt_2": {
                        "field2check": 10,
                        "ref_key": 10
                    },
                    "parentObjt_1": {
                        "field2check": "regex_test 123",
                        "ref_key": 11
                    }
                    })");

    auto op = bld::opBuilderHelperRegexMatch(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_TRUE(result.success());
}
