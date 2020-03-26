#include "boardgen.hpp"

namespace boardgen {

	void writeTab(std::fstream& fileStream, int count)
	{
		for (int i = 0; i < count; i++)
		{
			fileStream << "\t";
		}
	}

	void writeType(std::fstream& fileStream, std::string type) {
		fileStream << "\"type\": \"";
		fileStream << type;
		fileStream << "\",\n";
	}

	void writeSquare(std::fstream& fileStream, std::string proto, int x, int y, int z, int maxX, int maxY)
	{
		writeTab(fileStream, 2);
		fileStream << "{\n";
		writeTab(fileStream, 3);
		fileStream << "\"prototype\": \"";
		fileStream << proto;
		fileStream << "\",\n";
		writeTab(fileStream, 3);
		fileStream << "\"components\": [\n";
		writeTab(fileStream, 4);
		fileStream << "{\n";
		writeTab(fileStream, 5);
		writeType(fileStream, "transform");
		writeTab(fileStream, 5);
		fileStream << "\"position\": [";
		fileStream << x << ", " << y << ", " << z << " ]\n";
		writeTab(fileStream, 4);
		fileStream << "},\n";
		writeTab(fileStream, 4);
		fileStream << "{\n";
		writeTab(fileStream, 5);
		writeType(fileStream, "board_square");
		writeTab(fileStream, 5);
		fileStream << "\"x\": " << x + maxX << ",\n";
		writeTab(fileStream, 5);
		fileStream << "\"y\": " << z + maxY << "\n";
		writeTab(fileStream, 4);
		fileStream << "}\n";
		writeTab(fileStream, 3);
		fileStream << "]\n";
		writeTab(fileStream, 2);
		fileStream << "},\n";
	}

	void startBoard(std::fstream& fileStream)
	{
		fileStream << "{\n";
		writeTab(fileStream, 1);
		fileStream << "\"root\": {\n";
		writeTab(fileStream, 2);
		fileStream << "\"components\": [\n";
		writeTab(fileStream, 3);
		fileStream << "{\n";
		writeTab(fileStream, 4);
		writeType(fileStream, "transform");
		writeTab(fileStream, 4);
		fileStream << "\"position\": [ 10, 0, 10 ],\n";
		writeTab(fileStream, 4);
		fileStream << "\"scale\": [ 1, 1, 1 ]\n";
		writeTab(fileStream, 3);
		fileStream << "},\n";
		writeTab(fileStream, 3);
		fileStream << "{\n";
		writeTab(fileStream, 4);
		writeType(fileStream, "board");
		writeTab(fileStream, 3);
		fileStream << "}\n";
		writeTab(fileStream, 2);
		fileStream << "]\n";
		writeTab(fileStream, 1);
		fileStream << "},\n";
		writeTab(fileStream, 1);
		fileStream << "\"children\": [\n";
	}

	void endBoard(std::fstream& fileStream) {
		writeTab(fileStream, 1);
		fileStream << "]\n";
		fileStream << "}";
	}

	void generateBoardJson(std::string boardFile, std::string squareProto, std::string squareProto2, int maxX, int maxY)
	{
		std::fstream f;
		f.open(boardFile, std::fstream::out | std::fstream::trunc);
		if (f.is_open()) {
			std::cerr << "file opened to write" << std::endl;
		}
		startBoard(f);
		for (int i = -maxX; i <= maxX; i++) {
			for (int j = maxY; j >= -maxY; j--) {
				if (i % 2 != 0)
                    writeSquare(f, squareProto, i, 1, j, maxX, maxY);
                else
                    writeSquare(f, squareProto2, i, 1, j, maxX, maxY);
			}
		}
		endBoard(f);
		f.close();
		if (!f.is_open()) {
			std::cerr << "file closed" << std::endl;
		}
	}
}