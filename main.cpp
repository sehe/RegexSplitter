#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <memory>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
namespace qi = boost::spirit::qi;

#include "RegexSplitter.h"

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
	RegexSplitter::ASTNode* out_node;
    bool result = Parse(input, RegexSplitter::Grammar(), out_node);

    if (result) {
		std::cout << "print():" << input << std::endl;
		out_node->print(0);
		std::cout << std::endl;
		delete out_node;
		std::cout << std::endl;
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
    	// some initial trials
        TestRegexSplitter("hello");
        TestRegexSplitter("(hello)");
        TestRegexSplitter("(hello)(world)");
        TestRegexSplitter("(hello)something(world)");
#if 0
        TestRegexSplitter("aa");
        TestRegexSplitter("Hello, World!");
        TestRegexSplitter("FrankBergemann");

        // groups
        TestRegexSplitter("[a]");
        TestRegexSplitter("[ab]");
        TestRegexSplitter("[a-z]");
        TestRegexSplitter("[b-c]");
        TestRegexSplitter("[A-Z]");
        TestRegexSplitter("[a-zA-Z]");
        TestRegexSplitter("[abcABC]");

        // negative groups
        TestRegexSplitter("[^a]");
        TestRegexSplitter("[^ab]");
        TestRegexSplitter("[^a-z]");
        TestRegexSplitter("[^b-c]");
        TestRegexSplitter("[^A-Z]");
        TestRegexSplitter("[^a-zA-Z]");
        TestRegexSplitter("[^abcABC]");

        // '*'
        TestRegexSplitter("[abcABC]*");
        TestRegexSplitter("[^abcABC]*");

        // '+'
        TestRegexSplitter("[abcABC]+");
        TestRegexSplitter("[^abcABC]+");

        TestRegexSplitter("SomeStrange.*Patter[nN]");
        TestRegexSplitter("^SomeStrange.*Patter[nN]");
#endif
    }

    return 0;
}

/******************************************************************************/
