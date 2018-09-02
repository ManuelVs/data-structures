#pragma once

#include <unordered_map>
#include <stdexcept>
#include <ostream>
#include <string>

// FibonacciHeap
// Una implementación de los montículos de Fibonacci
// Manuel Velasco Suárez
// Estructura de datos necesaria para la práctica de MARP
// T: Tipo de la clave
template <typename T>
class FibonacciHeap
{
public:
	// Esta estructura contiene todo lo necesario
	// para representar un nodo en el monículo de fibonacci
	struct Node {
		T key;              //La clave
		bool mark;          //Si esta marcado
		int degree;         //El grado, es decir, el numero de hijos

		Node* father;       //Puntero al padre
		Node* siblingLeft;  //Puntero al hermano izquierdo
		Node* siblingRight; //Puntero al hermano derecho
		Node* child;        //Puntero a un hijo
	};

protected:
	// Puntero al minimo
	Node* min;
	// Tamaño del monticulo
	int _size;
	// El grado maximo de un nodo
	int maxDegree;

	// Crea un nodo vacio con la clave
	// Se hace una copia de la clave
	// Se enlaza a si mismo, para facilitar futuras operaciones
	// O(1)
	Node* p_create(T const& key) {
		Node* node = new Node;
		node->key = key;
		node->mark = false;
		node->degree = 0; //No tiene hijos

		node->father = node->child = nullptr; //No tiene padre ni hijos
		node->siblingLeft = node->siblingRight = node; //Se enlaza a si mismo

		return node;
	}

	// Duplica toda la informacion de un nodo, duplicando tambien
	// a los hijos. Es util a la hora de clonar la estructura.
	// O(n); n = numero de "descendientes" y el mismo
	Node* p_duplicate(Node* other, Node* father) {
		if (other == nullptr) return nullptr;

		Node* node = new Node;
		node->key = other->key; //Se copia la clave
		node->degree = other->degree;
		node->father = father;
		node->siblingLeft = node->siblingRight = node; // Se enlaza a si mismo, es util a continuacion

		if (other->child != nullptr) { // Si tiene hijos
			Node* firstChild = other->child; // Esto servira para saber cuando acabar el bucle
			node->child = p_duplicate(firstChild, node); //Se duplica el primero y servira como "raiz"

			Node* it = firstChild->siblingLeft; //Se visita el hermano izquierdo
			while (it != firstChild) { //Si no hemos llegado al primero
				Node* dupl = p_duplicate(it, node); //entonces se duplica
				p_insertToLeft(dupl, node->child); //se inserta a la izquierda de la "raiz"

				it = it->siblingLeft; // y se visita de nuevo al hermano izquierdo
			}
		}
		else {
			// No tenia hijos
			node->child = nullptr; 
		}

		// No se conserva el orden inicial, pero no es algo que nos importe
		return node;
	}

	// Elimina un nodo y sus hijos
	// O(n); n = numero de "descendientes" y el mismo
	void p_delete(Node* node) {
		if (node == nullptr) return;
		
		Node* it = node->child; //Servira como iterador
		int i = 0;
		while (i < node->degree) { //Se tiene que ejecutar tantas veces como hijos tenga
			Node* aux = it; // Se guarda para poder visitar al hermano
			it = it->siblingLeft; // Se avanza
			p_delete(aux); // Se elimina el hermano derecho

			++i; // Se avanza
		}

		delete node; // Se elimina al fin el mismo
	}

	// Inserta un nodo a la izquierda de otro
	// O(1)
	void p_insertToLeft(Node* toInsert, Node* origin) {
		Node* aux = origin->siblingLeft; // Se guarda un puntero del hermano izquierdo

		origin->siblingLeft = toInsert; //Se enlaza el nuevo nodo a la izquierda
		toInsert->siblingRight = origin; //A la derecha del nuevo nodo esta la raiz

		toInsert->siblingLeft = aux; //A la izquierda del nuevo nodo está el antiguo
		aux->siblingRight = toInsert; //A la derecha debe estar el nuevo nodo
	}

	// Elimina a un nodo la lista a la que pertenece
	// O(1)
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

	// Enlaza a y como hijo de x
	// O(1)
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

	// Consolida el montículo
	// Coste peor: O(n); n = numero de elementos
	// Coste amortizado: O(log(n)); n = numero de elementos
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

				if (y->key < x->key) {
					// x siempre tiene la clave pequeña
					std::swap(x, y);
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
		this->maxDegree = it->second->degree;
		++it;
		while (it != a.end()) {
			// Se recorre el "array"
			p_insertToLeft(it->second, this->min); //Se inserta este nodo a la izquierda del minimo
			if (it->second->key < this->min->key) {
				this->min = it->second; //Si se encuentra un nuevo minimo, se actualiza
			}
			if (this->maxDegree < it->second->degree) {
				this->maxDegree = it->second->degree; // Si se encuentra un nodo con mayor grado, se actualiza
			}
			++it;
		}
	}

	// Quita el hijo x de y
	// O(1)
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

	// Mira si es necesario hacer un corte,
	// es decir, si esta marcado, hay que hacer
	// un corte, si no, se deja como está, pero se marca
	// para la siguiente vez
	// Coste peor: O(log(n))
	// Coste amortizado: O(1)
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

	// Inserta un nodo en la estructura
	// Se devuelve el nodo asociado para que luego se pueda
	// llamar (si es necesario) a decreaseKey
	// O(1)
	Node* p_insert(T const& key) {
		// Se crea un nodo con la clave
		Node* newNode = p_create(key);

		if (this->min == nullptr) {
			// Si el monticulo estaba vacio, es el nuevo minimo
			this->min = newNode;
			_size = 1;
		}
		else {
			// Si no estaba vacio, se inserta a la izquierda
			p_insertToLeft(newNode, this->min);

			// Se actualiza el minimo si es necesario
			if (newNode->key < this->min->key) {
				this->min = newNode;
			}
			++_size;
		}

		return newNode;
	}

	// Une dos monticulos. El monticulo pasado como referencia se "destruye" (pasa a ser vacio)
	// O(1)
	void p_union(FibonacciHeap<T>& other) {
		if (other.min == nullptr) return;

		/************ Concatenate ************/
		this->_size += other._size;
		if (this->maxDegree < other.maxDegree) this->maxDegree = other.maxDegree;

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
		if (other.min->key < this->min->key) {
			this->min = other.min;
		}

		// Hay que dejar en buen estado el otro
		other.min = nullptr;
		other._size = 0;
		other.maxDegree = 0;
	}

	// Decrementa una clave
	// Coste peor: O(log(n))
	// Coste amortizado: O(1)
	void p_decreaseKey(Node* x, T const& newKey) {
		if (x->key < newKey) {
			throw std::invalid_argument("New key is greater than current key");
		}

		x->key = newKey;
		Node* y = x->father;

		if (y != nullptr && x->key < y->key) {
			// No está en buena relación
			p_cut(x, y); //Se corta
			p_cascadingCut(y); //Se mira si hay que cortar su padre
		}

		if (x->key < this->min->key) {
			// Se actualiza si es necesario
			this->min = x;
		}
	}

	// Elimina el minimo del monticulo
	// Coste peor: O(n)
	// Coste amortizado: O(log(n))
	T p_pop() {
		Node* z = this->min;
		if (z != nullptr) {
			int i = 0;
			Node* it = z->child;
			while (i < z->degree) {
				// Desgraciadamente, hay que hacer este bucle, ya que hay que:
				// - Poner los "father" a null
				// - Comprobar el maxDegree
				// - Desmarcarlo
				Node* aux = it;
				it = it->siblingLeft;

				p_insertToLeft(aux, this->min);
				aux->father = nullptr;
				aux->mark = false;

				if (this->maxDegree < aux->degree) this->maxDegree = aux->degree;
				++i;
			}

			--this->_size;
			if (z == z->siblingLeft) {
				// Si era el ultimo que quedaba
				this->min = nullptr;
				this->maxDegree = 0;
			}
			else {
				this->min = z->siblingRight; // Temporal, hasta que consolidate haga su trabajo
				p_delFromList(z);
				p_consolidate();
			}
		}

		T key = z->key;
		delete z; // Se elimina
		return key; //Se devuelve la clave
	}

	// Crea la estructura vacia
	// O(1)
	inline void p_new() {
		min = nullptr;
		_size = 0;
		maxDegree = 0;
	}

	// Elimina la estructura
	// O(n)
	inline void p_delete() noexcept {
		if (min != nullptr) {
			Node* it = min;

			while (it != it->siblingLeft) {
				Node* aux = it;
				it = it->siblingLeft;

				p_delFromList(aux);
				p_delete(aux);
			}
			p_delete(it);
		}
	}

	// Clona otra estructura a esta. Elimina toda la informacion de la actual
	// O(n1 + n2);
	//  n1 = Numero de nodos en la original
	//  n2 = Numero de nodos en la que se quiere copiar
	inline void p_copy(FibonacciHeap<T> const& other) noexcept {
		if (other.min == nullptr) {
			this->min = nullptr;
			this->_size = 0;
			this->maxDegree = 0;
		}
		else {
			this->min = p_duplicate(other.min, nullptr);

			Node* it = other.min->siblingLeft;
			while (it != other.min) {
				Node* dupl = p_duplicate(it, nullptr);
				p_insertToLeft(dupl, this->min);

				it = it->siblingLeft;
			}

			this->_size = other._size;
			this->maxDegree = other.maxDegree;
		}
	}

	// Mueve la estructura, eliminando la otra
	// O(n); n = Numero de nodos en la original, normalmente es 0, al menos que
	// haga a posta
	inline void p_move(FibonacciHeap<T>& other) noexcept {
		this->min = other.min;
		this->_size = other._size;
		this->maxDegree = other.maxDegree;

		other.p_new();
	}

	// Escribe la estructura completa, con un formato peculiar
	// O(n)
	void p_print(std::ostream& out, Node* node, std::string start) {
		out << start << node->key << (node->mark ? "!" : "") << std::endl;

		if (node->child != nullptr) {
			p_print(out, node->child, start + " ");
		}

		Node* it = node->siblingRight;
		while (it != node) {
			out << start << it->key << (it->mark ? "!" : "") << std::endl;

			if (it->child != nullptr) {
				p_print(out, it->child, start + " ");
			}

			it = it->siblingRight;
		}
	}

public:
	
	// Constructora por defecto, crea la estructura vacia
	// O(1)
	FibonacciHeap() {
		p_new();
	}
	
	// Constructora por copia
	// O(n1 + n2);
	//  n1 = Numero de datos en la original
	//  n2 = Numero de datos en la que se quiere copiar
	FibonacciHeap(FibonacciHeap<T> const& other) {
		this->p_copy(other);
	}

	// Constructora de movimiento
	// O(n); n = Numero de datos en la original, ya que la original se debe eliminar.
	// Normalmente n es 0, al menos que haga a posta y sea mas.
	FibonacciHeap(FibonacciHeap<T>&& other) noexcept {
		this->p_move(other);
	}
	
	// Destructora
	// O(n); n = Numero de datos
	~FibonacciHeap() {
		p_delete();
	}
	
	// Operador de asignacion de copia
	// O(n1 + n2);
	//  n1 = Numero de datos en la original, ya que hay que eliminar la original
	//  n2 = Numero de datos en la que se quiere copiar
	FibonacciHeap<T>& operator=(FibonacciHeap<T> const& other) noexcept {
		if (this != &other) {
			this->p_delete();
			this->p_copy(other);
		}
		return *this;
	}

	// Operador de asignacion de movimiento
	// O(n); n = Numero de datos en la original, normalmente es 0, al menos que
	// haga a posta y sea mas, ya que la original se debe eliminar.
	FibonacciHeap<T>& operator=(FibonacciHeap<T>&& other) noexcept {
		if (this != &other) {
			this->p_delete();
			this->p_move(other);
		}
		return *this;
	}
	
	// Intercambia los valores de dos monticulos
	// O(1)
	inline void swap(FibonacciHeap<T>& other) noexcept {
		std::swap(maxDegree, other.maxDegree);
		std::swap(_size, other._size);
		std::swap(min, other.min);
	}

	// Inserta un nodo en la estructura
	// Se devuelve el nodo valido para decreaseKey
	// O(1)
	Node* insert(T const& key) {
		return p_insert(key);
	}

	// Consulta el minimo
	// O(1)
	T const& top() const {
		return min->key;
	}

	// Elimina el minimo del monticulo
	// Coste peor: O(n)
	// Coste amortizado: O(log(n))
	T pop() {
		return p_pop();
	}

	// Une dos monticulos. El monticulo pasado como referencia se "destruye" (pasa a ser vacio)
	// O(1)
	void merge(FibonacciHeap<T>& other) {
		p_union(other);
	}

	// Decrementa una clave
	// Coste peor: O(log(n))
	// Coste amortizado: O(1)
	void decreaseKey(Node* node, T const& newKey) {
		p_decreaseKey(node, newKey);
	}

	// Consulta el tamaño del monticulo
	// O(1)
	inline int size() const {
		return this->_size;
	}

	// Comprueba si la estructura está vacia
	// O(1)
	inline bool empty() const {
		return this->min == nullptr;
	}

	// Escribe la estructura completa, con un formato peculiar
	// O(n)
	void print(std::ostream& out) {
		if(this->min != nullptr)
			p_print(out, this->min, "");
	}
};

namespace std {
	template <typename T>
	inline void swap(FibonacciHeap<T>& left, FibonacciHeap<T>& right) noexcept {
		left.swap(right);
	}
}

