#include "TreeTest.hpp"

#include "trees/AVLTree.hpp"

TEST(AVLTreeTest, Construct) {
	construct_test<AVLTree<int>>();
}

TEST(AVLTreeTest, Insert1) {
    insert1_test<AVLTree<int>>();
}
