#pragma once

#include <unordered_map>
#include <stdexcept>
#include <ostream>
#include <string>
#include <functional>

// FibonacciHeap
// Una implementación de los montículos de Fibonacci
// Manuel Velasco Suárez
// Estructura de datos necesaria para la práctica de MARP
// T: Tipo de la clave
template <typename T, class Comparator = std::greater<T>, class Allocator = std::allocator<T>>
class FibonacciHeap
{
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

	/**
	 * @brief This struct contains all what is needed to represent a node in this
	 * FibonacciHeap
	 */
	struct Node {
		T key;
		bool mark;
		std::size_t degree;

		Node* father;
		Node* siblingLeft;
		Node* siblingRight;
		Node* child;
	};
	
	using NodeAllocator = std::allocator<Node>;

	using alloc_key_traits = std::allocator_traits<Allocator>;
	using alloc_node_traits = std::allocator_traits<NodeAllocator>;

	NodeAllocator alloc_node;

	Comparator comparator;
	Allocator alloc_key;

	Node* min;
	std::size_t _size;

	/**
	 * @brief Create an empy node constructing the element on it. <br>
	 * The node is linked to itself, this makes easier future operations. <br>
	 * Time complexity: O(object_creation)
	 * @tparam Args List of type params of the object constructor
	 * @param args List of params of the object Constructor
	 * @return Node* Created node
	 */
	template<class... Args>
	Node* p_create(Args&&... args) {
		Node* node = alloc_node_traits::allocate(alloc_node, 1);
		alloc_key_traits::construct(alloc_key, &node->key, std::forward<Args>(args)...);

		node->mark = false;
		node->degree = 0; //No tiene hijos
		node->father = node->child = nullptr; //No tiene padre ni hijos
		node->siblingLeft = node->siblingRight = node; //Se enlaza a si mismo

		return node;
	}

	/**
	 * @brief Deallocates a node, its childs and its siblings.
	 * @param node Node to deallocate
	 */
	void p_delete(Node* node){
		if(node != nullptr){
			Node* siblingLeft = node->siblingLeft;
			siblingLeft->siblingRight = nullptr;

			p_delete(node->child);
			p_delete(node->siblingRight);

			alloc_key_traits::destroy(alloc_key, &node->key);
			alloc_node_traits::deallocate(alloc_node, node, 1);
		}
	}

	/**
	 * @brief Duplicates the node and its childs, NOT ITS SIBLINGS <br>
	 * @param node Node to duplicate
	 * @param father Its new father (it shouldnt be here...)
	 * @return Node* New node
	 */
	Node* p_copy(Node* node, Node* father) {
		if (node == nullptr) return nullptr;

		Node* new_node = p_create(node->key); //copy key
		new_node->mark = node->mark;
		new_node->degree = node->degree;
		new_node->father = father;

		if(new_node->child == nullptr) new_node->child = nullptr; 
		else { // Si tiene hijos
			Node* firstChild = node->child; // Esto servira para saber cuando acabar el bucle
			new_node->child = p_copy(firstChild, new_node); //Se duplica el primero y servira como "raiz"

			Node* it = firstChild->siblingLeft; //Se visita el hermano izquierdo
			while (it != firstChild) { //Si no hemos llegado al primero
				Node* dupl = p_copy(it, new_node); //entonces se duplica
				p_insertToLeft(dupl, new_node->child); //se inserta a la izquierda de la "raiz"

				it = it->siblingLeft; // y se visita de nuevo al hermano izquierdo
			}
		}

		// No se conserva el orden inicial, pero no es algo que nos importe
		return new_node;
	}

	void p_default() {
		this->min = nullptr;
		this->_size = 0;
	}

	void p_new(Comparator const& c = Comparator(), Allocator const& alloc = Allocator()) {
		this->p_default();
		this->comparator = c;
		this->alloc_key = alloc;
	}

	void p_delete() {
		this->p_delete(this->min);
	}

	void p_copy(FibonacciHeap const& other) noexcept {
		this->comparator = other.comparator;
		this->alloc_key = other.alloc_key;

		if (other.min == nullptr) {
			this->min = nullptr;
			this->_size = 0;
		}
		else {
			this->min = p_copy(other.min, nullptr);

			Node* it = other.min->siblingLeft;
			while (it != other.min) {
				Node* dupl = p_copy(it, nullptr);
				p_insertToLeft(dupl, this->min);

				it = it->siblingLeft;
			}

			this->_size = other._size;
		}
	}

	void p_move(FibonacciHeap& other) noexcept {
		this->comparator = std::move(other.comparator);
		this->alloc_key = std::move(other.alloc_key);
		this->alloc_node = std::move(other.alloc_node);

		this->min = other.min;
		this->_size = other._size;

		other.p_default();
	}

	/**
	 * @brief Insert the node to the left of origin
	 * @param toInsert Node to insert
	 * @param origin Its new sibling
	 */
	void p_insertToLeft(Node* toInsert, Node* origin) {
		Node* aux = origin->siblingLeft; // Se guarda un puntero del hermano izquierdo

		origin->siblingLeft = toInsert; //Se enlaza el nuevo nodo a la izquierda
		toInsert->siblingRight = origin; //A la derecha del nuevo nodo esta la raiz

		toInsert->siblingLeft = aux; //A la izquierda del nuevo nodo está el antiguo
		aux->siblingRight = toInsert; //A la derecha debe estar el nuevo nodo
	}

	/**
	 * @brief Unlink a node from the list belongs <br>
	 * The node is linked to itself
	 * @param node The node to unlink
	 */
	void p_delFromList(Node* node) {
		// Elimina el nodo de la lista, enlazando entre sí a sus hermanos
		Node* left = node->siblingLeft; //Se guarda el izquierdo
		Node* right = node->siblingRight; //Se guarda el derecho

		// Se entrelazan entre si
		left->siblingRight = right;
		right->siblingLeft = left;

		//No tiene padre
		node->father = nullptr;
		node->siblingLeft = node->siblingRight = node; //Se enlaza a si mismo, esto es util
	}

	/**
	 * @brief Converts 'y' to child of 'x'
	 * @param x New father
	 * @param y New child
	 */
	void p_link(Node* x, Node* y){
		// Pone a y como hijo de x
		y->father = x;
		y->mark = false;

		//Se aumenta el numero de hijos
		++x->degree;
		
		if (x->child == nullptr) {
			// Si no tiene hijos, se convierte en el único
			x->child = y;
			y->siblingLeft = y->siblingRight = y;
		}
		else {
			// Lo inserta como su hijo, por ejemplo, a la izquierda del que se estaba apuntando
			p_insertToLeft(y, x->child);
		}
	}

	/**
	 * @brief Consolidate the heap
	 * Time complexity: O(this->size)
	 * Amortized time complexity: O(log(this->size))
	 */
	void p_consolidate() {
		// No se sabe de qué tamaño debe ser el array, así que opte por un hashmap
		std::unordered_map<int, Node*> a;

		//Rompo la lista enlazada circular, asi es mas facil
		Node* next = this->min;
		next->siblingLeft->siblingRight = nullptr;
		next->siblingLeft = nullptr;
		while (next != nullptr) { //Mientras que no hayamos llegado al final de la lista enlazada
			Node* x = next;
			//Avanzamos, se hace aquí porque se puede dar el caso en el que next se deba bajar como hijo de algun nodo
			next = next->siblingRight;

			int d = x->degree; // El grado de x
			x->siblingLeft = x->siblingRight = x; // Se elimina de la lista enlazada
			while (a[d] != nullptr) { //Mientras sigan existiendo nodos con el mismo grado
				// Uno de los dos se debe colgar del otro, por convenio sera siempre x
				Node* y = a[d]; // El nodo que tiene el mismo grado

				if (comparator(y->key, x->key)) {
					// x siempre tiene la clave pequeña
					p_adl_swap(x, y);
				}

				//Se enlaza y de x
				p_link(x, y);

				// Ya no existen nodos de grado d
				a.erase(d);

				// El nodo actual tiene grado d + 1, ya que ha ganado un hijo
				++d;
			}
			// El nodo x que sale del bucle (o sin haber entrado) tiene grado d
			a[d] = x;
		}

		// Hay que encontrar el nuevo mínimo
		auto it = a.begin();
		this->min = it->second;
		++it;
		while (it != a.end()) {
			// Se recorre el "array"
			p_insertToLeft(it->second, this->min); //Se inserta este nodo a la izquierda del minimo
			if (comparator(it->second->key, this->min->key)) {
				this->min = it->second; //Si se encuentra un nuevo minimo, se actualiza
			}
			++it;
		}
	}

	/**
	 * @brief Unlinks x from y
	 * @param x Child to unlink
	 * @param y Father
	 */
	void p_cut(Node* x, Node* y) {
		//Corta x, hijo de y
		--y->degree;

		/**** En las transparencias no pone esto ****/
		// Si el hijo que se elimina (x) es al que apuntaba 'y',
		// hay que actualizar el hijo de y, ya que si no se "pierden"
		// nodos, y peor aun, ¡¡'y' seria el padre de la lista raiz!!
		if (y->child == x) {
			if (y->degree == 0) {
				// Otro caso especial, si y se queda sin hijos hay
				// que poner un nullptr.
				y->child = nullptr;
			}
			else {
				// Se apunta a su izquierdo (por decir algo), que sabemos que no es el mismo,
				// ya que y->degree > 0 => y->degreeORIGINAL > 1 => habia por lo menos 2 hijos
				y->child = x->siblingLeft;
			}
		}
		/**** FIN En las transparencias no pone esto ****/

		p_delFromList(x); //Se elimina como hijo
		p_insertToLeft(x, this->min); //Se inserta al lado del minimo, en la rootList
		x->mark = false;
	}

	/**
	 * @brief Recursive cut based on node->mark. <br>
	 * If 'y' is marked its needed to do a cascade cut, cutting his father
	 * @param y Node to cut
	 */
	void p_cascadingCut(Node* y) {
		if (y->mark) {
			// Estaba marcado, hay que cortarlo
			Node* z = y->father;
			p_cut(y, z);
			p_cascadingCut(z);
		}
		else if(y->father != nullptr){
			// No estaba marcado, se marca, excepto si pertenece a la lista raiz
			y->mark = true;
		}
	}

	/**
	 * @brief Inserts a new element on the heap, constructing it. <br>
	 * Returns the node associated to the new element. Its a valid node that can be used
	 * to call 'decreaseKey'. <br>
	 * Time complexity: O(1)
	 * @tparam Args List of type params of the object constructor
	 * @param args List of params of the object Constructor
	 * @return Node* Created node
	 */
	template<class... Args>
	Node* p_emplace(Args&&... args) {
		// Se crea un nodo con la clave
		Node* newNode = p_create(std::forward<Args>(args)...);

		if (this->min == nullptr) {
			// Si el monticulo estaba vacio, es el nuevo minimo
			this->min = newNode;
			_size = 1;
		}
		else {
			// Si no estaba vacio, se inserta a la izquierda
			p_insertToLeft(newNode, this->min);

			// Se actualiza el minimo si es necesario
			if (comparator(newNode->key, this->min->key)) {
				this->min = newNode;
			}
			++_size;
		}

		return newNode;
	}

	/**
	 * @brief Merge two heaps. The other heap is left on a default state.
	 * @param other The other heap to merge.
	 */
	void p_union(FibonacciHeap& other) {
		if (other.min == nullptr) return;

		/************ Concatenate ************/
		this->_size += other._size;

		// Rompo la lista
		Node* otherFirst = other.min;
		Node* otherLast = otherFirst->siblingRight;
		otherLast->siblingLeft = nullptr;
		otherFirst->siblingRight = nullptr;

		// Las uno, poniendo la lista del otro entre medias de this->min y this->min->left
		Node* preMin = this->min->siblingLeft;
		this->min->siblingLeft = otherFirst;
		otherFirst->siblingRight = this->min;

		preMin->siblingRight = otherLast;
		otherLast->siblingLeft = preMin;
		/********** End Concatenate **********/

		// other->min todavia es valido
		if (comparator(other.min->key, this->min->key)) {
			this->min = other.min;
		}

		// Hay que dejar en buen estado el otro
		other.min = nullptr;
		other._size = 0;
	}

	/**
	 * @brief Decrements a key.
	 * Time complexity: O(log(this->size))
	 * Amortized time complexity: O(1)
	 * @param x Node to decrement key
	 * @param newKey New key
	 */
	void p_decreaseKey(Node* x, T const& newKey) {
		if (x->key > newKey) {
			throw std::invalid_argument("New key is greater than current key");
		}

		x->key = newKey;
		Node* y = x->father;

		if (y != nullptr && x->key > y->key) {
			// No está en buena relación
			p_cut(x, y); //Se corta
			p_cascadingCut(y); //Se mira si hay que cortar su padre
		}

		if (comparator(x->key, this->min->key)) {
			// Se actualiza si es necesario
			this->min = x;
		}
	}

	/**
	 * @brief Deletes the greatest element on the heap
	 * Time complexity: O(this->size)
	 * Amortized time complexity: O(log(this->size))
	 * @return T element
	 */
	T p_pop() {
		Node* z = this->min;
		if (z != nullptr) {
			std::size_t i = 0;
			Node* it = z->child;
			while (i < z->degree) {
				// Desgraciadamente, hay que hacer este bucle, ya que hay que:
				// - Poner los "father" a null
				// - Desmarcarlo
				Node* aux = it;
				it = it->siblingLeft;

				p_insertToLeft(aux, this->min);
				aux->father = nullptr;
				aux->mark = false;

				++i;
			}

			--this->_size;
			if (z == z->siblingLeft) {
				// Si era el ultimo que quedaba
				this->min = nullptr;
			}
			else {
				this->min = z->siblingRight; // Temporal, hasta que consolidate haga su trabajo
				p_delFromList(z);
				p_consolidate();
			}
		}

		T key = std::move(z->key);
		alloc_node_traits::deallocate(alloc_node, z, 1); // Se elimina
		return key; //Se devuelve la clave
	}

public:
	/**
	 * @brief Construct a new Fibonacci Heap object
	 * @param c Comparator to use
	 * @param alloc Allocator to use
	 */
	FibonacciHeap(Comparator const& c = Comparator(), Allocator const& alloc = Allocator()) {
		this->p_new(c, alloc);
	}
	
	/**
	 * @brief Construct a new Fibonacci Heap object by copy
	 * @param other The other heap
	 */
	FibonacciHeap(FibonacciHeap const& other) {
		this->p_copy(other);
	}

	/**
	 * @brief Construct a new Fibonacci Heap object by move
	 * 
	 * @param other The other heap
	 */
	FibonacciHeap(FibonacciHeap&& other) noexcept {
		this->p_move(other);
	}
	
	/**
	 * @brief Destroy the Fibonacci Heap object
	 */
	~FibonacciHeap() {
		this->p_delete();
	}
	
	/**
	 * @brief Copy assignment operator
	 * Time complexity: O(other.size() + this->size())
	 *  - Because delete operator costs O(n)
	 * @param other The other heap to copy
	 * @return FibonacciHeap& Reference to *this
	 */
	FibonacciHeap& operator=(FibonacciHeap const& other) noexcept {
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
	 * @return FibonacciHeap& Reference to *this
	 */
	FibonacciHeap& operator=(FibonacciHeap&& other) noexcept {
		if (this != &other) {
			this->p_delete();
			this->p_move(other);
		}
		return *this;
	}
	
	/**
	 * @brief Swap the two heaps
	 * Time complexity: O(1)
	 * @param other The other heap to swap with
	 */
	inline void swap(FibonacciHeap& other) noexcept {
		p_adl_swap(this->comparator, other.comparator);
		p_adl_swap(this->alloc_key, other.alloc_key);

		p_adl_swap(_size, other._size);
		p_adl_swap(min, other.min);
	}

	/**
	 * @brief Push 'elem' to the heap, copying it
	 * Time complexity: O(O(elem copy))
	 * @param elem The elem to be pushed
	 * @return void* Pointer that can be used to decreaseKey
	 */
	void* push(T const& elem) {
		return this->p_emplace(elem);
	}

	/**
	 * @brief Push 'elem' to the heap, moving it
	 * Time complexity: O(log(this->size) + O(elem move))
	 * @param elem lvalue reference to the elem to be pushed
	 */
	void* push(T&& elem) {
		return this->p_emplace(std::move(elem));
	}

	/**
	 * @brief Construct element directly on the heap
	 * Time complexity: O(O(elem creation))
	 * @tparam Args Template parameter list, type parameters of the object constructor
	 * @param args Parameters list of the object constructor
	 */
	template<class... Args>
	void* emplace(Args&&... args){
		return this->p_emplace(std::forward<Args>(args)...);
	}

	/**
	 * @brief Consult top element of the heap
	 * Time complexity: O(1)
	 * @return T const& Reference to the top element
	 */
	T const& top() const {
		if (empty()) throw std::domain_error("Empty heap");
		return min->key;
	}

	/**
	 * @brief Pop element from the heap
	 * Time complexity: O(this->size)
	 * Amortized time complexity: O(log(this->size))
	 * @return T Element popped
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
	void merge(FibonacciHeap& other) {
		this->p_union(other);
	}

	/**
	 * @brief Decrements a key.
	 * Time complexity: O(log(this->size))
	 * Amortized time complexity: O(1)
	 * @param node Node pointer returned by push / emplace
	 * @param newKey New key
	 */
	void decreaseKey(void* node, T const& newKey) {
		this->p_decreaseKey((Node*)node, newKey);
	}

	/**
	 * @brief Number of elements
	 * Time complexity: O(1)
	 * @return std::size_t Number of elements
	 */
	inline std::size_t size() const {
		return this->_size;
	}

	/**
	 * @brief Number of elements
	 * Time complexity: O(1)
	 * @return std::size_t Number of elements
	 */
	inline bool empty() const {
		return this->min == nullptr;
	}
};

/**
 * @brief Swaps the two heaps
 * Time complexity: O(1)
 * This implementation is intended to be found by the ADL algorithm
 * @tparam T Same as class documentation
 * @tparam Comparator Same as class documentation
 * @tparam Allocator Same as class documentation
 * @param lhs Left heap
 * @param rhs Right heap
 */
template<typename T, class C, class A>
void swap(FibonacciHeap<T, C, A>& left, FibonacciHeap<T, C, A>& right) noexcept {
	left.swap(right);
}
