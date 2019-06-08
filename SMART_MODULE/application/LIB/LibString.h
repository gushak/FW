#ifndef __LIBSTRING_H__
#define __LIBSTRING_H__
#include <string>
#include <vector>

namespace lib {

	template<typename T> void split(const std::string &s, char delim, T result);
	std::vector<std::string> split(const std::string &s, char delim);
	std::vector<std::string> split(const std::string& text, const std::string& delims);
	bool	isdigit(char);
	template<class T> void tokenize(const std::string& str, T& tokens, const std::string& delimiters = " ", bool trimEmpty = false);
}
#endif	/* __LIBSTRING_H__ */
