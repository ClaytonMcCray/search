#ifndef FILESYSTEM_ITERABLE_H
#define FILESYSTEM_ITERABLE_H

#include <filesystem>
#include <string>

class FilesystemIterable {
      public:
	static auto iterable(const std::string &directory) { return std::filesystem::directory_iterator(directory); }
};

#endif