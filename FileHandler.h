#pragma once
#include "ConceptDefinitions.h"

class FileHandler
{
public:
	std::string readFileContent(std::string_view path) const;

	template <Streamable... Args>
	void saveToFile(std::ofstream& file, std::string_view category, Args&&... args);

	template <Streamable... Args>
	void createAndSaveToFile(std::string_view filePath, std::string_view category, Args&&... args);
private:
};

template<Streamable ...Args>
inline void FileHandler::saveToFile(std::ofstream& file, std::string_view category, Args && ...args)
{
	if (!file.is_open())
		return;
	file << "[" << category << "]\n";
	((file << args << '\n'), ...);
	file << "[/" << category << "]\n";
}

template<Streamable ...Args>
inline void FileHandler::createAndSaveToFile(std::string_view filePath, std::string_view category, Args && ...args)
{
	std::ofstream file(filePath, std::ios::out);
	saveToFile(file, category, std::forward<Args>(args)...);
}
