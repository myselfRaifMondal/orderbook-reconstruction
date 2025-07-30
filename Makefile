CXX      := g++
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -march=native -flto -DNDEBUG -ffast-math -funroll-loops

all: reconstruction_blockhouse

orderbook.o: orderbook.cpp orderbook.hpp
	$(CXX) $(CXXFLAGS) -c orderbook.cpp -o orderbook.o

main.o: main.cpp orderbook.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

reconstruction_blockhouse: main.o orderbook.o
	$(CXX) $(CXXFLAGS) main.o orderbook.o -o reconstruction_blockhouse

clean:
	rm -f *.o reconstruction_blockhouse

