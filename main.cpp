#include "pch.h"
#include "Engine.h"
#include "DungeonGenerator.h"
#include "FileHandler.h"
#include "Parser.h"

void printParseResult(const Parser::ParseResult& result, int indent = 0);
void printNode(const Parser::ParserNode& node, int indent);

int main()
{
	Engine engine;
	engine.start();
}

void printNode(const Parser::ParserNode& node, int indent)
{
    std::string indentation(indent, ' ');

    std::cout << indentation << "[" << node.category << "]\n";

    for (const auto& [key, value] : node.data)
    {
        std::cout << indentation << "  " << key << " = " << value << "\n";
    }

    for (const auto& child : node.childrenNodes)
    {
        printNode(child, indent + 2); // increase indentation for nesting
    }

    std::cout << indentation << "[/" << node.category << "]\n";
}

void printParseResult(const Parser::ParseResult& result, int indent)
{
    for (const auto& node : result)
    {
        printNode(node, indent);
    }
}