
#include <any>
#include <gtest/gtest.h>
#include <vector>

#include <baseTypes.hpp>

#include "opBuilderHelperFilter.hpp"

using namespace base;
namespace bld = builder::internals::builders;

TEST(opBuilderHelperIntLessThanEqual, Builds)
{
    auto tuple = std::make_tuple(
        std::string {"/field"}, std::string {"int_less_or_equal"}, std::vector<std::string> {"10"});

    ASSERT_NO_THROW(bld::opBuilderHelperIntLessThanEqual(tuple));
}

TEST(opBuilderHelperIntLessThanEqual, Exec_less_than_equal_false)
{
    auto tuple = std::make_tuple(std::string {"/field2check"},
                                 std::string {"int_less_or_equal"},
                                 std::vector<std::string> {"8"});

    auto event1 = std::make_shared<json::Json>(R"({"field2check": 10})");

    auto op =
        bld::opBuilderHelperIntLessThanEqual(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_FALSE(result.success());
}

TEST(opBuilderHelperIntLessThanEqual, Exec_less_than_true)
{
    auto tuple = std::make_tuple(std::string {"/field2check"},
                                 std::string {"int_less_or_equal"},
                                 std::vector<std::string> {"12"});

    auto event1 = std::make_shared<json::Json>(R"({"field2check": 10})");

    auto op =
        bld::opBuilderHelperIntLessThanEqual(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_TRUE(result.success());
}

TEST(opBuilderHelperIntLessThanEqual, Exec_less_than_equal_true)
{
    auto tuple = std::make_tuple(std::string {"/field2check"},
                                 std::string {"int_less_or_equal"},
                                 std::vector<std::string> {"10"});

    auto event1 = std::make_shared<json::Json>(R"({"field2check": 10})");

    auto op =
        bld::opBuilderHelperIntLessThanEqual(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_TRUE(result.success());
}

TEST(opBuilderHelperIntLessThanEqual, Exec_less_than_equal_ref_false)
{
    auto tuple = std::make_tuple(std::string {"/field2check"},
                                 std::string {"int_less_or_equal"},
                                 std::vector<std::string> {"$otherfield"});

    auto event1 = std::make_shared<json::Json>(R"({"field2check": 12,
                                                   "otherfield": 10})");

    auto op =
        bld::opBuilderHelperIntLessThanEqual(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_FALSE(result.success());
}

TEST(opBuilderHelperIntLessThanEqual, Exec_less_than_ref_true)
{
    auto tuple = std::make_tuple(std::string {"/field2check"},
                                 std::string {"int_less_or_equal"},
                                 std::vector<std::string> {"$otherfield"});

    auto event1 = std::make_shared<json::Json>(R"({"field2check": 10,
                                                   "otherfield": 12})");

    auto op =
        bld::opBuilderHelperIntLessThanEqual(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_TRUE(result.success());
}

TEST(opBuilderHelperIntLessThanEqual, Exec_less_than_equal_ref_true)
{
    auto tuple = std::make_tuple(std::string {"/field2check"},
                                 std::string {"int_less_or_equal"},
                                 std::vector<std::string> {"$otherfield"});

    auto event1 = std::make_shared<json::Json>(R"({"field2check": 10,
                                                   "otherfield": 10})");

    auto op =
        bld::opBuilderHelperIntLessThanEqual(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_TRUE(result.success());
}

TEST(opBuilderHelperIntLessThanEqual, Exec_less_than_equal_multilevel_false)
{
    auto tuple = std::make_tuple(std::string {"/parentObjt_1/field2check"},
                                 std::string {"int_less_or_equal"},
                                 std::vector<std::string> {"8"});

    auto event1 = std::make_shared<json::Json>(R"({
                    "parentObjt_2": {
                        "field2check": 10,
                        "ref_key": 10
                    },
                    "parentObjt_1": {
                        "field2check": 11,
                        "ref_key": 11
                    }
                    })");

    auto op =
        bld::opBuilderHelperIntLessThanEqual(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_FALSE(result.success());
}

TEST(opBuilderHelperIntLessThanEqual, Exec_less_than_multilevel_true)
{
    auto tuple = std::make_tuple(std::string {"/parentObjt_1/field2check"},
                                 std::string {"int_less_or_equal"},
                                 std::vector<std::string> {"12"});

    auto event1 = std::make_shared<json::Json>(R"({
                    "parentObjt_2": {
                        "field2check": 10,
                        "ref_key": 10
                    },
                    "parentObjt_1": {
                        "field2check": 11,
                        "ref_key": 11
                    }
                    })");

    auto op =
        bld::opBuilderHelperIntLessThanEqual(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_TRUE(result.success());
}

TEST(opBuilderHelperIntLessThanEqual, Exec_less_than_equal_multilevel_true)
{
    auto tuple = std::make_tuple(std::string {"/parentObjt_1/field2check"},
                                 std::string {"int_less_or_equal"},
                                 std::vector<std::string> {"11"});

    auto event1 = std::make_shared<json::Json>(R"({
                    "parentObjt_2": {
                        "field2check": 10,
                        "ref_key": 10
                    },
                    "parentObjt_1": {
                        "field2check": 11,
                        "ref_key": 11
                    }
                    })");

    auto op =
        bld::opBuilderHelperIntLessThanEqual(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_TRUE(result.success());
}

TEST(opBuilderHelperIntLessThanEqual, Exec_less_than_equal_multilevel_ref_false)
{
    auto tuple = std::make_tuple(std::string {"/parentObjt_1/field2check"},
                                 std::string {"int_less_or_equal"},
                                 std::vector<std::string> {"$parentObjt_2.field2check"});

    auto event1 = std::make_shared<json::Json>(R"({
                    "parentObjt_2": {
                        "field2check": 10,
                        "ref_key": 10
                    },
                    "parentObjt_1": {
                        "field2check": 11,
                        "ref_key": 11
                    }
                    })");

    auto op =
        bld::opBuilderHelperIntLessThanEqual(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_FALSE(result.success());
}

TEST(opBuilderHelperIntLessThanEqual, Exec_less_than_multilevel_ref_true)
{
    auto tuple = std::make_tuple(std::string {"/parentObjt_1/field2check"},
                                 std::string {"int_less_or_equal"},
                                 std::vector<std::string> {"$parentObjt_2.field2check"});

    auto event1 = std::make_shared<json::Json>(R"({
                    "parentObjt_2": {
                        "field2check": 12,
                        "ref_key": 10
                    },
                    "parentObjt_1": {
                        "field2check": 10,
                        "ref_key": 10
                    }
                    })");

    auto op =
        bld::opBuilderHelperIntLessThanEqual(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_TRUE(result.success());
}

TEST(opBuilderHelperIntLessThanEqual, Exec_less_than_equal_multilevel_ref_true)
{
    auto tuple = std::make_tuple(std::string {"/parentObjt_1/field2check"},
                                 std::string {"int_less_or_equal"},
                                 std::vector<std::string> {"$parentObjt_2.field2check"});

    auto event1 = std::make_shared<json::Json>(R"({
                    "parentObjt_2": {
                        "field2check": 10,
                        "ref_key": 10
                    },
                    "parentObjt_1": {
                        "field2check": 10,
                        "ref_key": 10
                    }
                    })");

    auto op =
        bld::opBuilderHelperIntLessThanEqual(tuple)->getPtr<Term<EngineOp>>()->getFn();

    result::Result<Event> result = op(event1);

    ASSERT_TRUE(result.success());
}
