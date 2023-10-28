#include <string>

struct CustomObjectData
{
	std::string name {};
	std::string desc {};
	std::string format {};
	std::string object_string {};
	int object_id = 0;
	int views = 0;
	double rating = 0;
	
	int author_id = 0;
	std::string author_name {};
};