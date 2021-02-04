/*
 * RegexSplitter.h
 *
 *  Created on: Jan 9, 2020
 *      Author: frank
 */
#pragma once
#include <boost/spirit/include/qi.hpp>

namespace RegexSplitter
{
    namespace qi = boost::spirit::qi;

    struct COLLECTION_c;
    struct BREAKABLE_c;
    struct UNBREAKABLE_c;

    using Iterator = std::string::const_iterator;
    using Range = boost::iterator_range<Iterator>;

    struct Token {
        enum Kind { Group, Sequence } kind;
        Range range;
    };

    using Tokens = qi::unused_type; // std::vector<Token>;

    struct Grammar : public qi::grammar<Iterator, Tokens()>
    {
        Grammar() : Grammar::base_type(tok_TL_elements)
        {
            // top level elements
            tok_TL_elements     = +tok_TL_element;
            tok_TL_element      = (tok_TL_group | tok_set | tok_TL_nongroup);
            tok_TL_group        = ('(' >> tok_nested_elements >> ')' >> -qi::char_('?'));
            tok_TL_nongroup     = (qi::as_string[+((qi::char_("\\") >> qi::char_) |
                        (qi::char_ - qi::char_("()[]")))]);

            // the nested elements
            tok_nested_elements = *tok_nested_element;
            tok_nested_element  = (tok_nested_group | tok_set | tok_nested_nongroup);
            tok_nested_group    = '(' >> tok_nested_elements >> ')' >> -qi::char_('?');
            tok_nested_nongroup = +('\\' >> qi::char_ | ~qi::char_("()[]"));
            tok_set             = (tok_positive_set | tok_negative_set);

            // TODO: merge tok_positive_set and tok_negative_set
            tok_positive_set    = '[' >> tok_set_items >> ']';
            tok_negative_set    = "[^" >> tok_set_items >> ']';

            tok_set_items       = +tok_set_item;
            tok_set_item        = tok_range | tok_char;
            tok_range           = tok_char >> '-' >> tok_char;
            // TODO BNF: <char> ::= any non metacharacter | "\" metacharacter
            tok_char = qi::alnum;
        }

        qi::rule<Iterator, Tokens()> tok_RE, tok_TL_elements,
            tok_TL_element, tok_TL_group, tok_TL_nongroup, tok_nested_elements,
            tok_nested_element, tok_nested_group, tok_nested_nongroup, tok_set;

        // no AST nodes required for these
        qi::rule<Iterator, std::string()> tok_positive_set, tok_negative_set,
            tok_set_items, tok_set_item, tok_range, tok_char;
    };

} // namespace RegexSplitter
