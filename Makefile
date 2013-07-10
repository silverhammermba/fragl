CXXFLAGS=-std=c++11 -ggdb

fract: main.o
	$(CXX) $(CXXFLAGS) -o fract $+ -lglfw -lGLEW -lGL
