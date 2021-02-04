#pragma once
//#define BOOST_SPIRIT_DEBUG
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted.hpp>

namespace RegexSplitter {
    using Iterator = std::string::const_iterator;
    using Range    = boost::iterator_range<Iterator>; // std::string; 

    struct Token {
        enum Kind { Group, Set, Nongroup } kind;
        Range range;
    };

    using Tokens = std::vector<Token>;
}

BOOST_FUSION_ADAPT_STRUCT(RegexSplitter::Token, kind, range)

namespace RegexSplitter {
    namespace qi = boost::spirit::qi;

    struct Grammar : public qi::grammar<Iterator, Tokens()> {
        Grammar() : Grammar::base_type(elements) {
            // elements
            elements = *element;
            element  = group | set | nongroup;

            group    = qi::attr(Token::Group) >> qi::raw [
                       '(' >> elements >> ')' >> -qi::char_('?')
                     ];

            nongroup = qi::attr(Token::Nongroup) >> qi::raw [
                       +('\\' >> qi::char_ | ~qi::char_("()[]"))
                     ];

            set      = qi::attr(Token::Set) >> qi::raw [
                       '[' >> qi::matches['^'] >> *set_item >> ']'
                     ];

            // sets
            set_item = range | char_;
            range    = char_ >> '-' >> char_;
            char_    = qi::alnum; // TODO BNF: <char> ::= any non metacharacter | "\" metacharacter

            BOOST_SPIRIT_DEBUG_NODES((elements)(element)(group)(nongroup)(set)
                (set_item)(range)(char_)
            )
        }

      private:
        qi::rule<Iterator, Tokens()> elements;
        qi::rule<Iterator, Token()>  element, group, nongroup, set;
        qi::rule<Iterator>           set_item, range, char_;
    };

} // namespace RegexSplitter
