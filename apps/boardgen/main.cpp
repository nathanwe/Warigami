//
// Created by sava on 2020-02-28.
//

#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

using nlohmann::json;


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

    for (int i = 0; i < 6; ++i )
    {
        for (int j = 0; j < 8; ++j)
        {
            std::string boardsquare_proto = "invalid";

            if (i % 2 != 0)
                if (j % 2 == 0)
                    boardsquare_proto = "assets/prototypes/boardsquare.json";
                else
                    boardsquare_proto = "assets/prototypes/boardsquare2.json";
            else if (j % 2 != 0)
                boardsquare_proto = "assets/prototypes/boardsquare.json";
            else
                boardsquare_proto = "assets/prototypes/boardsquare2.json";


            all["children"][i + j] = {
                {"prototype",  boardsquare_proto},
                {"components", {
                   {
                       {"type", "transform"},
                       {"position", {i - 3, 1, j - 4}}
                   },
                   {
                       {"type", "board_square"},
                       {"x", i},
                       {"y", j}
                   }
               }},
            };
        }
    }

    std::ofstream o("gameboard.json");
    o << all << std::endl;
    return 0;
}