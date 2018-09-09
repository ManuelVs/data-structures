#include "TreeTest.hpp"

#include <set>

TEST(STLSetTest, Construct) {
	construct_test<std::set<int>>();
}

TEST(STLSetTest, Insert1) {
    insert1_test<std::set<int>>();
}

TEST(STLSetTest, Insert2) {
    insert2_test<std::set<int>>();
}

TEST(STLSetTest, BigInsert1) {
    big_insert1_test<std::set<int>>();
}