CXX = clang++
CXXFLAGS = -std=c++11 -g -O2 -I./hume -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer

all: treeviz

treeviz: main.o random-bst/RandomBST.c random-bst/RandomBSTNode.c hume/libhume.a
	$(CXX) $(CXXFLAGS) $^ -o $@

main.o: main.cpp

.PHONY: clean

clean:
	$(RM) treeviz *.o
