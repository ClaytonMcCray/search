#ifndef SEARCH
#define SEARCH

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <future>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

enum class [[nodiscard]] ec{not_regular, stream_fail};
using result_vector = std::vector<std::future<std::optional<ec>>>;
namespace fs = std::filesystem;

template <typename Reader, typename Writer> class Search {

      private:
	Writer writer;
	const std::string search_key;
	bool no_binary = true;

	auto is_binary(const std::string &file_path) -> bool;
	auto search_file_for(const fs::path file_path) -> std::optional<ec>;
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

// returns std::nullopt on success
template <typename Reader, typename Writer>
auto Search<Reader, Writer>::search_file_for(const fs::path file_path) -> std::optional<ec> {

	if ((this->no_binary && this->is_binary(file_path)) || !fs::is_regular_file(file_path)) {
		return std::optional<ec>{ec::not_regular};
	}

	auto f = Reader::stream(file_path.string());
	if (f.fail()) {
		return std::optional<ec>{ec::stream_fail};
	}

	std::string line;
	for (int64_t line_number = 0; std::getline(f, line); line_number++) {
		if (line.find(this->search_key) == std::string::npos) {
			continue;
		}

		this->writer.write(file_path.string(), ":", line_number, ":\t", line, "\n");
	}

	return std::nullopt;
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

// int main(int argc, char **argv) {
template <typename Reader, typename Writer> void Search<Reader, Writer>::search(const fs::path &dir_path) {

	if (!std::filesystem::is_directory(dir_path)) {
		this->search_file_for(dir_path);
	} else {
		auto error_results = result_vector{};
		this->directory_searcher(dir_path, &error_results);

		for (auto &err_future : error_results) {
			err_future.get(); // not handling any errors
		}
	}
}

template <typename Reader, typename Writer>
Search<Reader, Writer>::Search(const std::string &&search_key) : search_key(search_key) {
	this->writer = writer;
}

#endif