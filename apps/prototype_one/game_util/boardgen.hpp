#ifndef GAME_BOARDGEN_HPP
#define GAME_BOARDGEN_HPP

#include <iostream>
#include <fstream>

// Simple util to generate large json files for board prototypes
namespace boardgen 
{
	// Outputs count tabs to fileStream
	void writeTab(std::fstream& fileStream, int count);
	// Outputs a formatted json type to fileStream and ends the line with a comma newline
	void writeType(std::fstream& fileStream, std::string type);
	// Outputs an entire game_board_square to the fileStream of prototype proto with transform [x,y,z]
	void writeSquare(std::fstream& fileStream, std::string proto, int x, int y, int z);
	// Outputs the opening of a game board prototype
	void startBoard(std::fstream& fileStream);
	// Outputs the closing of a game board prototype
	void endBoard(std::fstream& fileStream);
	// Generates a checkered board with alternating squareProto and squareProto2 of size ((maxX * 2) + 1) by ((maxY * 2) + 1)
	void generateBoardJson(std::string boardFile, std::string squareProto, std::string squareProto2, int maxX, int maxY);
}

#endif
