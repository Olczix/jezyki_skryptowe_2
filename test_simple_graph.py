# Testowanie/porówananie działania kodu prowadzącego i mojego modułu (klasa AdjacencyMartix)
# python3 test_simple_graph.py

import simple_graphs            # import tworzonego przeze mnie modułu
from graphs import Graph        # import klasy utworzonej przez prowadzącego
                                # obie powyższe klasy mają robić to samo 

# Wykonanie dopełnienia grafu przez klasę udostępnioną przez prowadzącego
g1 = Graph("CT")
complement_g1 = g1.complement()


# Wykonanie dopełnienia grafu przez mój moduł
g = simple_graphs.AdjacencyMatrix("CT")

# TODO: wywołanie g.complement() ma zwracać nowy obiekt typu AdjacencyMatrix
# (w C++ jest to zaimplementowane jako struktura)
complement_g = g.complement()   


# TODO: sprawdzenie czy tworzenie dopełnienia grafu zadziała poprawnie
if complement_g == complement_g1:
    print("Dopełnienia grafów są równe")
