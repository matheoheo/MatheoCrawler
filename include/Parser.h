#pragma once
class Parser
{
public:
	struct ParserNode
	{
		std::string category;
		std::unordered_map<std::string, std::string> data;
		std::vector<ParserNode> childrenNodes;
	};
	using ParseResult = std::vector<ParserNode>;
	ParseResult parseData(std::string_view input) const;
private:
	bool isStartOfCategory(std::string_view data) const;
	bool isEndOfCategory(std::string_view data) const;
	std::optional<std::string_view> getCategory(std::string_view data) const;
	std::string_view trimLeft(std::string_view data) const;
	std::pair<std::string_view, std::string_view> splitAtDelimiter(std::string_view data) const;
	ParserNode parseCategory(std::string_view currentCategory, auto& it, const auto& end) const;
	std::string removeSpaces(std::string_view data) const;
};

