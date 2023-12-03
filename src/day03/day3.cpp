#include <vector>
#include <iostream>
#include <fstream>
#include <regex>

struct pos_t{
    int x, y;
};

struct part_t {
    pos_t pos;
    std::string name;
}; 

struct schematic_t{
    std::vector<part_t> numbers;
    std::vector<part_t> symbols;
};

bool is_number(const std::string& str) {
  return std::all_of(str.begin(), str.end(), ::isdigit);
}

schematic_t load_input(const std::string& file){
    schematic_t ret;
    std::ifstream fs(file);
    std::string line;
    int y = 0;
    while (std::getline(fs, line)) {
        std::regex word_regex("[\\w]+|[^.]");
        for(auto it=std::sregex_iterator(line.begin(), line.end(), word_regex); it!=std::sregex_iterator(); it++){
            if(is_number(it->str())){
                ret.numbers.push_back({{(int)it->position(),y}, it->str()});
            }else{
                ret.symbols.push_back({{(int)it->position(),y}, it->str()});
            }
        }
        y++;
    }
    return ret;
}

bool near(const part_t& number, const part_t& symbol){
    for(int i=0; i<number.name.size(); ++i){
        int xdist = abs(symbol.pos.x - (number.pos.x + i));
        int ydist = abs(symbol.pos.y - number.pos.y);
        if((xdist==1 && ydist==1) || (xdist==0 && ydist==1) || (xdist==1 && ydist==0)){
            return true; // diagonal or vertical or horizontal 
        }
    }
    return false;
}

int part1(const schematic_t& schematic)
{
    auto search = [&](auto& number){
        for(auto& symbol : schematic.symbols){
            if(near(number, symbol)){
                return true;
            }
        }
        return false;
    };

    int sum = 0;
    for(auto& number : schematic.numbers){
        if(search(number)){
            sum += std::stoi(number.name);
        }
    }
    return sum;
}

int part2(const schematic_t& schematic)
{
    auto search = [&](auto& gear){
        std::vector<int> found;
        for(auto& number : schematic.numbers){
            if(near(number, gear)){
                found.push_back(std::stoi(number.name));
            }
        }
        return found.size() == 2 ? found[0] * found[1] : 0;
    };

    int sum = 0;
    for(auto& symbol : schematic.symbols){
        if(symbol.name == "*"){
            sum += search(symbol);
        }
    }
    return sum;
}

void main()
{
    auto test_values = load_input("../src/day03/test_input.txt");
    auto actual_values = load_input("../src/day03/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}