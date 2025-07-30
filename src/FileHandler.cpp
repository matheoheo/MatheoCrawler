#include "pch.h"
#include "FileHandler.h"

std::string FileHandler::readFileContent(std::string_view path) const
{
    std::ifstream file(path);
    if (!file)
        return "";

    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}
