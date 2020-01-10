#ifndef SEARCH
#define SEARCH

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <future>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

using result_vector = std::vector<std::future<std::optional<std::string>>>;
namespace fs = std::filesystem;

template <typename Reader, typename Writer>
class SearchBuilder;

template <typename Reader, typename Writer>
class Search {

      public:
	friend class SearchBuilder<Reader, Writer>;
	void search(const fs::path &dir_path);

      private:
	Writer writer;
	const std::string search_key;
	bool no_binary = true;

	Search(const std::string &&search_key);
	auto is_binary(const fs::path &file_path) -> bool;
	auto search_file(const fs::path file_path) -> std::optional<std::string>;
	void directory_searcher(const fs::path dir_path, result_vector *results);
};

template <typename Reader, typename Writer>
auto Search<Reader, Writer>::is_binary(const fs::path &file_path) -> bool {
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
auto Search<Reader, Writer>::search_file(const fs::path file_path) -> std::optional<std::string> {

	if ((this->no_binary && this->is_binary(file_path)) || !fs::is_regular_file(file_path)) {
		return std::nullopt;
	}

	auto f = Reader::stream(file_path);
	if (f.fail()) {
		return std::nullopt;
	}

	std::string line;
	std::stringstream matches;
	for (int64_t line_number = 1; std::getline(f, line); line_number++) {
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
		    std::async(std::launch::async, [=]() { return this->search_file(contents.path()); }));
	}
}

template <typename Reader, typename Writer>
void Search<Reader, Writer>::search(const fs::path &dir_path) {

	if (!std::filesystem::is_directory(dir_path)) {
		auto hits = this->search_file(dir_path);
		if (hits != std::nullopt) {
			this->writer.write(*hits);
		}
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
Search<Reader, Writer>::Search(const std::string &&search_key) : search_key(search_key) {}

template <typename Reader, typename Writer>
class SearchBuilder {
      private:
	bool no_binary = true;
	Search<Reader, Writer> searcher;

      public:
	SearchBuilder(const std::string &&search_key) : searcher(std::move(search_key)) {}
	SearchBuilder &search_binary_files() {
		this->searcher.no_binary = false;
		return *this;
	}

	Search<Reader, Writer> build() { return this->searcher; }
};

#endif