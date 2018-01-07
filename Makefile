CXX=g++
CFLAGS = -std=c++11 -m64 -march=native -pedantic -Wall -Wno-unused-function -Wno-unused-result -ftree-vectorize -funroll-loops -Ofast
#CFLAGS = -std=c++11 -g -m64 -Wall -Wno-unused-function -Wno-unused-result

OPENCV_LIBS=`pkg-config --cflags --libs opencv`

all: main main2 main3

main: main.cpp avl_tree.o distribfunction.o kmax_distance.o
	$(CXX) $(CFLAGS) -o main main.cpp avl_tree.o distribfunction.o kmax_distance.o -L$(OPENCV_LIBS)

main2: main2.cpp avl_tree.o distribfunction.o kmax_distance.o
	$(CXX) $(CFLAGS) -o main2 main2.cpp avl_tree.o distribfunction.o kmax_distance.o -L$(OPENCV_LIBS)

main3: main3.cpp avl_tree.o distribfunction.o kmax_distance.o
	$(CXX) $(CFLAGS) -o main3 main3.cpp avl_tree.o distribfunction.o kmax_distance.o -L$(OPENCV_LIBS)

kmax_distance.o: kmax_distance.cpp
	$(CXX) $(CFLAGS) -c kmax_distance.cpp

avl_tree.o: avl_tree.c
	$(CXX) $(CFLAGS) -c avl_tree.c

distribfunction.o: distribfunction.cpp
	$(CXX) $(CFLAGS) -c distribfunction.cpp

clean:
	rm -f main main2 main3 *.o