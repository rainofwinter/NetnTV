#pragma once
#ifdef MSVC
#include <direct.h>
#endif

std::string getFileNameWithoutDirectory(const std::string & absFileName);
std::string getDirectory(const std::string & absFileName);
std::string getFileExtension(const std::string & fileName);
bool checkFileExtension(const std::string & fileName, const std::string & extension);
std::string getFileTitle(const std::string & fileName);


/**
Example:
paths are treated as case insensitive

convertToRelativePath("c:/temp/orig", "C:\\temp/dog/cat.txt") should return
"../dog/cat.txt"

*/
std::string convertToRelativePath(
	const std::string & curDir, const std::string & absFilePath);

/**
Example:

getAbsFileName("c:/temp/orig", "../dog/cat.txt") should return
"c:/temp/dog/cat.txt"
*/
std::string getAbsFileName(
	const std::string & absDir, const std::string & relFileName);

std::string getCurDir();