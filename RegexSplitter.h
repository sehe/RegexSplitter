/*
 * RegexSplitter.h
 *
 *  Created on: Jan 9, 2020
 *      Author: frank
 */

#ifndef REGEX_SPLITTER_H_
#define REGEX_SPLITTER_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <boost/spirit/include/qi.hpp>
namespace qi = boost::spirit::qi;

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>


namespace RegexSplitter {
class MyString;
}

std::ostream & operator<<(
		std::ostream & os,
		RegexSplitter::MyString const & val);

namespace RegexSplitter {


class MyString
{
public:
	typedef enum
	{
		UNDEFINED_c		= 0,
		BREAKABLE_c		= 1,
		UNBREAKABLE_c	= 2,
	} Type;

	MyString()
	: fStr(),
	  fType(UNDEFINED_c)
	{ }

	MyString (
			std::string const & str,
			Type const type)
	: fStr(str),
	  fType(type)
	{ }

	~MyString()
	{ }

	// other standard functions: compiler generated

	std::string const &
	GetStr(void) const
	{
		return fStr;
	}

	Type const &
	GetType(void) const
	{
		return fType;
	}

    static std::string const
	TypeStr(Type const & type);

private:
	std::string fStr;
	Type fType;
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
			Type const type);

	// BREAKABLE_c
	ASTNode(
			std::vector<char> & data,
			Type const type);

	// UNBREAKABLE_c
	template < class FUSION >
	ASTNode(
			FUSION & fusion,
			Type const type);

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

    static std::string const
	TypeStr(Type const & type);

    std::string const &
	GetString(void) const;

    virtual ~ASTNode();

private:
    MyString fString;
    std::vector<MyString> fStrings;
    Type fType;
    static std::map<Type, std::string> const sType2String;
};

using ASTNodePtr = ASTNode*;

class Grammar : public qi::grammar<std::string::const_iterator, ASTNode*>
{
public:
    using Iterator = std::string::const_iterator;

    Grammar();

    qi::rule<Iterator, ASTNode*>
    	tok_RE, tok_elements, tok_element, tok_group, tok_something;
};

}

#endif /* REGEX_SPLITTER_H_ */
