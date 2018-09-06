#include "gtest/gtest.h"
#include "HeapTest.hpp"

#include "heaps/BinomialHeap.hpp"

TEST(BinomialHeapTest, Construct) {
	construct_test<BinomialHeap<int>>();
}

TEST(BinomialHeapTest, ConstructCopy) {
	construct_copy_test<BinomialHeap<int>>();
}

TEST(BinomialHeapTest, ConstructMove) {
	construct_move_test<BinomialHeap<int>>();
}

TEST(BinomialHeapTest, Push1) {
	push1_test<BinomialHeap<int>>();
}

TEST(BinomialHeapTest, Push2) {
	push2_test<BinomialHeap<int>>();
}

TEST(BinomialHeapTest, Emplace1) {
	emplace1_reverse_test<BinomialHeap<int, std::less<int>>>();
}

TEST(BinomialHeapTest, BigPush1) {
	big_push1_test<BinomialHeap<int>>();
}

TEST(BinomialHeapTest, BigPush2) {
	big_push2_test<BinomialHeap<int>>();
}

TEST(BinomialHeapTest, NestedHeap) {
	BinomialHeap<int> h0;
	BinomialHeap<int> h1;
	h1.push(1);
	BinomialHeap<int> h2;
	h2.push(2);
	h2.push(3);

	BinomialHeap<BinomialHeap<int>, std::function<bool(const BinomialHeap<int>&, const BinomialHeap<int>&)>> heap(
		[](const BinomialHeap<int>& left, const BinomialHeap<int>& right) {
		return left.size() < right.size();
		}
	);

	heap.push(h0);
	heap.push(h1);
	heap.push(h2);

	EXPECT_EQ(heap.pop().size(), 0);
	EXPECT_EQ(heap.pop().size(), 1);
	EXPECT_EQ(heap.pop().size(), 2);
}

TEST(BinomialHeapTest, RaisesExceptionWhenEmpty) {
	raises_exception_when_empty_test<BinomialHeap<int>>();
}

TEST(BinomialHeapTest, Merge) {
	merge_test<BinomialHeap<int>>();
}

TEST(BinomialHeapTest, BigMerge) {
	big_merge_test<BinomialHeap<int>>();
}
