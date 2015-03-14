#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "config.h"

#include <cstdint>
#include <iostream>
#include <string>

//order of operations
//http://en.cppreference.com/w/cpp/language/operator_precedence

//1
//::	l2r

//1
/* these are left 2 right
++ -- (suffix)
()	function call
[]
.
->
*/

//3
/*	these are r2l
++ -- (prefix)
+ - (unary positive or negative)
! ~
(type)	a type cast
* (dereference)
&	address of
sizeof
new, new[]
delete, delete[]
*/

//4
//.* ->* pointer to member l2r

//5
//* / %		l2r

//6
//+ -	add/subtract	l2r

//7
//<< >>	bit shifting	l2r

//8
// < <= > >=	l2r

//9
//== != l2r

//10
//& bitwise and	l2r

//11
//^ bitwise xor l2r

//12
//| bitwise or	l2r

//13
//&& l2r

//14
//|| l2r

//15
/* these are r2l
?:
=
+= -=
*= /= %= <<= >>= &= ^= |=
*/

//16
//throw	r2l

//17
//,	comma	l2r

//18
//a plain variable

enum oper_precedence
{	//lower means more precedence - like golf scores
	OPER_LVL0,
	OPER_LVL1,
	OPER_LVL2,
	OPER_LVL3,
	OPER_LVL4,
	OPER_LVL5,
	OPER_LVL6,
	OPER_LVL7,
	OPER_LVL8,
	OPER_LVL9,
	OPER_LVL10,
	OPER_LVL11,
	OPER_LVL12,
	OPER_LVL13,
	OPER_LVL14,
	OPER_LVL15,
	OPER_LVL16,
	OPER_LVL17,
	OPER_LVL18
};

class variable
{
	public:
		variable();
		variable(std::string &in);
		virtual ~variable();
		std::size_t mysize() { return thesize; }
		friend std::ostream &operator<<(std::ostream &out, variable &o);
		oper_precedence get_p();
	protected:
		virtual std::ostream &print(std::ostream &out);
		oper_precedence p;
	private:
		char isconst;	//is the datatype constant?
		char sign;	//is the datatype signed (-1), unsigned (1), or irrelevant (0)
		std::string type;
		std::string name;
		int num_elements;	//for arrays of specific sizes
		address addr;	//the address of the variable
		char valid_address;	//not all variables have an address
		std::size_t thesize;
};

#endif
