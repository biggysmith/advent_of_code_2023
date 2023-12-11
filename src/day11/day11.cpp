#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

struct pos_t{
    int64_t x, y;
};

struct rect_t{
    int64_t top, bottom, left, right;
};

struct galaxy_t{
    pos_t pos, offset;
};

using galaxies_t = std::vector<galaxy_t>;

rect_t dimensions(const galaxies_t& galaxies) {
    rect_t dim = { INT64_MAX, INT64_MIN, INT64_MAX, INT64_MIN };
    for(auto& galaxy : galaxies){
        dim.top = std::min(galaxy.pos.y, dim.top);
        dim.bottom = std::max(galaxy.pos.y, dim.bottom);
        dim.left = std::min(galaxy.pos.x, dim.left);
        dim.right = std::max(galaxy.pos.x, dim.right);
    }
    return dim;
}

int64_t manhatten(const pos_t& a, const pos_t& b){
    return std::abs(a.x-b.x) + std::abs(a.y-b.y);
}

galaxies_t load_input(const std::string& file){
    galaxies_t galaxies;
    std::ifstream fs(file);
    std::string line;

    int y = 0;
    while (std::getline(fs, line)) {
        for(int x=0; x<line.size(); ++x){
            if(line[x] == '#'){
                galaxies.push_back({{x,y}, {0,0}});
            }
        }
        y++;
    }

    return galaxies;
}

size_t process(galaxies_t galaxies, int64_t expansion) 
{    
    expansion -= 1;
    rect_t dims = dimensions(galaxies);

    for(int64_t x=dims.left; x<=dims.right; ++x){
        if(std::find_if(galaxies.begin(), galaxies.end(), [&](auto& galaxy){ return galaxy.pos.x == x; }) == galaxies.end()){
            for(auto& galaxy : galaxies){
                galaxy.offset.x += galaxy.pos.x > x ? expansion : 0;
            }
        }
    }

    for(int64_t y=dims.top; y<=dims.bottom; ++y){
        if(std::find_if(galaxies.begin(), galaxies.end(), [&](auto& galaxy){ return galaxy.pos.y == y; }) == galaxies.end()){
            for(auto& galaxy : galaxies){
                galaxy.offset.y += galaxy.pos.y > y ? expansion : 0;
            }
        }
    }

    galaxies_t expanded;
    for(auto& galaxy : galaxies){
        expanded.push_back({{galaxy.pos.x + galaxy.offset.x, galaxy.pos.y + galaxy.offset.y}, {0,0}});
    }

    int64_t sum = 0;
    for(size_t i=0; i<expanded.size(); ++i) {
        for(size_t j=i+1; j<expanded.size(); ++j) {
            sum += manhatten(expanded[i].pos, expanded[j].pos);
        }
    }
    return sum;
}

void main()
{
    auto test_values = load_input("../src/day11/test_input.txt");
    auto actual_values = load_input("../src/day11/input.txt");

    std::cout << "part1: " << process(test_values, 2) << std::endl;
    std::cout << "part1: " << process(actual_values, 2) << std::endl;

    std::cout << "part2: " << process(test_values, 10) << std::endl;
    std::cout << "part2: " << process(test_values, 100) << std::endl;
    std::cout << "part2: " << process(actual_values, 1'000'000) << std::endl;
}