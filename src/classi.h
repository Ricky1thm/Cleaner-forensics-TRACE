#include <string>

using namespace std;

struct body {
	string memory;
};

size_t WriteCallBack(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t total_size = size * nmemb;
	((body*)userp)->memory.append((char*)contents, total_size);
	return total_size;
}

class menu {
public:
	int scelta;
	int cast;
};