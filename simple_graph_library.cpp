// KOD MODUŁU
// Kompilacja modułu za pomocą komendy:
// python setup_simple_graph.py install

#include <Python.h>
#include <iostream>
#define VERTICES 16
using namespace std;

int count_ones(int number){
  int counter = 0;
  while (number) {
    counter += number & 1;
    number >>= 1;
  }
  return counter;
}

// Struktura reprezentująca macierz sąsiedztwa
typedef struct {
    PyObject_HEAD       // część inicjalizacyjna
    unsigned short vertices_int = 0;
    unsigned short edges_array[VERTICES];
} AdjacencyMatrix;

// Konstruktor typu AdjacencyMatrix.
static PyObject *AdjacencyMatrix__new__( PyTypeObject *type, PyObject *args ) {
    return type->tp_alloc( type, 0 );
}

// Destruktor typu AdjacencyMatrix.
static void AdjacencyMatrix__del__( AdjacencyMatrix *self ) {
    Py_TYPE( self )->tp_free( (PyObject *)self );
}

// Inicjalizator typu AdjacencyMatrix.
static int AdjacencyMatrix__init__(AdjacencyMatrix *self, PyObject *args ) {
    char* g6_format;
    if (PyArg_ParseTuple( args, "s", &g6_format)) {
        // Pobieranie informacji o wierzchołkach
        int vertices_number = g6_format[0] - 63;
        for(int i = VERTICES - 1; i >= VERTICES - vertices_number; i--) {
            self->vertices_int += 1 << i;
        }

        // Inicjalizacja zerami w tablicy krawędzi
        for(int i = 0 ; i < VERTICES; i++){
            self->edges_array[i] = 0;
        }  

        // extract information about edges
        int k = 0, i = 1, c;
        for(int v = 1; v < vertices_number; v++) {
            for (int u = 0; u < v; u++ ) {
                if (k == 0) {
                    c = g6_format[i] - 63;
                    i++;
                    k = 6;
                }
                k--;
                if ((c & (1 << k)) != 0) {
                    self->edges_array[u] += (1 << (VERTICES - 1 - v));    // edge between u and v!
                    self->edges_array[v] += (1 << (VERTICES - 1 - u));    // and also edge between v and u
                }
            }       
        }
        
        return 0;
    }
    return -1;
}

static PyObject *AdjacencyMatrix__str__( AdjacencyMatrix *self ) {
    PyObject *r = PyUnicode_FromFormat( "AdjacencyMatrix to string\n");
    return r;
}

// Ilość wierzchołków grafu.
static PyObject *method_number_of_vertices( AdjacencyMatrix *self ) {
    return PyLong_FromLong(count_ones(self->vertices_int));
}

// Wierzchołki grafu.
static PyObject *method_vertices( AdjacencyMatrix *self ) {
    PyObject *results_set = PySet_New(NULL);
    int v = self->vertices_int;
    for(int i = VERTICES - 1; i >= 0; i--) {
        if( v % 2 == 1){
            PySet_Add(results_set, PyLong_FromLong(i));
        }
        v /= 2;
    }
    return results_set;
}


// Stopień wierzchołka.
static PyObject *method_vertex_degree( AdjacencyMatrix *self, PyObject *args ) {
    int vertex;
    PyArg_ParseTuple( args, "i", &vertex);
    return PyLong_FromLong(count_ones(self->edges_array[vertex]));
}


// Sąsiedztwo wierzchołka.
static PyObject *method_vertex_neighbors( AdjacencyMatrix *self, PyObject *args ) {
    int vertex;
    PyArg_ParseTuple( args, "i", &vertex);
    PyObject *neighbours = PySet_New(NULL);
    int all_neighbours = self->edges_array[vertex];
    for(int i = VERTICES - 1; i >= 0; i--) {
        if(all_neighbours % 2 == 1){
            PySet_Add(neighbours, PyLong_FromLong(i));
        }
        all_neighbours /= 2;
      }
      return neighbours;
}


// Dodanie wierzchołka do grafu. (metoda typu 'void')
static PyObject *method_add_vertex( AdjacencyMatrix *self, PyObject *args ) {
    int vertex;
    PyArg_ParseTuple( args, "i", &vertex);
    int vertex_index = VERTICES - 1 - vertex;
    int is_vertex = ((self->vertices_int) >> vertex_index) % 2;
    if(is_vertex == 0) { 
        self->vertices_int += (1 << vertex_index);
    }
    Py_RETURN_NONE;
}


// Usunięcie wierzchołka do grafu. (metoda typu 'void')
static PyObject *method_delete_vertex( AdjacencyMatrix *self, PyObject *args ) {
    int vertex;
    PyArg_ParseTuple( args, "i", &vertex);

    self->vertices_int -= (1 << (VERTICES - 1 - vertex));
    self->edges_array[vertex] = 0;
    for(int i = 0; i < VERTICES; i++){
        if(i != vertex){
            if(self->edges_array[i] >> (VERTICES - 1 - vertex) % 2 == 1)
                self->edges_array[i] -= (1 << (VERTICES - 1 - vertex));
        }
    }
    Py_RETURN_NONE;
}


// Liczba krawędzi w grafie.
static PyObject *method_number_of_edges( AdjacencyMatrix *self ){
    int all_edges = 0;
    for(int i = 0; i < VERTICES; i++){
        all_edges += count_ones(self->edges_array[i]);
    }
    return PyLong_FromLong(all_edges/2);
}


// Krawędzie grafu.
static PyObject *method_edges( AdjacencyMatrix *self ) {
    PyObject *results_set = PySet_New(NULL);
    for(int i = 0; i < VERTICES; i++){
        for(int j = i; j < VERTICES; j++){
            if((self->edges_array[i] >> (VERTICES - 1 - j)) % 2){
                PyObject *tuple = PyTuple_New(2);
                PyTuple_SetItem(tuple, 0, PyLong_FromLong(i));
                PyTuple_SetItem(tuple, 1, PyLong_FromLong(j));
                PySet_Add(results_set, tuple);
                Py_DECREF(tuple);
            }
        }
    }
    return results_set;
}

// Czy wierzchołki ze sobą sąsiadują
static PyObject *method_is_edge( AdjacencyMatrix *self, PyObject *args ) {
    Py_INCREF(Py_True);
    PyObject* result = Py_True;   
    int v1, v2;
    PyArg_ParseTuple( args, "|ii", &v1, &v2);
    if((self->edges_array[v1] >> (VERTICES - 1 - v2)) % 2 == 1){
        return result;
    }
    Py_DECREF(result);
    Py_INCREF(Py_False);
    result = Py_False; 
    return result;
}


// Dodanie krawędzi. (metoda typu 'void')
static PyObject *method_add_edge( AdjacencyMatrix *self, PyObject *args ) {
    int v1, v2;
    PyArg_ParseTuple( args, "ii", &v1, &v2);
    self->edges_array[v1] += (1 << (VERTICES - 1 - v2));
    self->edges_array[v2] += (1 << (VERTICES - 1 - v1));
    Py_RETURN_NONE;
}


// Usunięcie krawędzi. (metoda typu 'void')
static PyObject *method_delete_edge( AdjacencyMatrix *self, PyObject *args ) {
    int v1, v2;
    PyArg_ParseTuple( args, "ii", &v1, &v2);
    self->edges_array[v1] -= (1 << (VERTICES - 1 - v2));
    self->edges_array[v2] -= (1 << (VERTICES - 1 - v1));
    Py_RETURN_NONE;
}

// Dopełenienie grafu.
static PyObject *method_complement( AdjacencyMatrix *self ){
    AdjacencyMatrix matrix;       // inicjalizacja macierzy
    
    // Inicjalizacja zerami w tablicy krawędzi
    for(int i = 0 ; i < VERTICES; i++) {
        matrix.edges_array[i] = 0;
    }
    
    // Inicjalizacja listy wierzchołków
    matrix.vertices_int = self->vertices_int;

    // Dopełnienie
    for(int i = 0; i < VERTICES; i++) {
        if((self->vertices_int >> (VERTICES - 1 - i)) % 2 == 1) {
            matrix.edges_array[i] = ((~self->edges_array[i]) - (1 << (VERTICES - 1 - i))) & self->vertices_int;
        }
    }

    // TODO: chwilowo to jest None, bo nie umiem zwrócić struktury
    Py_RETURN_NONE;

    // TODO: jak tutaj zwrócić strukturę, aby Python uznał to za obiekt typu AdjacencyMatrix?
    // return matrix;
}


// Lista wszystkich dostępnych metod w tym module wraz z opiem, etc.
static PyMethodDef SimpleGraphsMethods[] = {
    {"number_of_vertices", (PyCFunction)method_number_of_vertices, METH_NOARGS, "Return number of vertices in graph."},
    {"vertices", (PyCFunction)method_vertices, METH_NOARGS, "Return vertices in graph."},
    {"vertex_degree", (PyCFunction)method_vertex_degree, METH_VARARGS, "Return vertex degree."},
    {"vertex_neighbors", (PyCFunction)method_vertex_neighbors, METH_VARARGS, "Return vertex neighbours."},
    {"add_vertex", (PyCFunction)(void(*)(void))method_add_vertex, METH_VARARGS, "Add vertex to the graph."},
    {"delete_vertex", (PyCFunction)(void(*)(void))method_delete_vertex, METH_VARARGS, "Delete vertex from graph."},
    {"number_of_edges", (PyCFunction)method_number_of_edges, METH_NOARGS, "Return number of edges in graph."},
    {"edges", (PyCFunction)method_edges, METH_VARARGS, "Return edges in graph."},
    {"is_edge", (PyCFunction)method_is_edge, METH_VARARGS, "Return information if given vertices are neighbours."},
    {"add_edge", (PyCFunction)(void(*)(void))method_add_edge, METH_VARARGS, "Add edge to the graph."},
    {"delete_edge", (PyCFunction)(void(*)(void))method_delete_edge, METH_VARARGS, "Delete edge from graph."},
    // TODO: Tutaj są wszystkie wspominki na funkcje, które są w tym module dostępne. Ta poniżej chwilowo ma (void)
    // ze względu na to, że nie potrafię zwrócić struktury. Gdzieś wyczytałam, że to musi być typ chyba "PyObject*" (?)
    {"complement", (PyCFunction)(void(*)(void))method_complement, METH_NOARGS, "Return complement of the graph."},
    {NULL}
};

// Opis typu AdjacencyMatrix.
static PyTypeObject AdjacencyMatrixType = {
    PyVarObject_HEAD_INIT( NULL,0 ) // inicjalizacja
    "simple_graphs.AdjacencyMatrix", // nazwa
    sizeof( AdjacencyMatrix ), // rozmiar
    0, //
    (destructor)AdjacencyMatrix__del__, // destruktor
    0,0,0,0,0,0,0,0,0,0, //
    (reprfunc)AdjacencyMatrix__str__, // obiekt->napis
    0,0,0, //
    Py_TPFLAGS_DEFAULT, //
    "Matrix", // opis
    0,0,0,0,0,0, //
    SimpleGraphsMethods, // metody
    0,0,0,0,0,0,0, //
    (initproc)AdjacencyMatrix__init__, // inicjalizator
    0, //
    (newfunc)AdjacencyMatrix__new__ // konstruktor
};


static struct PyModuleDef simplegraphsmodule = {
    PyModuleDef_HEAD_INIT,
    "simple_graphs",
    "Python interface for the simple_graphs C/C++ library functions",
    -1,
    SimpleGraphsMethods
};


// Inicjalizacja modułu
PyMODINIT_FUNC PyInit_simple_graphs( void ) {
    if (PyType_Ready( &AdjacencyMatrixType ) < 0) return NULL;
    
    PyObject* m = PyModule_Create( &simplegraphsmodule );
    if (m == NULL) return NULL;
    Py_INCREF( &AdjacencyMatrixType );
    PyModule_AddObject( m, "AdjacencyMatrix", (PyObject *)&AdjacencyMatrixType );
    return m;
}
