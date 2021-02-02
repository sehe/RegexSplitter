#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
namespace qi = boost::spirit::qi;

#include "RegexSplitter.h"

static void ReadFromFile (
		std::string const & fileName,
		std::vector<std::string> & list)
{
	list.clear();
	std::ifstream file;
	file.open(fileName);
    	if (!file)
        	throw std::runtime_error("File not found!!");
	else {
		for (std::string line; std::getline(file, line); /* */) {
			list.push_back(line);
		}
	}
}

template <typename Parser, typename ... Args>
bool
Parse(
    const std::string& input,
	const Parser& p,
    Args&& ... args)
{
	std::string::const_iterator begin = input.begin(), end = input.end();

    bool result = qi::parse(begin, end, p, std::forward<Args>(args) ...);

    if (!result || begin != end)
    {
        std::cout
	<< "Unparseable: '" << input << "', failed at: '" << std::string(begin, end) << "'" << std::endl;

        result = false;
    }

    return result;
}


void
TestRegexSplitter(
		std::string const & input)
{
	RegexSplitter::ASTNode* out_node = nullptr;

	std::cout << "TEST:" << input << std::endl;

	bool result = Parse(input, RegexSplitter::Grammar(), out_node);
	if (result) {
		if (out_node != nullptr) {
			out_node->print(0);
			std::cout << std::endl;
			delete out_node;
			std::cout << std::endl;
		}
	}
}
/******************************************************************************/

int
main(
		int argc,
		char* argv[])
{
	if (argc >= 2)
	{
		TestRegexSplitter(argv[1]);
	}
	else
	{
	    std::vector<std::string> reList;
	    ReadFromFile("testfile-ok", reList);
	    for (auto & re : reList)
	    {
		    TestRegexSplitter(re);
	    }

	}

	return 0;
}

/******************************************************************************/
