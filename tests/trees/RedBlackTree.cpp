#include "TreeTest.hpp"

#include "trees/RedBlackTree.hpp"

TEST(RedBlackTreeTest, Construct) {
	construct_test<LeftLeaningRedBlackTree<int>>();
}

TEST(RedBlackTreeTest, Insert1) {
    insert1_test<LeftLeaningRedBlackTree<int>>();
}

TEST(RedBlackTreeTest, Insert2) {
    insert2_test<LeftLeaningRedBlackTree<int>>();
}

TEST(RedBlackTreeTest, BigInsert1) {
    big_insert1_test<LeftLeaningRedBlackTree<int>>();
}
