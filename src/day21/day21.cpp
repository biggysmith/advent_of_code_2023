#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
#include <numeric>
#include <queue>
#include "matrix.hpp"

struct pos_t{
    int x, y;
};

int mod(int a, int b) { return (a % b + b) % b; }
bool operator<(const pos_t& a, const pos_t& b){ return std::tuple(a.x, a.y) < std::tuple(b.x, b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }
pos_t operator%(const pos_t& a, const pos_t& b){ return { mod(a.x, b.x), mod(a.y, b.y) }; }

pos_t up    { 0, -1 };
pos_t right { 1, 0 };
pos_t down  { 0, 1 };
pos_t left  { -1, 0 };

struct garden_t {
    pos_t start;
    std::set<pos_t> rocks;
    int width;
    int height;
};

garden_t load_input(const std::string& file){
    garden_t ret;
    std::ifstream fs(file);
    std::string line;

    int y = 0;
    while (std::getline(fs, line)) {
        for(int x=0; x<line.size(); ++x){
            if(line[x] == '#'){
                ret.rocks.insert({x, y});
            }else if(line[x] == 'S'){
                ret.start = { x, y };
            }
        }
        ret.width = (int)line.size();
        y++;
    }
    ret.height = y;

    return ret;
}

using marked_t = std::set<pos_t>;

size_t process(const garden_t& garden, size_t steps) 
{    
    marked_t garden_a;
    marked_t garden_b;

    garden_a.insert(garden.start);
    pos_t dims = { garden.width, garden.height };

    auto try_step = [&](marked_t& marked, const pos_t& pos){
        if(!garden.rocks.count(pos % dims)){
            marked.insert(pos);
        }
    };

    for(int s=0; s<steps; ++s)
    {
        for(auto& pos : garden_a)
        {
            try_step(garden_b, pos + up);
            try_step(garden_b, pos + right);
            try_step(garden_b, pos + down);
            try_step(garden_b, pos + left);
        }

        std::swap(garden_a, garden_b);
        garden_b.clear();
    }

    return garden_a.size();
}

size_t part2(const garden_t& garden)
{
    size_t steps = 26501365ull;
    size_t mod = steps % garden.width;

    size_t x0 = mod;
    size_t y0 = process(garden, x0);

    size_t x1 = mod + garden.width;
    size_t y1 = process(garden, x1);

    size_t x2 = mod + garden.width + garden.width;
    size_t y2 = process(garden, x2);

    dmat33 mat(
        1, (double)x0, (double)(x0*x0),
        1, (double)x1, (double)(x1*x1),
        1, (double)x2, (double)(x2*x2)
    );

    dvec3 vec((double)y0, (double)y1, (double)y2);

    dvec3 abc = inverse(mat) * vec;

    double val = abc.z*(steps*steps) + abc.y*steps + abc.x;
    return (size_t)std::round(val);
}

void main()
{
    auto test_values = load_input("../src/day21/test_input.txt");
    auto actual_values = load_input("../src/day21/input.txt");

    std::cout << "part1: " << process(test_values, 6) << std::endl;
    std::cout << "part1: " << process(actual_values, 64) << std::endl;

    std::cout << "part2: " << part2(actual_values) << std::endl;
}