#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <cctype>
#include <map>
#include <regex>

struct game_t{
    int id;
    std::vector<int> red;
    std::vector<int> green;
    std::vector<int> blue;
};

using games_t = std::vector<game_t>;

games_t load_input(const std::string& file){
    games_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        game_t game;
        sscanf_s(line.c_str(), "Game %d\n", &game.id); 
        std::string str = line.substr(line.find(":")+1);
        std::regex word_regex("[0-9]+.\\w");
        for(auto it=std::sregex_iterator(str.begin(), str.end(), word_regex); it!=std::sregex_iterator(); it++){
            int num_blocks; char block_color;
            sscanf_s(it->str().c_str(), "%d %c\n", &num_blocks, &block_color, 1);  
            if(block_color == 'r'){
                game.red.push_back(num_blocks);
            }else if(block_color == 'g'){
                game.green.push_back(num_blocks);
            }else if(block_color == 'b'){
                game.blue.push_back(num_blocks);
            }
        }
        ret.push_back(game);
    }
    return ret;
}

int part1(const games_t& games)
{
    auto test_color = [](auto& colors, int max_color){
        return std::all_of(colors.begin(), colors.end(), [&](auto& num){ return num <= max_color; });
    };

    int sum = 0;
    for(auto& game : games){
        if(test_color(game.red, 12) && test_color(game.green, 13) && test_color(game.blue, 14)){
            sum += game.id;
        }
    }

    return sum;
}

int part2(const games_t& games)
{
    auto max_color = [](auto& colors){
        return *std::max_element(colors.begin(), colors.end());
    };

    int sum = 0;
    for(auto& game : games){
        sum += max_color(game.red) * max_color(game.green) * max_color(game.blue);
    }

    return sum;
}

void main()
{
    auto test_values = load_input("../src/day02/test_input.txt");
    auto actual_values = load_input("../src/day02/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}