#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <optional>
#include <future>
#include <vector>
#include <algorithm>

enum class [[nodiscard]] ec {not_regular, stream_fail};

auto is_binary(const std::string &file_path) -> bool {
	auto f = std::ifstream{file_path, std::ios::binary};
	constexpr auto null_byte = 0x00;
	for (auto b = f.get(); b != EOF; b = f.get()) {
		if (b == null_byte) {
			return true;
		}
	}

	return false;
}


// returns std::nullopt on success
auto search_file_for(const std::filesystem::path file_path, const std::string &search_key, 
		bool no_binary=true) -> std::optional<ec> {
	if ((no_binary && is_binary(file_path)) || !std::filesystem::is_regular_file(file_path)) {
		return std::optional<ec>{ec::not_regular};
	}

	auto f = std::ifstream{file_path};
	if (f.fail()) {
		return std::optional<ec>{ec::stream_fail};
	}

	std::string line;
	for (int64_t line_number = 0; std::getline(f, line); line_number++) {
		if (line.find(search_key) == std::string::npos) {
			continue;
		}

		std::cout <<  file_path.string() << ":" << line_number << ":\t" << line << std::endl;
	}

	return std::nullopt;
}


void directory_searcher(const std::filesystem::path dir_path, const std::string &search_phrase,
		std::vector<std::future<std::optional<ec>>> *results) {
	for (auto contents : std::filesystem::directory_iterator(dir_path)) {
		if (std::filesystem::is_directory(contents.path())) {
			directory_searcher(contents.path(), search_phrase, results);
			continue;
		}


		std::promise<std::optional<ec>> p;
		p.set_value(search_file_for(contents.path(), search_phrase));
		results->push_back(p.get_future());
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

	if (!std::filesystem::is_directory(file_path)) {
		search_file_for(file_path, search_key);
	} else {
		auto error_results = std::vector<std::future<std::optional<ec>>>{};
		directory_searcher(file_path, search_key, &error_results);
		std::for_each(std::begin(error_results), std::end(error_results), 
			[](auto &err) { 
				err.wait();
				auto rc = err.get();
				if (rc != std::nullopt) {
					std::cout << "failure with " << (*rc == ec::not_regular ? "ec::not_regular" : "ec::stream_fail") << std::endl;
				} 
			});
	}
}
