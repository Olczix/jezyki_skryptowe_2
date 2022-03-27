# TWORZENIE MODUŁU Z POZIOMU PYTHON

# Komenda: python3 setup_simple_graph.py install 
# Ta komenda tworzy moduł "simple_graphs"

from distutils.core import setup, Extension

def main():
    setup(name="simple_graphs",
          version="1.0.0",
          description="Python interface for the simple_graph C library functions",
          author="Aleksandra Barska",
          author_email="barska-ola@tlen.pl",
          ext_modules=[Extension("simple_graphs", ["simple_graph_library.cpp"])])

if __name__ == "__main__":
    main()
