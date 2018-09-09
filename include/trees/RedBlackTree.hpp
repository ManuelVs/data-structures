#pragma once

template <typename Key>
class LeftLeaningRedBlackTree {
private:

	//*********** DEFINITIONS ***********//
	enum Color { RED, BLACK };

	struct Node {
		Key key;
		Color color;
		Node* left;
		Node* right;
	};

	//********* END DEFINITIONS *********//

	//************ STRUCTURE ************//

	Node* root;

	//********** END STRUCTURE **********//

	LeftLeaningRedBlackTree(Node* node) : root(node) {}

	//************* METHODS *************//

	void del(Node* node) {
		if (node != nullptr) {
			del(node->left);
			del(node->right);
			delete node;
		}
	}

	bool isRed(Node* node) {
		if (node == nullptr) return false;
		else return node->color == RED;
	}

	bool is4Node(Node* node) {
		if (node == nullptr) return false;
		else return isRed(node->left) && isRed(node->right);
	}

	void flipColor(Node* node) {
		node->color = node->color == RED ? BLACK : RED;
		node->left->color = node->left->color == RED ? BLACK : RED;
		node->right->color = node->right->color == RED ? BLACK : RED;
	}

	Node* rotateLeft(Node* h) {
		Node* x = h->right; //x sera el nuevo padre
		h->right = x->left; //h adopta el hijo izquierdo de x
		x->left = h; //h se hace el hijo izquierdo de x
		x->color = h->color; //x adopta el color que tenia h
		h->color = RED; //h es rojo

		return x;
	}

	Node* rotateRight(Node* h) {
		Node* x = h->left;
		h->left = x->right;
		x->right = h;
		x->color = h->color;
		h->color = RED;

		return x;
	}

	Node* fixUp(Node* node) {
		if (isRed(node->right)) {
			node = rotateLeft(node);
		}
		if (isRed(node->left) && isRed(node->left->left)) {
			// h->left could be nullptr, but isRed returns false so the second call does not dereference a nullptr (&&)
			node = rotateRight(node);
		}
		if (isRed(node->left) && isRed(node->right)) {
			flipColor(node);
		}

		return node;
	}

	Node* moveRedRight(Node* node) {
		flipColor(node);
		if (isRed(node->left->left)) {
			node = rotateRight(node);
			flipColor(node);
		}

		return node;
	}

	Node* moveRedLeft(Node* node) {
		flipColor(node);
		if (isRed(node->right->left)) {
			node->right = rotateRight(node->right);
			node = rotateLeft(node);
			flipColor(node);
		}

		return node;
	}

	Node* p_eraseMax(Node* node) {
		if (isRed(node->left))
			node = rotateRight(node);

		if (node->right == nullptr) {
			//revise...
			delete node;
			return nullptr;
		}

		if (!isRed(node->right) && !isRed(node->right->left)) // => node->right is a 2-node
			node = moveRedRight(node);

		node->right = p_eraseMax(node->right);

		return fixUp(node);
	}

	Node* p_eraseMin(Node* node) {
		if (node->left == nullptr) {
			//revise...
			delete node;
			return nullptr;
		}

		if (!isRed(node->left) && !isRed(node->left->left)) // => node->left is a 2-node
			node = moveRedRight(node);

		node->left = p_eraseMin(node->left);

		return fixUp(node);
	}

	Node* p_setNullMin(Node* node) {
		if (node->left == nullptr) {
			//revise...
			//delete node;
			return nullptr;
		}

		if (!isRed(node->left) && !isRed(node->left->left)) // => node->left is a 2-node
			node = moveRedRight(node);

		node->left = p_eraseMin(node->left);

		return fixUp(node);
	}

	Node* p_findMin(Node* node) {
		while (node->left != nullptr) {
			node = node->left;
		}

		return node;
	}

	Node* p_findMax(Node* node) {
		while (node->right != nullptr) {
			node = node->right;
		}

		return node;
	}

	Node* p_erase(Node* node, Key const& key) {
		if (node == nullptr) return nullptr;

		if (key < node->key) {
			if (!isRed(node->left) && node->left != nullptr && !isRed(node->left->left))
				node = moveRedLeft(node);

			node->left = p_erase(node->left, key);
		}
		else {
			if (isRed(node->left))
				node = rotateRight(node);

			if (key == node->key && node->right == nullptr) {
				delete node;
				return nullptr;
			}

			if (!isRed(node->right) && node->right != nullptr && !isRed(node->right->left))
				node = moveRedRight(node);

			if (key == node->key) {
				//copying key and value may be expensive
				Node* min = p_findMin(node->right);
				min->left = node->left;
				min->right = p_setNullMin(node->right);

				delete node;
				node = min;
			}
			else node->right = p_erase(node->right, key);
		}

		return node = fixUp(node);
	}

	bool p_contains(Node* node, Key const& key) {
		while (node != nullptr) {
			if (node->key > key) {
				node = node->left;
			}
			else if (node->key < key) {
				node = node->right;
			}
			else return true;
		}

		return false;
	}

	Node* p_insert(Node* node, Key const& key) {

		if (node == nullptr) return new Node{ key, RED, nullptr, nullptr };

		if (is4Node(node))
			flipColor(node); //split 4-nodes

		if (node->key == key) node->key = key; //do nothing
		else if (node->key > key) {
			node->left = p_insert(node->left, key); //insert to left
		}
		else {
			node->right = p_insert(node->right, key); //insert to right
		}

		if (isRed(node->right)) {
			node = rotateLeft(node); //there must not be right red links
		}

		if (isRed(node->left) && isRed(node->left->left)) {
			// There must not be two consecutive red links.
			// If there are, they are right - right because right-leaning red links were fixed before
			node = rotateRight(node);
		}

		return node;
	}

	//*********** END METHODS ***********//

public:

	LeftLeaningRedBlackTree() {
		root = nullptr;
	}

	~LeftLeaningRedBlackTree() {
		del(root);
	}


	bool contains(Key const& key) {
		return p_contains(root, key);
	}

	void insert(Key const& key) {
		root = p_insert(root, key);
	}

	void erase(Key const& key) {
		root = p_erase(root, key);
	}

	bool count(Key const& key) {
		return p_contains(root, key) ? 1 : 0;
	}
};