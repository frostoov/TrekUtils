#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <string>

class AbstractConfigParser {
  public:
	virtual ~AbstractConfigParser() {}
	virtual void load(const std::string& fileName) = 0;
	virtual void save(const std::string& fileName) = 0;
  protected:
	AbstractConfigParser() {}

  private:
};

#endif // CONFIG_PARSER_H
