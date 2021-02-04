/*
 * RegexSplitter.h
 *
 *  Created on: Jan 9, 2020
 *      Author: frank
 */

#ifndef REGEX_SPLITTER_H_
#define REGEX_SPLITTER_H_

#include <string>
#include <utility>

#include <vector>
#include <map>
#include <iostream>

#include <boost/spirit/include/qi.hpp>
namespace qi = boost::spirit::qi;

// for testing parser only, without AST construction
#undef PARSER_ONLY

namespace RegexSplitter
{
class MyString;
}

std::ostream & operator<<(
		std::ostream & os,
		RegexSplitter::MyString const & val);

namespace RegexSplitter
{

class MyString
{
public:
    enum Type
	{
		UNDEFINED_c		= 0,
		BREAKABLE_c		= 1,
		UNBREAKABLE_c	= 2,
	};

	MyString (
			std::string str = {},
			Type const type = UNDEFINED_c)
	: fStr(std::move(str)),
	  fType(type)
	{ }

	// other standard functions: compiler generated

	std::string const &
	GetStr() const
	{
		return fStr;
	}

	Type const &
	GetType() const
	{
		return fType;
	}

	static std::string 
	TypeStr(Type const & type);

private:
	std::string fStr;
	Type fType{UNDEFINED_c};
	static std::map<Type, std::string> const sType2String;
};


class ASTNode
{
public:
	typedef enum
	{
		COLLECTION_c	= 1,
		BREAKABLE_c		= 2,
		UNBREAKABLE_c	= 3,
		TEST_c			= 4,
	} Type;

	// TODO: use Type 1st and vargs next for c'tors? And dispatch to value-templates? to have a unique function structure

	// COLLECTION_c
	ASTNode(
			ASTNode const * first,
			std::vector<ASTNode *> & others,
			Type type);

	// BREAKABLE_c
	ASTNode(
			std::string & data,
			Type type);

	// UNBREAKABLE_c
	template < class FUSION >
	ASTNode(
			FUSION & fusion,
			Type type);

#if 0
	// TEST_c, used for debugging
	ASTNode(
			char const,
			Type const type);
#endif

	ASTNode(ASTNode const &rhs) = delete;
	ASTNode & operator=(ASTNode const &rhs) = delete;

	void print(
			int indent = 0);

	static std::string 
	TypeStr(
			Type const & type);

	std::string
	GetString() const;

	static MyString::Type
	TypeToDataType(
			Type type);

	virtual ~ASTNode();

private:
	// TODO: don't use blob -> sub-class
	MyString fString;
	std::vector<MyString> fStrings;
	Type fType;
	static std::map<Type, std::string> const sType2String;
};

#ifdef PARSER_ONLY
using ASTNodePtr = qi::unused_type;
#else
using ASTNodePtr = ASTNode*;
#endif

class Grammar : public qi::grammar<std::string::const_iterator, ASTNodePtr()>
{
public:
	using Iterator = std::string::const_iterator;

	Grammar();

	qi::rule<Iterator, ASTNodePtr()>
	tok_RE,
	tok_TL_elements, tok_TL_element, tok_TL_group, tok_TL_nongroup,
	tok_nested_elements, tok_nested_element, tok_nested_group, tok_nested_nongroup,
	tok_set;

	// no AST nodes required for these
	qi::rule<Iterator, std::string()> tok_positive_set, tok_negative_set, tok_set_items, tok_set_item, tok_range, tok_char;
};

} // RegexSplitter

#endif /* REGEX_SPLITTER_H_ */
