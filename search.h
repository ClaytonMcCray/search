#ifndef SEARCH
#define SEARCH

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <future>
#include <iostream>
#include <optional>
#include <vector>
#include <sstream>

using result_vector = std::vector<std::future<std::optional<std::string>>>;
namespace fs = std::filesystem;

template <typename Reader, typename Writer> class Search {

      private:
	Writer writer;
	const std::string search_key;
	bool no_binary = true;

	auto is_binary(const std::string &file_path) -> bool;
	auto search_file_for(const fs::path file_path) -> std::optional<std::string>;
	void directory_searcher(const fs::path dir_path, result_vector *results);

      public:
	Search(const std::string &&search_key);
	void search(const fs::path &dir_path);
};

template <typename Reader, typename Writer>
auto Search<Reader, Writer>::is_binary(const std::string &file_path) -> bool {
	auto f = Reader::stream(file_path, std::ios::binary);
	constexpr auto null_byte = 0x00;
	for (auto b = f.get(); b != EOF; b = f.get()) {
		if (b == null_byte) {
			return true;
		}
	}

	return false;
}

// returns std::nullopt on failure
template <typename Reader, typename Writer>
auto Search<Reader, Writer>::search_file_for(const fs::path file_path) -> std::optional<std::string> {

	if ((this->no_binary && this->is_binary(file_path)) || !fs::is_regular_file(file_path)) {
		return std::nullopt;
	}

	auto f = Reader::stream(file_path.string());
	if (f.fail()) {
		return std::nullopt;
	}

	std::string line;
	std::stringstream matches;
	for (int64_t line_number = 0; std::getline(f, line); line_number++) {
		if (line.find(this->search_key) == std::string::npos) {
			continue;
		}

		matches << file_path.string() << ":" << line_number << ":\t" << line << std::endl;
	}

	return std::optional<std::string>{matches.str()};
}

template <typename Reader, typename Writer>
void Search<Reader, Writer>::directory_searcher(const fs::path dir_path, result_vector *results) {

	for (auto contents : fs::directory_iterator(dir_path)) {
		if (std::filesystem::is_directory(contents.path())) {
			this->directory_searcher(contents.path(), results);
			continue;
		}

		results->push_back(
		    std::async(std::launch::async, [&]() { return this->search_file_for(contents.path()); }));

		// std::promise<std::optional<ec>> p;
		// results->push_back(p.get_future());
		// p.set_value(this->search_file_for(contents.path()));
	}
}


template <typename Reader, typename Writer> void Search<Reader, Writer>::search(const fs::path &dir_path) {

	if (!std::filesystem::is_directory(dir_path)) {
		this->search_file_for(dir_path);
	} else {
		auto results = result_vector{};
		this->directory_searcher(dir_path, &results);

		for (auto &future_content : results) {
			auto opt = future_content.get();
			if (opt == std::nullopt) {
				continue;
			}

			this->writer.write(*opt);
		}
	}
}

template <typename Reader, typename Writer>
Search<Reader, Writer>::Search(const std::string &&search_key) : search_key(search_key) {
	this->writer = writer;
}

#endif