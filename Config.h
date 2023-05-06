#include <string>

class Config {
public:
	std::string log_file;
	std::string errlog_file;
	void load_config(std::string path);
};
