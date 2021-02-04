CXX=g++-10
CXXFLAGS=-std=c++11  #-Weffc++ considered harmful
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
	

tidy :
	run-clang-tidy-9 -fix -header-filter='.*' -checks='-*,readability-*,modernize-*,-modernize-use-trailing-return-type'


.PHONY : tidy clean
