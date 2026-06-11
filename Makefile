CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wno-unused-function -Iinclude -Isrc

TARGET = compiler
OBJS = src/main.o src/parser.tab.o src/lexer.yy.o src/Driver.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

src/main.o: src/main.cpp src/parser.tab.hh
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o src/main.o

src/Driver.o: src/Driver.cpp include/Driver.hpp
	$(CXX) $(CXXFLAGS) -c src/Driver.cpp -o src/Driver.o

src/parser.tab.cc src/parser.tab.hh: src/parser.yy
	bison -d -o src/parser.tab.cc src/parser.yy

src/lexer.yy.cc: src/lexer.ll src/parser.tab.hh
	flex -o src/lexer.yy.cc src/lexer.ll

src/parser.tab.o: src/parser.tab.cc
	$(CXX) $(CXXFLAGS) -c src/parser.tab.cc -o src/parser.tab.o

src/lexer.yy.o: src/lexer.yy.cc
	$(CXX) $(CXXFLAGS) -c src/lexer.yy.cc -o src/lexer.yy.o

clean:
	rm -f $(TARGET) src/*.o src/*.tab.cc src/*.tab.hh src/*.yy.cc src/*.hh
