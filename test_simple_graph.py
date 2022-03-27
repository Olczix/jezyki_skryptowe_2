import simple_graphs            
from graphs import Graph        

g6_format = "G???F?"
for i in range(1000):
    g1 = Graph(g6_format)
    g2 = g1.smoothing()

    g = simple_graphs.AdjacencyMatrix(g6_format)
    g.smoothing()

    if g2 == g:
        print(f"[{i}] OK")
    else:
        print(f"[{i}] FAIL")
    
    # if g1.smoothing() == g.smoothing():
    #     print(f"[{i}] OK")
    # else:
    #     print(f"[{i}] FAIL")
