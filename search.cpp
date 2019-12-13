#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdint>


auto is_binary(const std::string &file_path) -> bool {
	auto f = std::ifstream{file_path, std::ios::binary};
	for (auto b = f.get(); b != EOF; b = f.get()) {
		if (b == 0x00) {
			return true;
		}
	}

	return false;
}


void search_file_for(const std::string file_path, const std::string &search_key, bool reg=true) {
	if (reg && (!std::filesystem::is_regular_file(file_path) || is_binary(file_path))) {
		return;
	}

	auto f = std::ifstream{file_path};
	std::string line;
	for (int64_t line_number = 0; std::getline(f, line); line_number++) {
		if (line.find(search_key) == std::string::npos) {
			continue;
		}
		
		std::cout <<  file_path << ":" << line_number << ":\t" << line << std::endl;
	}
}


void directory_searcher(const std::string dir_path, const std::string &search_key) {
	for (auto contents : std::filesystem::directory_iterator(dir_path)) {
		if (std::filesystem::is_directory(contents)) {
			directory_searcher(contents.path().string(), search_key);
		}

		search_file_for(contents.path().string(), search_key);
	}
}


int main(int argc, char **argv) {

	if (argc != 3) {
		return 1;
	}

	std::string file_path = argv[1];
	std::string search_key = argv[2];

	if (!std::filesystem::exists(file_path)) {
		return 1;
	}

	if (!std::filesystem::is_directory(argv[1])) {
		search_file_for(file_path, search_key);
	} else {
		directory_searcher(file_path, search_key);
	}
}
