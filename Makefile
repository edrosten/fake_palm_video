CXX=g++ -std=c++14
CXXFLAGS=-O3

vid:vid.o
	$(CXX) -o $@ $^ -lcvd

vid2:vid2.o
	$(CXX) -o $@ $^ -lcvd


