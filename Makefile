CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -Isrc

TARGET = compiler
OBJS = src/main.o src/parser.tab.o src/lexer.yy.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

src/main.o: src/main.cpp src/parser.tab.hh
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o src/main.o

src/parser.tab.cc src/parser.tab.hh: src/parser.yy
	bison -d -o src/parser.tab.cc src/parser.yy

src/lexer.yy.cc: src/lexer.ll src/parser.tab.hh
	flex -o src/lexer.yy.cc src/lexer.ll

clean:
	rm -f $(TARGET) src/*.o src/*.tab.cc src/*.tab.hh src/*.yy.cc src/*.hh
