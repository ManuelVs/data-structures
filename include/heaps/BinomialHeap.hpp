#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>

#include "../allocators/BlockAllocator.hpp"

/**
 * @brief Heap
 * Element priority can be defined using the Comparator <br>
 * Element allocations are made using the Allocator <br>
 * @see https://es.wikipedia.org/wiki/Heap_Binomial
 * @tparam T Elements' type this heap will store
 * @tparam Comparator Comparator between keys
 * @tparam Allocator Keys' allocator
 */
template <typename T, typename Comparator = std::greater<T>, class Allocator = std::allocator<T>>
class BinomialHeap {
protected:
	/**
	 * @brief Uses the first swap found by the ADL algorithm. <br>
	 * This should find the user-defined swap before the generic std::swap, that makes
	 * three moves
	 * @tparam U Type
	 * @param left Left object
	 * @param right Right object
	 */
	template<typename U>
	void p_adl_swap(U& left, U& right) {
		using std::swap;
		swap(left, right); // ADL swap
	}

	struct Node {
		T key;
		int degree;

		Node* father;
		Node* sibling;
		Node* child;
	};
	
	using NodeAllocator = std::allocator<Node>;

	using alloc_key_traits = std::allocator_traits<Allocator>;
	using alloc_node_traits = std::allocator_traits<NodeAllocator>;

	NodeAllocator alloc_node;

	Comparator comparator;
	Allocator alloc_key;

	Node* _root;
	Node* _greater;
	std::size_t _size;

	template<class... Args>
	Node* p_create(Args&&... args) {
		Node* node = alloc_node_traits::allocate(alloc_node, 1);

		alloc_key_traits::construct(alloc_key, &node->key, std::forward<Args>(args)...);
		node->degree = 0;
		node->father = nullptr;
		node->sibling = nullptr;
		node->child = nullptr;

		return node;
	}

	void p_delete(Node* root) {
		if (root != nullptr) {
			p_delete(root->child);
			p_delete(root->sibling);
			alloc_key_traits::destroy(alloc_key, &root->key);
			alloc_node_traits::deallocate(alloc_node, root, 1);
		}
	}

	Node* p_copy(Node* root) {
		if (root == nullptr) return nullptr;
		Node* new_node = p_create(root->key); //copy key
		new_node->child = p_copy(root->child);
		new_node->sibling = p_copy(root->sibling);
		return new_node;
	}

	void p_default() {
		this->_root = nullptr;
		this->_greater = nullptr;
		this->_size = 0;
	}

	void p_new(Comparator const& c = Comparator(), Allocator const& alloc = Allocator()) {
		this->p_default();
		this->comparator = c;
		this->alloc_key = alloc;
	}

	void p_delete() {
		this->p_delete(this->_root);
		this->p_default();
	}

	void p_copy(BinomialHeap const& other) {
		this->comparator = other.comparator;
		this->alloc_key = other.alloc_key;

		this->_root = p_copy(other._root);
		this->_size = other._size;
		this->p_update_greater();
	}

	void p_move(BinomialHeap& other) {
		this->comparator = std::move(other.comparator);
		this->alloc_key = std::move(other.alloc_key);
		this->alloc_node = std::move(other.alloc_node);

		this->_root = other._root;
		this->_size = other._size;
		this->_greater = other._greater;
		
		other.p_default();
	}

	void p_swap(BinomialHeap& other) {
		p_adl_swap(this->comparator, other.comparator);
		p_adl_swap(this->alloc_key, other.alloc_key);
		p_adl_swap(this->_root, other._root);
		p_adl_swap(this->_greater, other._greater);
		p_adl_swap(this->_size, other._size);
	}

	/**
	 * @brief Merge h1 and h2 lists. Returns the initial root.
	 * All intermediate pointers are reasigned, so the user
	 * must suppose that h1 and h2 stays in a 'bad' state.
	 * This *DOES NOT* link equal degree nodes.
	 * Time complexity: O(length(h1) + length(h2));
	 * @param h1 First heap
	 * @param h2 Second heap
	 * @return Node* Initial root of merge result
	 */
	Node* p_merge_list(Node* h1, Node* h2) {
		if (h1 != nullptr && h2 == nullptr) return h1;
		if (h1 == nullptr && h2 != nullptr) return h2;
		if (h1 == nullptr && h2 == nullptr) return nullptr;

		Node* orig;
		Node* iter;
		if (h1->degree <= h2->degree) {
			orig = iter = h1;
			h1 = h1->sibling;
		}
		else {
			orig = iter = h2;
			h2 = h2->sibling;
		}

		// Merge algorithm on 'degree'
		while (h1 != nullptr && h2 != nullptr) {
			if (h1->degree <= h2->degree) {
				iter->sibling = h1;
				h1 = h1->sibling;
				iter = iter->sibling;
			}
			else {
				iter->sibling = h2;
				h2 = h2->sibling;
				iter = iter->sibling;
			}
		}

		if (h1 != nullptr) {
			iter->sibling = h1;
		}
		else if (h2 != nullptr) {
			iter->sibling = h2;
		}

		return orig;
	}

	void p_link(Node* father, Node* child) {
		if (father->degree == 0) {
			father->child = child;
		}
		else {
			Node* origChild = father->child;

			while (origChild->sibling != nullptr) {
				origChild = origChild->sibling;
			} // father->degree iterations

			origChild->sibling = child;
		}
		++father->degree;
		child->sibling = nullptr;
		child->father = father;
	}

	void p_update_greater() {
		this->_greater = this->_root;
		Node* iter = this->_root;
		while (iter != nullptr) {
			if (comparator(iter->key, this->_greater->key)) {
				this->_greater = iter;
			}
			iter = iter->sibling;
		}
	}

	Node* p_union(Node* h1, Node* h2) {
		Node* head = p_merge_list(h1, h2);
		if (head == nullptr) return nullptr;

		Node* nAnt = nullptr;
		Node* nAct = head;
		Node* nSig = head->sibling;

		while (nSig != nullptr) {
			if (nAct->degree != nSig->degree || (nSig->sibling != nullptr && nSig->sibling->degree == nAct->degree)) {
				// Forward nAnt, nAct and nSig
				nAnt = nAct;
				nAct = nSig;
				nSig = nSig->sibling;
			}
			else {
				// nAct and nSig have the same degree AND
				//   OR nSig->sibling is null
				//   OR nAct and nSig and nSig->sibling have the same grade
				if (comparator(nAct->key, nSig->key)) {
					// nAct key is greater, nSig must be the child
					nAct->sibling = nSig->sibling;
					p_link(nAct, nSig);
					nSig = nAct->sibling; //recover nSig to the root list
				}
				else {
					// nAct key is less, nAct must be the child
					if (nAnt == nullptr) {
						// If the root has to go child we must recover the rootlist.
						head = nSig;
					}
					else nAnt->sibling = nSig;

					p_link(nSig, nAct);

					nAct = nSig;
					nSig = nSig->sibling;
				}
			}
		}

		return head;
	}

	template<class... Args>
	void p_emplace(Args&&... args) {
		Node* node = p_create(std::forward<Args>(args)...);

		this->_root = p_union(this->_root, node);
		this->p_update_greater();
		++this->_size;
	}

	void p_merge(BinomialHeap& other){
		Node* this_root = this->_root;
		Node* other_root = other._root;

		this->_root = p_union(this_root, other_root);
		this->p_update_greater();

		this->_size += other._size;
		other.p_default();
	}

	T p_pop() {
		Node* iter = this->_root;
		Node* ant = nullptr;

		while (iter != this->_greater) {
			ant = iter;
			iter = iter->sibling;
		}

		if (ant != nullptr) {
			ant->sibling = iter->sibling;
		}

		Node* firstChild = this->_greater->child;
		if (firstChild != nullptr) {
			while (firstChild->sibling != nullptr) {
				firstChild->father = nullptr;
				firstChild = firstChild->sibling;
			}
			firstChild = this->_greater->child;
		}
		if (this->_root == this->_greater) this->_root = this->_root->sibling;
		
		T key = std::move(this->_greater->key);
		alloc_node_traits::deallocate(alloc_node, this->_greater, 1);

		this->_root = p_union(this->_root, firstChild);
		this->p_update_greater();
		--this->_size;

		return key;
	}

public:

	/**
	 * @brief Construct a new Binomial Heap object
	 * Time complexity: O(1)
	 * @param c 
	 * @param alloc 
	 */
	BinomialHeap(Comparator const& c = Comparator(), Allocator const& alloc = Allocator()) {
		this->p_new(c, alloc);
	}

	/**
	 * @brief Construct a new Binomial Heap object by copy
	 * Time complexity: O(other.size())
	 * @param other The other heap to copy
	 */
	BinomialHeap(BinomialHeap const& other) {
		this->p_copy(other);
	}

	/**
	 * @brief Construct a new Binomial Heap object by move
	 * Time complexity: O(1)
	 * @param other The other heap to move
	 */
	BinomialHeap(BinomialHeap&& other) {
		this->p_move(other);
	}

	/**
	 * @brief Destroy the Binomial Heap object
	 * Time complexity: O(this->size())
	 *  - Because delete operator costs O(n)
	 */
	~BinomialHeap() {
		this->p_delete();
	}

	/**
	 * @brief Copy assignment operator
	 * Time complexity: O(other.size() + this->size())
	 *  - Because delete operator costs O(n)
	 * @param other The other heap to copy
	 * @return BinomialHeap& Reference to *this
	 */
	BinomialHeap& operator=(BinomialHeap const& other) {
		if (this != &other) {
			this->p_delete();
			this->p_copy(other);
		}
		return *this;
	}

	/**
	 * @brief Move assignment operator
	 * Time complexity: O(this->size())
	 *  - Because delete operator costs O(n)
	 * @param other The other heap to move
	 * @return BinomialHeap& Reference to *this
	 */
	BinomialHeap& operator=(BinomialHeap&& other) noexcept {
		if (this != &other) {
			this->p_delete();
			this->p_move(other);
		}
		return *this;
	}

	/**
	 * @brief Swap the two heaps
	 * Time complexity: O(1)
	 * @param other The other heap
	 */
	void swap(BinomialHeap& other) {
		this->p_swap(other);
	}

	/**
	 * @brief Push 'elem' to the heap, copying it
	 * Time complexity: O(log(this->size()) + copy)
	 * @param key element to push
	 */
	void push(T const& key) {
		this->p_emplace(key);
	}

	/**
	 * @brief Push 'elem' to the heap, moving it
	 * Time complexity: O(log(this->size()) + move)
	 * @param key element to push
	 */
	void push(T&& key) {
		this->p_emplace(std::move(key));
	}

	/**
	 * @brief Push 'elem' to the heap, creating it
	 * Time complexity: O(log(this->size()) + creation)
	 * @tparam Args List of types to the object constructor
	 * @param args Parameters to the object constructor
	 */
	template <class... Args>
	void emplace(Args&&... args) {
		this->p_emplace(std::forward<Args>(args)...);
	}

	/**
	 * @brief Pop element from the heap
	 * Time complexity: O(log(this->size))
	 */
	T pop() {
		if (empty()) throw std::domain_error("Empty heap");
		return this->p_pop();
	}

	/**
	 * @brief Merge two FibonacciHeaps. The other heap ys left on a default state.
	 * Time complexity: O(1)
	 * @param other 
	 */
	void merge(BinomialHeap& other) {
		this->p_merge(other);
	}

	/**
	 * @brief Consult top element of the heap
	 * Time complexity: O(1)
	 */
	T const& top() const {
		if (empty()) throw std::domain_error("Empty heap");
		return this->_greater->key;
	}

	/**
	 * @brief If heap empty
	 * Time complexity: O(1)
	 */
	bool empty() const {
		return _root == nullptr;
	}

	/**
	 * @brief Number of elements
	 * Time complexity: O(1)
	 */
	std::size_t size() const {
		return this->_size;
	}
};

/**
 * @brief Time complexity: O(1)
 * ADL finds this swap
 */
template<typename T, class C, class A>
void swap(BinomialHeap<T, C, A>& lhs, BinomialHeap<T, C, A>& rhs) {
	lhs.swap(rhs);
}