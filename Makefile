CXXFLAGS=-std=c++11 -Weffc++

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
	$(CXX) -o $@ $^
	
