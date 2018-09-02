#include "gtest/gtest.h"

#include <queue>

TEST(STLHeapTest, Push1) {
	std::priority_queue<int> heap;
	heap.push(1);
	heap.push(8);
	heap.push(6);
	heap.push(5);
	heap.push(3);
	heap.push(7);
	heap.push(4);

	EXPECT_EQ(heap.top(), 8); heap.pop();
	EXPECT_EQ(heap.top(), 7); heap.pop();
	EXPECT_EQ(heap.top(), 6); heap.pop();
	EXPECT_EQ(heap.top(), 5); heap.pop();
	EXPECT_EQ(heap.top(), 4); heap.pop();
	EXPECT_EQ(heap.top(), 3); heap.pop();
	EXPECT_EQ(heap.top(), 1); heap.pop();
}

TEST(STLHeapTest, Push2) {
	std::priority_queue<int> heap;
	heap.push(10);
	heap.push(1);
	heap.push(8);
	heap.push(2);
	heap.push(7);
	heap.push(11);
	heap.push(10);

	EXPECT_EQ(heap.top(), 11); heap.pop();
	EXPECT_EQ(heap.top(), 10); heap.pop();
	EXPECT_EQ(heap.top(), 10); heap.pop();
	EXPECT_EQ(heap.top(), 8); heap.pop();
	EXPECT_EQ(heap.top(), 7); heap.pop();
	EXPECT_EQ(heap.top(), 2); heap.pop();
	EXPECT_EQ(heap.top(), 1); heap.pop();
}

TEST(STLHeapTest, Emplace1) {
	std::priority_queue<int, std::vector<int>, std::greater<int>> heap;
	heap.emplace(1);
	heap.emplace(2);
	heap.emplace(3);
	heap.emplace(4);
	heap.emplace(5);
	heap.emplace(6);
	heap.emplace(7);

	EXPECT_EQ(heap.top(), 1); heap.pop();
	EXPECT_EQ(heap.top(), 2); heap.pop();
	EXPECT_EQ(heap.top(), 3); heap.pop();
	EXPECT_EQ(heap.top(), 4); heap.pop();
	EXPECT_EQ(heap.top(), 5); heap.pop();
	EXPECT_EQ(heap.top(), 6); heap.pop();
	EXPECT_EQ(heap.top(), 7); heap.pop();
}

#include <random>
TEST(STLHeapTest, RandomTest1) {
	std::priority_queue<int> heap;
	std::default_random_engine random;
	size_t size = 1000000;

	for (std::size_t i = 0; i < size; ++i) {
		heap.push(i);
	}

	EXPECT_EQ(heap.size(), size);

	int actual = heap.top(); heap.pop();
	for (std::size_t i = 0; i < size - 1; ++i) {
		EXPECT_LE(heap.top(), actual);
		actual = heap.top(); heap.pop();
	}

	EXPECT_TRUE(heap.empty());
}

#include <functional>
TEST(STLHeapTest, NestedHeap) {
	std::priority_queue<int> h0;
	std::priority_queue<int> h1;
	h1.push(1);
	std::priority_queue<int> h2;
	h2.push(2);
	h2.push(3);

	std::priority_queue<
		std::priority_queue<int>,
		std::vector<std::priority_queue<int>>,
		std::function<bool(const std::priority_queue<int>&, const std::priority_queue<int>&)>>
		heap(
		[](const std::priority_queue<int>& left, const std::priority_queue<int>& right) {
			return left.size() > right.size();
		}
	);
	
	heap.push(h0);
	heap.push(h1);
	heap.push(h2);

	EXPECT_EQ(heap.top().size(), 0); heap.pop();
	EXPECT_EQ(heap.top().size(), 1); heap.pop();
	EXPECT_EQ(heap.top().size(), 2); heap.pop();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}