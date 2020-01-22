#ifndef FILESYSTEM_ITERABLE_H
#define FILESYSTEM_ITERABLE_H

#include <filesystem>
#include <string>

class FilesystemIterable {
      public:
	auto iterable(const std::string &directory) const { return std::filesystem::directory_iterator(directory); }
};

#endif