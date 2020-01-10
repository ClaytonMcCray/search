#include "ReadFromFile.h"
#include "WriteToStdout.h"
#include "search.h"

#include <string>
#include <utility>

int main(int argc, char **argv) {
	if (argc != 3) {
		return 1;
	}

	const std::string &file_path = argv[1];
	const std::string &search_key = argv[2];

	if (!std::filesystem::exists(file_path)) {
		return 1;
	}

	auto controller = SearchBuilder<ReadFromFile, WriteToStdout>{std::move(search_key)}.build();
	controller.search(file_path);
}