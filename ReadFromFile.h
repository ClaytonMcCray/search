#ifndef READ_FROM_FILE_H
#define READ_FROM_FILE_H

#include <fstream>
#include <filesystem>


class ReadFromFile {
	public:
	static std::ifstream stream(const std::filesystem::path &file_path) {
		return std::ifstream{file_path};
	}


	static std::ifstream stream(const std::filesystem::path &file_path, std::ios::openmode m) {
		return std::ifstream{file_path, m};
	}
};

#endif