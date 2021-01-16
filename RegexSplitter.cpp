/*
 * RegexSplitter.cpp
 *
 *  Created on: Jan 9, 2020
 *      Author: frank
 */

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

#include "RegexSplitter.h"

namespace RegexSplitter {

// TODO: memory leak for releasing all the phx::new_ instances!
// TODO: nullptr checks are missing for ASTNode*

std::map<MyString::Type, std::string> const MyString::sType2String =
	{
			{MyString::UNDEFINED_c,		"?"	},
			{MyString::BREAKABLE_c,		"B"	},
			{MyString::UNBREAKABLE_c,	"U"	},
	};

std::map<ASTNode::Type, std::string> const ASTNode::sType2String =
	{
			{ASTNode::COLLECTION_c,		"Collection"	},
			{ASTNode::BREAKABLE_c,		"Breakable"		},
			{ASTNode::UNBREAKABLE_c,	"Unbreakable"	},
			{ASTNode::TEST_c,			"Test"			},
	};


std::string const
MyString::TypeStr(
		Type const & type)
{
	auto it = sType2String.find(type);
	if (it != sType2String.end())
	{
		return it->second;
	}

	return std::string("unknown:") + std::to_string(type);
}


// COLLECTION_c
ASTNode::ASTNode(
		ASTNode const * first,
		std::vector<ASTNode *> & others,
		Type const type)
: fString(),
  fStrings(),
  fType(type)
{
	fStrings.push_back(first->fString);

	delete first;
	first = nullptr;

	for (auto & other : others)
	{
		fStrings.push_back(other->fString);
		delete other;
		other = nullptr;
	}

	std::cout << "### ASTNode c'tor " << TypeStr(fType) << std::endl;
}

// BREAKABLE_c
ASTNode::ASTNode(
		std::vector<char> const & data,
		Type const type)
: fString(),
  fStrings(),
  fType(type)
{
	std::string str;
	for (auto & elem : data)
	{
		str.push_back(elem);
	}
	fString = MyString(str, MyString::BREAKABLE_c);

	std::cout << "### ASTNode c'tor " << TypeStr(fType) << std::endl;
};

// UNBREAKABLE_c
ASTNode::ASTNode(
		ASTNode const * other,
		Type const type)
: fString(),
  fStrings(),
  fType(type)
{
	fString = MyString(other->fString.GetStr(), MyString::UNBREAKABLE_c);
	delete other;
	other = nullptr;

	std::cout << "### ASTNode c'tor " << TypeStr(fType) << std::endl;
}

#if 0
	// TEST_c, used for debugging
ASTNode::ASTNode(
			char const,
			Type const type)
: fString(),
  fStrings(),
  fType(type)
{
	std::cout << "### ASTNode c'tor " << TypeStr(fType) << std::endl;
}
#endif

ASTNode::~ ASTNode()
{
	std::cout << "### ASTNode d'tor " << TypeStr(fType) << std::endl;
}

void
ASTNode::print(
		int indent)
{
	for (auto & str : fStrings)
	{
		std::cout << str << ", ";
	}
}

std::string const
ASTNode::TypeStr(
		Type const & type)
{
	auto it = sType2String.find(type);
	if (it != sType2String.end())
	{
		return it->second;
	}

	return std::string("unknown:") + std::to_string(type);
}


Grammar::Grammar()
: Grammar::base_type(tok_RE)
{
	tok_RE =
			( tok_elements ) [ qi::_val = qi::_1 ];

	tok_elements =
			( tok_element >> *tok_element ) [ qi::_val = phx::new_<ASTNode> (qi::_1, qi::_2, ASTNode::COLLECTION_c) ];

	tok_element =
			( tok_group | tok_something );

	tok_group  =
			// TODO: i want to pass the ENTIRE string including leading '(' and trailing ')'
			// TODO: can a group be empty?
			( qi::char_('(') >> tok_something >> qi::char_(')') ) [ qi::_val = phx::new_<ASTNode> (qi::_2, ASTNode::UNBREAKABLE_c) ];

	tok_something =
			( +(qi::char_ - qi::char_("()")) ) [ qi::_val = phx::new_<ASTNode> (qi::_1, ASTNode::BREAKABLE_c) ];
}

} // namespace RegexSplitter

std::ostream & operator<<(
		std::ostream & os,
		RegexSplitter::MyString const & val)
{
	return os << RegexSplitter::MyString::TypeStr(val.GetType()) << ':' << val.GetStr();
}

