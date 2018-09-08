#include "gtest/gtest.h"

template<class T>
void construct_test(){
    T tree;
}

template<class T>
void insert1_test(){
    T tree;
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);
    tree.insert(4);

	EXPECT_TRUE(tree.contains(1));
	EXPECT_TRUE(tree.contains(2));
	EXPECT_TRUE(tree.contains(3));
	EXPECT_TRUE(tree.contains(4));
}

