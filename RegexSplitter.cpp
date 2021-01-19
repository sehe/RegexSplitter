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

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

#include <boost/optional.hpp>

#include "RegexSplitter.h"

#define PRINT_DEBUG

namespace RegexSplitter {

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
	{ASTNode::BREAKABLE_c,		"Breakable"	},
	{ASTNode::UNBREAKABLE_c,	"Unbreakable"	},
	{ASTNode::TEST_c,		"Test"		},
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
#ifdef PRINT_DEBUG
	std::cout << "### ASTNode c'tor: " << TypeStr(fType) << std::endl;
#endif

	fStrings.push_back(first->fString);

	delete first;
	first = nullptr;

	for (auto & other : others)
	{
		fStrings.push_back(other->fString);
		delete other;
		other = nullptr;
	}
#ifdef PRINT_DEBUG
	std::cout << "### ASTNode c'tor: " << TypeStr(fType) << std::endl;
#endif
}



// BREAKABLE_c
ASTNode::ASTNode(
		std::vector<char> & data,
		Type const type)
: fString(),
  fStrings(),
  fType(type)
{
#ifdef PRINT_DEBUG
	std::cout << "### ASTNode c'tor #1: " << TypeStr(fType) << std::endl;
#endif
	std::string str;
	for (auto & elem : data)
	{
		str.push_back(elem);
	}
	fString = MyString(str, MyString::BREAKABLE_c);
#ifdef PRINT_DEBUG
	std::cout << "### ASTNode c'tor #2: " << fString << std::endl;
#endif
};

// UNBREAKABLE_c
template < class FUSION >
ASTNode::ASTNode(
		FUSION & fusion,
		Type const type)
: fString(),
  fStrings(),
  fType(type)
{
	// local helper
	struct Collect
	{
		Collect()
		: fCollect()
		{ }

		void operator()( char c ) const
		{
#ifdef PRINT_DEBUG
			std::cout << "--- collect char" << std::endl;
#endif
			fCollect += c;
		};

		void operator() (boost::optional<char> & c) const
		{
#ifdef PRINT_DEBUG
			std::cout << "--- collect optional char" << std::endl;
#endif
			if (c)
			{
				fCollect += *c;
			}
		}

		void operator() (ASTNode * ptr ) const
		{
#ifdef PRINT_DEBUG
			std::cout << "--- collect ASTNode *" << std::endl;
#endif

			if (ptr != nullptr)
			{
				fCollect += ptr->GetString();
			}
		};

		mutable std::string fCollect; // TODO: has to be MyString ref to update directly
	};

#ifdef PRINT_DEBUG
	std::cout << "### ASTNode c'tor #1: " << TypeStr(fType) << std::endl;
#endif
	Collect collect;
	boost::fusion::for_each(fusion, collect);
	fString = MyString (collect.fCollect,  MyString::UNBREAKABLE_c);
#ifdef PRINT_DEBUG
	std::cout << "### ASTNode c'tor #2: " << fString << std::endl;
#endif
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
#ifdef PRINT_DEBUG
	std::cout << "### ASTNode c'tor " << TypeStr(fType) << std::endl;
#endif
}
#endif

ASTNode::~ASTNode()
{
	std::cout << "### ASTNode d'tor " << TypeStr(fType) << std::endl;
}

std::string const &
ASTNode::GetString(void) const
{
	return fString.GetStr();
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
: Grammar::base_type(tok_RE),
  tok_RE(),
  tok_elements(),
  tok_element(),
  tok_group(),
  tok_something()
{
	tok_RE =
			( tok_elements ) [ qi::_val = qi::_1 ];

	tok_elements =
			( tok_element >> *tok_element ) [ qi::_val = phx::new_<ASTNode> (qi::_1, qi::_2, ASTNode::COLLECTION_c) ];

	tok_element =
			( tok_group | tok_something );

	tok_group  =
			// TODO: can a group be empty?
			( qi::char_('(') >> tok_something >> qi::char_(')') >> (-qi::char_('?')) ) [ qi::_val = phx::new_<ASTNode> (qi::_0, ASTNode::UNBREAKABLE_c) ];

	tok_something =
			( +(qi::char_ /* - qi::char_("()") */) ) [ qi::_val = phx::new_<ASTNode> (qi::_1, ASTNode::BREAKABLE_c) ];
}

} // namespace RegexSplitter

std::ostream & operator<<(
		std::ostream & os,
		RegexSplitter::MyString const & val)
{
	return os << RegexSplitter::MyString::TypeStr(val.GetType()) << ':' << val.GetStr();
}

