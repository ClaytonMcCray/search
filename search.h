#ifndef SEARCH
#define SEARCH

#include <filesystem>
#include <future>
#include <optional>
#include <string_view>
#include <vector>

enum class [[nodiscard]] ec{not_regular, stream_fail};
using result_vector = std::vector<std::future<std::optional<ec>>>;
namespace fs = std::filesystem;

template <typename Reader, typename Writer> 
class Search {

      private:
	Writer writer;
	Reader reader;
	const std::string search_key;
	bool no_binary = true;

	auto is_binary(const std::string &file_path) -> bool;
	auto search_file_for(const fs::path file_path) -> std::optional<ec>;
	void directory_searcher(const fs::path dir_path, result_vector *results);

      public:
	Search(Reader reader, Writer writer, const std::string &&search_key);
	void search(const fs::path &dir_path);
};

#endif
