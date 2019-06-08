#include <string>
#include <sstream>
#include <vector>
#include <iterator>

namespace lib {

	template<typename T>
	void split(const std::string &s, char delim, T result) {
		std::stringstream ss;
		ss.str(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			*(result++) = item;
		}
	}
	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, std::back_inserter(elems));
		return elems;
	}
	
	std::vector<std::string> split(const std::string& text, const std::string& delims)
	{
		std::vector<std::string> tokens;
		std::size_t start = text.find_first_not_of(delims), end = 0;

		while ((end = text.find_first_of(delims, start)) != std::string::npos)
		{
			tokens.push_back(text.substr(start, end - start));
			start = text.find_first_not_of(delims, end);
		}
		if (start != std::string::npos)
			tokens.push_back(text.substr(start));

		return tokens;
	}

	bool	isdigit(char c) {
		return (c < '0' || c > '9')?false:true;
	}
	
	template<class T>
	void tokenize(const std::string& str, T& tokens, const std::string& delimiters, bool trimEmpty)
	{
		std::string::size_type pos, lastPos = 0, length = str.length();

		using value_type = typename T::value_type;
		using size_type  = typename T::size_type;

		while (lastPos < length + 1)
		{
			pos = str.find_first_of(delimiters, lastPos);
			if (pos == std::string::npos)
			{
				pos = length;
			}

			if (pos != lastPos || !trimEmpty)
				tokens.push_back(value_type(str.data()+lastPos,
					(size_type)pos-lastPos));

			lastPos = pos + 1;
		}
	}

}