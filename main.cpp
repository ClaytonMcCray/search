#include <string>
#include <utility>

#include "search.h"
#include "WriteToStdout.h"


int main(int argc, char **argv) {
	if (argc != 3) {
		return 1;
	}

	const std::string &file_path = argv[1];
	const std::string &search_key = argv[2];

	if (!std::filesystem::exists(file_path)) {
		return 1;
	}

	auto controller = Search{std::string{"dumbo"}, WriteToStdout{}, std::move(search_key)};
	controller.search(file_path);
}