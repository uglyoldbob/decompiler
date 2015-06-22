#ifndef __CODE_MULTI_IF_H__
#define __CODE_MULTI_IF_H__

#include <iostream>
#include <vector>
#include "code_element.h"

enum code_multi_type
{
	NONE,	//unitialized
	WITH_ELSE,	//conditionals could be ANDed or ORed together
	OR_NO_ELSE,
	AND_NO_ELSE
};

class code_multi_if : public code_element
{
	public:
		code_multi_if();
		~code_multi_if();
		void add(code_element *a);
		void common(code_element *c);
		void set_else(code_element *e);
		void set_final(code_element *f);
		void finish_and_no_else();
		void finish_or_no_else();
		void finish_with_else();
		void fprint(std::ostream &dest, int depth);
#ifdef PROVE_SIMPLIFY
		virtual void print_graph(std::ostream &dest);
#endif
	private:
		code_multi_type form;	//what form of the structure is this?
		std::vector<code_element*> ifs;	//the list if conditions that could lead to a common block
		code_element *common_block;
		code_element *helse;
		code_element *thefinal;
};

#endif