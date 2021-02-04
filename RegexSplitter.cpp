/*
 * RegexSplitter.cpp
 *
 *  Created on: Jan 9, 2020
 *      Author: frank
 */

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

#include <boost/optional.hpp>

#include "RegexSplitter.h"

//#define PRINT_DEBUG
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
	std::cout << "### ASTNode c'tor (ASTNode const *, std::vector<ASTNode *> &) #1: " << TypeStr(fType) << std::endl;
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
	std::cout << "### ASTNode c'tor (ASTNode const *, std::vector<ASTNode *> &) #2: " << TypeStr(fType) << std::endl;
#endif
}



// BREAKABLE_c
ASTNode::ASTNode(
		std::string & data,
		Type const type)
: fString(data, TypeToDataType(type)),
  fStrings(),
  fType(type)
{
#ifdef PRINT_DEBUG
	std::cout << "### ASTNode c'tor (std::string &) #1: " << TypeStr(fType) << std::endl;
#endif
#ifdef PRINT_DEBUG
	std::cout << "### ASTNode c'tor (std::string &) #2: " << fString << std::endl;
#endif
}

// UNBREAKABLE_c
// TODO: this does not restrict to boost::fusion
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
				delete ptr;
				ptr = nullptr;
			}
		};

		mutable std::string fCollect; // TODO: has to be MyString ref to update directly
	};

#ifdef PRINT_DEBUG
	std::cout << "### ASTNode c'tor (FUSION &) #1: " << TypeStr(fType) << std::endl;
#endif
	Collect collect;
	boost::fusion::for_each(fusion, collect);
	fString = MyString (collect.fCollect,  MyString::UNBREAKABLE_c);
#ifdef PRINT_DEBUG
	std::cout << "### ASTNode c'tor (FUSION &) #2: " << fString << std::endl;
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
	std::cout << "### ASTNode c'tor (char const)" << TypeStr(fType) << std::endl;
#endif
}
#endif

ASTNode::~ASTNode()
{

#ifdef PRINT_DEBUG
	std::cout << "### ASTNode d'tor #1: " << TypeStr(fType) << std::endl;
	if (fType == COLLECTION_c)
	{
		std::cout << "### ASTNode d'tor #2: C:" << GetString() << std::endl;
	}
	else
	{
		std::cout << "### ASTNode d'tor #2: " << fString << std::endl;
	}
#endif
}

MyString::Type
ASTNode::TypeToDataType(
		Type type)
{
	switch (type)
	{
	case BREAKABLE_c:
		return MyString::BREAKABLE_c;
		break;
	case UNBREAKABLE_c:
		return MyString::UNBREAKABLE_c;
		break;
	default:
		return MyString::UNDEFINED_c;
		break;
	}
}

std::string // TODO: have to construct std::string because of COLLECTION_c
ASTNode::GetString(void) const
{
	if (fType ==  COLLECTION_c)
	{
		std::string str;
		for (auto elem : fStrings)
		{
			str += elem.GetStr();
		}
		return str;
	}

	return fString.GetStr();
}

void
ASTNode::print(
		int /*indent*/)
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
  tok_TL_elements(), tok_TL_element(), tok_TL_group(), tok_TL_nongroup(),
  tok_nested_elements(), tok_nested_element(), tok_nested_group(), tok_nested_nongroup(),
  tok_set(), tok_positive_set(), tok_negative_set(), tok_set_items(), tok_set_item(), tok_range(), tok_char()
{
	tok_RE =
     ( tok_TL_elements )
     [ qi::_val = qi::_1 ]
	;

	// top level elements

	tok_TL_elements =
	( tok_TL_element >> *tok_TL_element )
	[ qi::_val = phx::new_<ASTNode> (qi::_1, qi::_2, ASTNode::COLLECTION_c) ]
	;

	tok_TL_element =
	( tok_TL_group | tok_set | tok_TL_nongroup )
	;

	tok_TL_group  =
	// TODO: can a group be empty?
	( qi::char_('(') >> tok_nested_elements >> qi::char_(')') >> (-qi::char_('?')) )
	[ qi::_val = phx::new_<ASTNode> (qi::_0, ASTNode::UNBREAKABLE_c) ]
	;

	tok_TL_nongroup =
	( qi::as_string[ +( (qi::char_("\\") >> qi::char_) | (qi::char_ - qi::char_("()[]")) ) ] )
	[ qi::_val = phx::new_<ASTNode> (qi::_1, ASTNode::BREAKABLE_c) ]
	;

	// the nested elements

	tok_nested_elements =
	( tok_nested_element >> *tok_nested_element )
	[ qi::_val = phx::new_<ASTNode> (qi::_1, qi::_2, ASTNode::COLLECTION_c) ]
	;

	tok_nested_element =
	( tok_nested_group | tok_set | tok_nested_nongroup )
	;

	tok_nested_group =
	( qi::char_('(') >> tok_nested_elements >> qi::char_(')') >> (-qi::char_('?')) )
	[ qi::_val = phx::new_<ASTNode> (qi::_0, ASTNode::UNBREAKABLE_c) ]
	;

	tok_nested_nongroup =
	( qi::as_string[ +( (qi::char_("\\") >> qi::char_) | (qi::char_ - qi::char_("()[]")) ) ] )
	[ qi::_val = phx::new_<ASTNode> (qi::_1, ASTNode::UNBREAKABLE_c) ]
	;

	tok_set =
	( tok_positive_set | tok_negative_set )
	[ qi::_val = phx::new_<ASTNode> (qi::_1, ASTNode::UNBREAKABLE_c) ]
	;

	// TODO: merge tok_positive_set and tok_negative_set
	tok_positive_set =
	qi::char_('[') >> tok_set_items >> qi::char_(']')
	;

	tok_negative_set =
	qi::char_('[') >> qi::char_('^') >> tok_set_items >> qi::char_(']')
	;

	tok_set_items =
	tok_set_item >> *tok_set_item
	;

	tok_set_item =
	tok_range | tok_char
	;

	tok_range =
	tok_char >> qi::char_('-') >> tok_char
	;

	tok_char =
	qi::alnum // TODO BNF: <char> ::= any non metacharacter | "\" metacharacter
	;

}

} // namespace RegexSplitter

std::ostream & operator<<(
		std::ostream & os,
		RegexSplitter::MyString const & val)
{
	return os << RegexSplitter::MyString::TypeStr(val.GetType()) << ':' << val.GetStr();
}

