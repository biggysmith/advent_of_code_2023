#include <vector>
#include <iostream>
#include <fstream>
#include <string>

struct pos_t{
    int x, y;
};

bool operator==(const pos_t& a, const pos_t& b){ return a.x==b.x && a.y==b.y; }
bool operator!=(const pos_t& a, const pos_t& b){ return !(a == b); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x+b.x, a.y+b.y }; }
pos_t& operator+=(pos_t& a, const pos_t& b){ return a = a + b; }

struct map_t{
    char at(const pos_t& p) const { return grid[p.y*width+p.x]; }
    char& at(const pos_t& p) { return grid[p.y*width+p.x]; }

    std::vector<char> grid;
    int width;
    int height;
};

map_t load_input(const std::string& file){
    map_t map;
    std::ifstream fs(file);
    std::string line;

    map.height = 0;
    while (std::getline(fs, line)) {
        std::copy(line.begin(), line.end(), std::back_inserter(map.grid));
        map.width = (int)line.size();
        map.height++;
    }

    return map;
}

pos_t up { 0, -1 };
pos_t down { 0, 1 };
pos_t left { -1, 0 };
pos_t right { 1, 0 };

struct loop_t{
    map_t map;
    std::vector<pos_t> points;
    int steps = 0;
};

loop_t find_loop(const map_t& map){
    auto start = std::find(map.grid.begin(), map.grid.end(), 'S');
    pos_t start_pos = { (int)std::distance(map.grid.begin(), start) % map.width, (int)std::distance(map.grid.begin(), start) / map.width };

    loop_t loop;
    loop.map = map;
    std::fill(loop.map.grid.begin(), loop.map.grid.end(), '.');
    loop.map.at(start_pos) = '*';

    pos_t dir;
    pos_t pos = start_pos;
    if(map.at(pos + down) == '|' || map.at(pos + down) == 'L' || map.at(pos + down) == 'J'){
        dir = down;
    }else if(map.at(pos + up) == '|' || map.at(pos + up) == 'F' || map.at(pos + up) == '7'){
        dir = up;
    }else if(map.at(pos + left) == '-' || map.at(pos + left) == 'F' || map.at(pos + left) == 'L'){
        dir = left;
    }else if(map.at(pos + right) == '-' || map.at(pos + right) == '7' || map.at(pos + right) == 'J'){
        dir = right;
    }

    loop.points.push_back(pos);
    pos += dir;
    loop.steps++;

    while (pos != start_pos){
        char pipe = map.at(pos);
        if(pipe == 'L'){
            dir = dir == down ? right : up;
            loop.points.push_back(pos);
        }else if(pipe == 'J'){
            dir = dir == down ? left : up;
            loop.points.push_back(pos);
        }else if(pipe == '7'){
            dir = dir == up ? left : down;
            loop.points.push_back(pos);
        }else if(pipe == 'F'){
            dir = dir == up ? right : down;
            loop.points.push_back(pos);
        }
        loop.map.at(pos) = '*';
        pos += dir;
        loop.steps++;
    };

    return loop;
}

size_t part1(const map_t& map)
{    
    loop_t loop = find_loop(map);
    return loop.steps / 2;
}

bool point_in_path(const pos_t& pos, const std::vector<pos_t>& path){
    int num = (int)path.size();
    int j = num - 1;
    bool ret = false;
    for(int i=0; i<num; ++i){
        if(pos == path[i]){
            return true;
        }

        if((path[i].y > pos.y) != (path[j].y > pos.y)) {
            int slope = (pos.x - path[i].x) * (path[j].y - path[i].y) - (path[j].x - path[i].x) * (pos.y - path[i].y);
            if(slope == 0){
                return true;
            }
            if ((slope < 0) != (path[j].y < path[i].y)){
                ret = !ret;
            }
        }
        j = i;
    }

    return ret;
}

size_t part2(const map_t& map)
{
    loop_t loop = find_loop(map);

    int in_count = 0;
    for(int y=0; y<map.height; ++y){
        for(int x=0; x<map.width; ++x){
            if(loop.map.at({x,y}) != '*' && point_in_path({x,y}, loop.points)){
                in_count++;
            }
        }
    }

    return in_count;
}

void main()
{
    auto test_values0 = load_input("../src/day10/test_input0.txt");
    auto test_values1 = load_input("../src/day10/test_input1.txt");
    auto test_values2 = load_input("../src/day10/test_input2.txt");
    auto test_values3 = load_input("../src/day10/test_input3.txt");
    auto test_values4 = load_input("../src/day10/test_input4.txt");
    auto test_values5 = load_input("../src/day10/test_input5.txt");
    auto actual_values = load_input("../src/day10/input.txt");

    std::cout << "part1: " << part1(test_values0) << std::endl;
    std::cout << "part1: " << part1(test_values1) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values2) << std::endl;
    std::cout << "part2: " << part2(test_values3) << std::endl;
    std::cout << "part2: " << part2(test_values4) << std::endl;
    std::cout << "part2: " << part2(test_values5) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}