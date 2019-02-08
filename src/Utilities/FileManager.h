#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <string>
#include <filesystem>

namespace fs = std::experimental::filesystem;
using Path = fs::path;


class FileManager
{
	
	static Path InitPath();
public:
	static Path PATH;
	static std::string RelativePath(char const *);
};

#endif