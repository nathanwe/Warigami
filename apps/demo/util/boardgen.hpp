#ifndef GAME_BOARDGEN_HPP
#define GAME_BOARDGEN_HPP

#include <iostream>
#include <fstream>

namespace boardgen 
{
	void writeTab(std::fstream& fileStream, int count);
	void writeType(std::fstream& fileStream, std::string type);
	void writeSquare(std::fstream& fileStream, std::string proto, int x, int y, int z);
	void startBoard(std::fstream& fileStream);
	void endBoard(std::fstream& fileStream);
	void generateBoardJson(std::string boardFile, std::string squareProto, int maxX, int maxY);
}

#endif
