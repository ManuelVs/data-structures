#pragma once

template <typename Key>
class AVLTree {
protected:
	struct Node {
		Key e;
		Node* left;
		Node* right;

		int cardinal;
		int height;
	};

	Node* root;

	void p_del(Node* node) {
		if (node != nullptr) {
			p_del(node->left);
			p_del(node->right);
			delete node;
		}
	}

	bool p_empty(Node* node) const {
		return node == nullptr;
	}

	int p_height(Node* node) {
		if (p_empty(node)) return 0;
		else return node->height;
	}

	int p_cardinal(Node* node) {
		if (p_empty(node)) return 0;
		else return node->cardinal;
	}

	Node* p_create_recycle(Node* left, Key const& elem, Node* right, Node* recycle) {
		recycle->left = left;
		recycle->e = elem;
		recycle->right = right;
		recycle->height = std::max(p_height(left), p_height(right)) + 1;
		recycle->cardinal = 1 + p_cardinal(left) + p_cardinal(right);

		return recycle;
	}

	Node* p_create(Node* left, Key const& elem, Node* right) {
		Node* node = new Node;
		return p_create_recycle(left, elem, right, node);
	}

	Node* p_equilLeft(Node* l, Key const& y, Node* r, Node* recycle) {
		Node* ll = l->left;
		Node* lr = l->right;
		Key x = l->e;
		if (p_height(ll) >= p_height(lr)) {
			//Rotacion LL
			Node* newlr = p_create_recycle(lr, y, r, l);
			return p_create_recycle(ll, x, newlr, recycle);
		}
		else {
			//Rotacion LR
			Node* lrl = lr->left;
			Node* lrr = lr->right;
			Key z = l->right->e;

			Node* newll = p_create_recycle(ll, x, lrl, lr);
			Node* newlr = p_create_recycle(lrr, y, r, l);
			return p_create_recycle(newll, z, newlr, recycle);
		}
	}

	Node* p_equilRight(Node* l, Key const& y, Node* r, Node* recycle) {
		Node* rl = r->left;
		Node* rr = r->right;
		Key x = r->e;

		if (p_height(rr) >= p_height(rl)) {
			//Rotacion RR
			Node* newrl = p_create_recycle(l, y, rl, recycle);
			return p_create_recycle(newrl, x, rr, r);
		}
		else {
			Node* rll = rl->left;
			Node* rlr = rl->right;
			Key z = rl->e;

			Node* newrr = p_create_recycle(rlr, x, rr, rl);
			Node* newrl = p_create_recycle(l, y, rll, recycle);
			return p_create_recycle(newrl, z, newrr, r);
		}
	}

	Node* p_equil(Node* l, Key const& key, Node* r, Node* recycle) {
		if (p_height(l) == p_height(r) + 2) {
			return p_equilLeft(l, key, r, recycle);
		}
		else if (p_height(l) + 2 == p_height(r)) {
			return p_equilRight(l, key, r, recycle);
		}
		else {
			return p_create_recycle(l, key, r, recycle);
		}
	}

	void p_eraseMin(Node* node, Key& elem, Node*& t) {
		if (p_empty(node->left)) {
			elem = node->e;
			t = node->right;
			delete node;
		}
		else {
			Node* result_left;
			p_eraseMin(node->left, elem, result_left);
			
			t = p_equil(result_left, node->e, node->right, node);
		}
	}

	Node* p_erase(Node* node, Key const& elem) {
		if (p_empty(node)) return node;
		
		if (elem < node->e) {
			Node* newl = p_erase(node->left, elem);
			return p_equil(newl, node->e, node->right, node);
		}
		else if (elem > node->e) {
			Node* newr = p_erase(node->right, elem);
			return p_equil(node->left, node->e, newr, node);
		}
		else {
			//Soy yo
			if (p_empty(node->right)) {
				Node* aux = node->left;
				delete node;
				return aux;
			}
			else {
				Key& min = node->e;
				Node* result_right;
				p_eraseMin(node->right, min, result_right);

				return p_equil(node->left, min, result_right, node);
			}
		}
	}

	Node* p_insert(Node* node, Key const& elem) {
		if (p_empty(node)) return p_create(nullptr, elem, nullptr);

		if (elem < node->e) {
			Node* aux = p_insert(node->left, elem);
			return p_equil(aux, node->e, node->right, node);
		}
		else if (elem == node->e) return node;
		else {
			Node* aux = p_insert(node->right, elem);
			return p_equil(node->left, node->e, aux, node);
		}
	}

	bool p_contains(Node* node, Key const& key) const {
		while (!p_empty(node)) {
			if (key < node->e) {
				node = node->left;
			}
			else if (key > node->e) {
				node = node->right;
			}
			else return true;
		}

		return false;
	}

public:

	AVLTree() {
		root = nullptr;
	}

	~AVLTree() {
		p_del(root);
	}

	void insert(Key const& key) {
		root = p_insert(root, key);
	}

	void erase(Key const& key) {
		root = p_erase(root, key);
	}

	bool contains(Key const& key) const {
		return p_contains(root, key);
	}
};