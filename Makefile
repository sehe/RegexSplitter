CXX=g++-10
CXXFLAGS=-std=c++11 -Weffc++
CXXFLAGS+=-Wextra -pedantic -Wall
CXXFLAGS+=-fsanitize=undefined
CXXFLAGS+=-fsanitize=address
CXXFLAGS+=-ggdb -fno-omit-frame-pointer -O2

INSTALL_FILES=\
	regex-splitter 
	
all : $(INSTALL_FILES)

clean :
	rm -f *.o
	rm -f $(INSTALL_FILES)

test : $(INSTALL_FILES)
	./regex-splitter
	
RegexSplitter.o : RegexSplitter.cpp RegexSplitter.h

main.o : main.cpp RegexSplitter.h
regex-splitter : main.o RegexSplitter.o
	$(CXX) -o $@ $(CXXFLAGS) $^
	
