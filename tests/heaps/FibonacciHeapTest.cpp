#include "gtest/gtest.h"
#include "HeapTest.hpp"

#include "heaps/FibonacciHeap.hpp"

TEST(FibonacciHeapTest, Construct) {
	construct_test<FibonacciHeap<int>>();
}

TEST(FibonacciHeapTest, ConstructCopy) {
	construct_copy_test<FibonacciHeap<int>>();
}

TEST(FibonacciHeapTest, ConstructMove) {
	construct_move_test<FibonacciHeap<int>>();
}

TEST(FibonacciHeapTest, Push1) {
	push1_test<FibonacciHeap<int>>();
}

TEST(FibonacciHeapTest, Push2) {
	push2_test<FibonacciHeap<int>>();
}

TEST(FibonacciHeapTest, Emplace1) {
	emplace1_reverse_test<FibonacciHeap<int, std::less<int>>>();
}

TEST(FibonacciHeapTest, BigPush1) {
	big_push1_test<FibonacciHeap<int>>();
}

TEST(FibonacciHeapTest, BigPush2) {
	big_push2_test<FibonacciHeap<int>>();
}

TEST(FibonacciHeapTest, NestedHeap) {
	FibonacciHeap<int> h0;
	FibonacciHeap<int> h1;
	h1.push(1);
	FibonacciHeap<int> h2;
	h2.push(2);
	h2.push(3);

	FibonacciHeap<FibonacciHeap<int>, std::function<bool(const FibonacciHeap<int>&, const FibonacciHeap<int>&)>> heap(
		[](const FibonacciHeap<int>& left, const FibonacciHeap<int>& right) {
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

TEST(FibonacciHeapTest, RaisesExceptionWhenEmpty) {
	raises_exception_when_empty_test<FibonacciHeap<int>>();
}

TEST(FibonacciHeapTest, MergeTest) {
	merge_test<FibonacciHeap<int>>();
}

TEST(BinomialHeapTest, BigMerge) {
	big_merge_test<FibonacciHeap<int>>();
}
