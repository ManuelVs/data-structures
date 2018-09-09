#include "TreeTest.hpp"

#include "trees/AVLTree.hpp"

TEST(AVLTreeTest, Construct) {
	construct_test<AVLTree<int>>();
}

TEST(AVLTreeTest, Insert1) {
    insert1_test<AVLTree<int>>();
}

TEST(AVLTreeTest, Insert2) {
    insert2_test<AVLTree<int>>();
}

TEST(AVLTreeTest, BigInsert1) {
    big_insert1_test<AVLTree<int>>();
}
