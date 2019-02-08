#include <FileManager.h>
#include <iostream>


Path FileManager::PATH = InitPath();
Path FileManager::InitPath()
{
	Path currentPath = fs::current_path();
	Path srcPath;
	for( auto& it : currentPath)
	{
		srcPath /= it;
		if (it == "src")
			break;
	}
	return srcPath;
}

std::string FileManager::RelativePath(char const *relativePath)
{
	Path path = PATH;
	path /= relativePath;
	return path.string();
}