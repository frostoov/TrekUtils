#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <string>
#include <locale>

class AbstractConfigParser {
  public:
	using siterator = std::string::const_iterator;
  public:
	virtual ~AbstractConfigParser() {}
	virtual void load(const std::string& fileName) = 0;
	virtual void save(const std::string& fileName) = 0;
  protected:
	AbstractConfigParser() {}
	template<typename iterator>
	iterator getToken(iterator iter, const iterator& end, std::string& token) __attribute__((noinline)) {
		token.clear();
		if(iter == end)
			return end;
		while(isblank(*iter) && iter != end)
			++iter;
		if(isdigit(*iter) || *iter == '-') {
			auto dotFlag = false;
			do {
				if(*iter == point)
					dotFlag = true;
				token.push_back(*iter);
				++iter;
			} while( isdigit( *iter ) || (*iter == point && !dotFlag ) );
			return iter;
		} else if(isalpha(*iter)) {
			while( isalnum(*iter) || *iter == '_') {
				token.push_back(*iter);
				++iter;
			}
			return iter;
		} else if(*iter == '=' || *iter == '#' || *iter == '.' || *iter == ':') {
			token.push_back(*iter);
			return ++iter;
		} else
			return end;
	}

  private:
	const int point = std::use_facet<std::numpunct<char>>(std::locale("")).decimal_point();
};

#endif // CONFIG_PARSER_H
