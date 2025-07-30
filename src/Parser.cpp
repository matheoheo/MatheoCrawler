#include "pch.h"
#include "Parser.h"
#include "Config.h"

Parser::ParseResult Parser::parseData(std::string_view input) const
{
    ParseResult result;
    auto lines = input | std::views::split('\n');
    auto it = std::ranges::begin(lines);
    auto end = std::ranges::end(lines);

    while (it != end)
    {
        std::string_view lineView = trimLeft(std::string_view(std::ranges::begin(*it), std::ranges::end(*it)));
        if (lineView.empty())
        {
            ++it;
            continue;
        }
        
        if (isStartOfCategory(lineView))
        {
            auto category = getCategory(lineView);
            if (category.has_value())
            {
                ++it;
                auto node = parseCategory(category.value(), it, end);
                result.push_back(std::move(node));
                continue;
            }
        }
        ++it;
    }
    return result;
}

bool Parser::isStartOfCategory(std::string_view data) const
{
    return data.starts_with("[") && !data.starts_with("[/");
}

bool Parser::isEndOfCategory(std::string_view data) const
{
    return data.starts_with("[/");
}

std::optional<std::string_view> Parser::getCategory(std::string_view data) const
{
    if (!data.starts_with("["))
        return {};

    const size_t start = data.find_first_not_of("[ /");
    if (start == std::string_view::npos)
        return {};

    const size_t end = data.find_first_of("]");
    if (end == std::string_view::npos || start >= end) return {};

    return data.substr(start, end - start);
}

std::string_view Parser::trimLeft(std::string_view data) const
{
    auto it = std::ranges::find_if(data, [](unsigned char c)
        {
            return !std::isspace(c);
        });

    return data.substr(std::distance(data.begin(), it));
}

std::pair<std::string_view, std::string_view> Parser::splitAtDelimiter(std::string_view data) const
{
    auto splitPoint = data.find(Config::delimiter);
    if (splitPoint == std::string_view::npos)
        return { data, std::string_view{} };

    return { data.substr(0, splitPoint), data.substr(splitPoint + 1) };
}

Parser::ParserNode Parser::parseCategory(std::string_view currentCategory, auto& it, const auto& end) const
{
    ParserNode node;
    node.category = std::string(currentCategory);

    for (; it != end; ++it)
    {
        std::string_view lineView = trimLeft(std::string_view(std::ranges::begin(*it), std::ranges::end(*it)));
        if (lineView.empty())
            continue;

        if (isStartOfCategory(lineView))
        {
            auto childCategory = getCategory(lineView);
            if (childCategory)
            {
                ++it;
                auto childNode = parseCategory(childCategory.value(), it, end);
                node.childrenNodes.push_back(std::move(childNode));
                continue;
            }
        }
        else if (isEndOfCategory(lineView))
        {
            auto category = getCategory(lineView);
            if (category && category.value() == currentCategory)
                return node;
        }
        auto [p1, p2] = splitAtDelimiter(lineView);
        
        if (!p1.empty() && !p2.empty())
        {
            auto key = removeSpaces(p1);
            auto value = removeSpaces(p2);
            node.data.emplace(key, value);
        }
    }
    return node;
}

std::string Parser::removeSpaces(std::string_view data) const
{
    std::string result(data);
    result.erase(std::remove_if(std::begin(result), std::end(result), ::isspace), std::end(result));
    return result;
}
