#include <FileManager.h>
#include <filesystem>

using Path = std::experimental::filesystem::path;
namespace fs = std::experimental::filesystem;

std::string FileManager::RelativePath(char const *relativePath)
{
	Path path = fs::current_path();
	//std::string path = 
	path /= relativePath;
	return path.string();
}