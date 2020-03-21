#ifndef __STATEMENT_H__
#define __STATEMENT_H__

class code_element;
#include "config.h"

#include <cstdint>
#include <iostream>
#include <string>

#include "var/type.h"

//order of operations
//http://en.cppreference.com/w/cpp/language/operator_precedence

//1
//::	l2r

//2
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
throw
*/

//16
//,	comma	l2r

//17
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
	OPER_LVL17
};

#define VAR_SIZE_8_BITS 1
#define VAR_SIZE_16_BITS 2
#define VAR_SIZE_32_BITS 4
#define VAR_SIZE_64_BITS 8
#define VAR_SIZE_OTHER 16

#define VAR_SIGN_SIGNED 1
#define VAR_SIGN_UNSIGNED 2

/// Describes a source code statement.
/** Holds everything required to describe a statement as used in source code. A statement might be (bob = 5;), (random_function_call(bob);), or (double sandwhiches;) */
class statement
{
	public:
		statement(); ///< Creates a basic statement.
		statement(std::string in, std::size_t size); ///< Creates a statement called in, type void, size is the size
		virtual ~statement();
		std::size_t mysize() { return thesize; } ///< Returns the size of the statement
		friend std::ostream &operator<<(std::ostream &out, statement &o);
		oper_precedence get_p(); ///< Returns the precedence of the statement
		std::size_t get_size();
		std::string get_name();
	protected:
		virtual std::ostream &print(std::ostream &out);
		oper_precedence p;
	private:
		char isconst;	//is the datatype constant?
		char sign;
		type var_type;
		std::string name;
		int num_elements;	//for arrays of specific sizes
		address addr;	//the address of the variable
		char valid_address;	//not all variables have an address
		char thesize;
		std::size_t othersize;
};

#endif
