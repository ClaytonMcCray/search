#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <optional>
#include <vector>

#include "WriteToStdout.h"

enum class [[nodiscard]] ec{not_regular, stream_fail};

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
template <typename Writer>
auto search_file_for(const std::filesystem::path file_path, const std::string &search_key, bool no_binary,
		     const Writer writer) -> std::optional<ec> {

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

		writer.write(file_path.string(), ":", line_number, ":\t", line, "\n");
	}

	return std::nullopt;
}

template <typename Writer>
void directory_searcher(const std::filesystem::path dir_path, const std::string &search_phrase,
			std::vector<std::future<std::optional<ec>>> *results, const Writer &writer) {

	for (auto contents : std::filesystem::directory_iterator(dir_path)) {
		if (std::filesystem::is_directory(contents.path())) {
			directory_searcher(contents.path(), search_phrase, results, writer);
			continue;
		}

		results->push_back(std::async(std::launch::async, [&]() {
			return search_file_for(contents.path(), search_phrase, true, writer);
		}));
	}
}

int main(int argc, char **argv) {

	if (argc != 3) {
		return 1;
	}

	const std::string &file_path = argv[1];
	const std::string &search_key = argv[2];

	if (!std::filesystem::exists(file_path)) {
		return 1;
	}

	auto writer = WriteToStdout{};

	if (!std::filesystem::is_directory(file_path)) {
		search_file_for(file_path, search_key, true, writer);
	} else {
		auto error_results = std::vector<std::future<std::optional<ec>>>{};
		directory_searcher(file_path, search_key, &error_results, writer);

		for (auto &err_future : error_results) {
			err_future.get(); // not handling any errors
		}
	}
}
