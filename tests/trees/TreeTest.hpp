#include "gtest/gtest.h"

template<class T>
void construct_test(){
    T tree;
}

template<class T>
void construct_copy_test(){
    T tree1;
    tree1.insert(1);
    tree1.insert(2);
    tree1.insert(3);
    tree1.insert(4);
    tree1.erase();

    T tree2(tree1);

    EXPECT_EQ(tree1.size(), tree2.size());
}

template<class T>
void construct_move_test(){
    T tree1;
    tree1.insert(1);
    tree1.insert(2);
    tree1.insert(3);
    tree1.insert(4);
    tree1.erase();

    T tree2(std::move(tree1));

    EXPECT_EQ(tree1.size(), 0);
    EXPECT_EQ(tree2.size(), 3);
}

template<class T>
void insert1_test(){
    T tree;
	tree.insert(1);
	tree.insert(8);
	tree.insert(6);
	tree.insert(5);
	tree.insert(3);
	tree.insert(7);
	tree.insert(4);

	EXPECT_TRUE(tree.count(8) == 1);
    tree.erase(8);
    EXPECT_FALSE(tree.count(8) == 1);

	EXPECT_TRUE(tree.count(7) == 1);
    tree.erase(7);
    EXPECT_FALSE(tree.count(7) == 1);

	EXPECT_TRUE(tree.count(6) == 1);
    tree.erase(6);
    EXPECT_FALSE(tree.count(6) == 1);

	EXPECT_TRUE(tree.count(5) == 1);
    tree.erase(5);
    EXPECT_FALSE(tree.count(5) == 1);

	EXPECT_TRUE(tree.count(4) == 1);
    tree.erase(4);
    EXPECT_FALSE(tree.count(4) == 1);

	EXPECT_TRUE(tree.count(3) == 1);
    tree.erase(3);
    EXPECT_FALSE(tree.count(3) == 1);

	EXPECT_TRUE(tree.count(1) == 1);
    tree.erase(1);
    EXPECT_FALSE(tree.count(1) == 1);
}

template<class T>
void insert2_test(){
    T tree;
	tree.insert(10);
	tree.insert(1);
	tree.insert(8);
	tree.insert(2);
	tree.insert(7);
	tree.insert(11);
	tree.insert(10);

	EXPECT_TRUE(tree.count(11) == 1);
    tree.erase(11);
    EXPECT_FALSE(tree.count(11) == 1);

	EXPECT_TRUE(tree.count(10) == 1);
    tree.erase(10);
    EXPECT_FALSE(tree.count(10) == 1);

	EXPECT_TRUE(tree.count(8) == 1);
    tree.erase(8);
    EXPECT_FALSE(tree.count(8) == 1);

	EXPECT_TRUE(tree.count(7) == 1);
    tree.erase(7);
    EXPECT_FALSE(tree.count(7) == 1);

	EXPECT_TRUE(tree.count(2) == 1);
    tree.erase(2);
    EXPECT_FALSE(tree.count(2) == 1);

	EXPECT_TRUE(tree.count(1) == 1);
    tree.erase(1);
    EXPECT_FALSE(tree.count(1) == 1);
}

template<class T>
void emplace1_reverse_test(){
    T tree;
    tree.emplace(1);
    tree.emplace(6);
    tree.emplace(3);
	tree.emplace(2);
    tree.emplace(7);
	tree.emplace(4);
	tree.emplace(5);

	EXPECT_TRUE(tree.count(1) == 1);
    tree.erase(1);
    EXPECT_FALSE(tree.count(1) == 1);

	EXPECT_TRUE(tree.count(6) == 1);
    tree.erase(6);
    EXPECT_FALSE(tree.count(6) == 1);

	EXPECT_TRUE(tree.count(3) == 1);
    tree.erase(3);
    EXPECT_FALSE(tree.count(3) == 1);

	EXPECT_TRUE(tree.count(2) == 1);
    tree.erase(2);
    EXPECT_FALSE(tree.count(2) == 1);

	EXPECT_TRUE(tree.count(7) == 1);
    tree.erase(7);
    EXPECT_FALSE(tree.count(7) == 1);

	EXPECT_TRUE(tree.count(4) == 1);
    tree.erase(4);
    EXPECT_FALSE(tree.count(4) == 1);

    EXPECT_TRUE(tree.count(5) == 1);
    tree.erase(4);
    EXPECT_FALSE(tree.count(5) == 1);
}

#include <random>
#include <vector>
template<class T>
void big_insert1_test(){
    T tree;
	std::default_random_engine random(1227992885);
	size_t size = 1000000;
    std::vector<int> v;
    v.reserve(size);

	for (std::size_t i = 0; i < size; ++i) {
        int elem = random();
		tree.insert(elem);
        v.push_back(elem);
	}

	for (std::size_t i = 0; i < size - 1; ++i) {
        int elem = v[i];
        tree.erase(elem);
	}
}
