#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <iostream>
#include <string>

void begin_line(std::ostream &b, int a);
std::string hstring(unsigned int a);

/// A helper class for reading hexadecimal numbers from an input stream
/** Used to read hex numbers from an input stream. Spaces at the start are consumed and discarded. An optional negative symbol in front of the number is supprted. A single 0 followed by anything  besides x is interpreted as 0. Hex characters are intrepreted normally. */
class hex
{
	public:
		hex(int &result); ///< Create an object with a reference to the result. The captured integer is stored there.
		friend std::istream &operator>>(std::istream &in, hex a); ///< Read the actual number from the specified input stream.
	private:
		int &r; ///<A reference to the output
};

/// A class used for storing a mask and producing a match. 
/** A class used to get all characters specified in a match string or all characters not in a match string. Capture stops at the first non-captured character. Only matching characters are consumed from the input. */
class scanset
{
	public:
		scanset(std::string mask, std::string &result); ///< Construct a scanset with the specified mask, to output the matched data to result
		scanset(const char *mask, std::string &result); ///< Construct a scanset with the specified mask, to output the matched data to result
		friend std::istream &operator>>(std::istream &in, scanset a); ///< Read all data specified by the mask from the given stream and scanset.
	private:
		std::string m; ///< The mask containing either a set of characters to match, or a set of all nonmatching character (the first character is ^ for non-matching.
		std::string &r; ///< A reference to the string to output matching characters to
};

/// Helper class for outputting multiple tabs to an output stream.
/** Convenience class for creating multiple tabs in an output stream. Currently always outputs the specified number of tabs. Future functionality includes outputting the specified number of (spaces) or (2 spaces) or (n spaces). */
class tabs
{
	public:
		tabs(int howmany);
		friend std::ostream &operator<<(std::ostream&out, tabs a);
	private:
		int hm;
};

#endif
