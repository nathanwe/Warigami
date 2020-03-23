//
// Created by sava on 2020-02-28.
//

#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

using nlohmann::json;


void do_boardsquares(json& children, int num_lanes, int num_columns)
{    
    for (int row = 0; row < num_lanes; row++)
    {
        std::string prototype = row % 2 == 0
            ? "assets/prototypes/boardsquare.json"
            : "assets/prototypes/boardsquare2.json";

        for (int col = 0; col < num_columns; col++)
        {
            json tile;
            tile["prototype"] = prototype;

            json transform;
            transform["type"] = "transform";
            transform["position"] = { row - num_lanes / 2, 1, col - num_columns / 2 };

            json board_square;
            board_square["type"] = "board_square";
            board_square["x"] = row;
            board_square["y"] = col;

            tile["components"] = {transform, board_square};

            children.push_back(tile);
        }
    }
}


int main()
{
    json all;

    all["root"]["components"] = {
            {
                    {"type", "transform"},
                    {"position", {10, 0, 10}},
                    {"scale", {1, 1, 1}},
            },
            {
                    {"type", "board"}
            }
    };

    all["children"] = {};

    do_boardsquares(all["children"], 5, 9);


    std::ofstream o("gameboard.json");
    o << all << std::endl;
    return 0;
}