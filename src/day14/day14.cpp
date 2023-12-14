#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <unordered_map>
#include <timer.hpp>

struct pos_t{
    int x, y;
};

bool operator==(const pos_t& a, const pos_t& b){ return std::tuple(a.x,a.y) == std::tuple(b.x,b.y); }

struct pos_hash {
    size_t operator()(const pos_t& pos) const {
        return (pos.x + pos.y + 1) * (pos.x + pos.y) / 2 + pos.y; // cantor
    }
};

struct dish_t{
    std::unordered_set<pos_t,pos_hash> round_rocks;
    std::unordered_set<pos_t,pos_hash> cube_rocks;
    int width;
    int height;
};

bool operator==(const dish_t& a, const dish_t& b){ return std::tuple(a.round_rocks,a.cube_rocks) == std::tuple(b.round_rocks,b.cube_rocks); }

struct dish_hash {
    size_t operator()(const dish_t& dish) const {
        //auto combine = [&](size_t acc, const pos_t& pos){ return acc ^ pos_hash()(pos); };
        auto combine = [&](size_t acc, const pos_t& pos){ return acc + (((324723947 + pos.x)) ^ 93485734985) + (((324723947 + pos.y)) ^ 93485734985); };
        size_t round = std::accumulate(dish.round_rocks.begin(), dish.round_rocks.end(), 0ull, combine);
        size_t cube = std::accumulate(dish.cube_rocks.begin(), dish.cube_rocks.end(), 0ull, combine);
        return round ^ cube;
    }
};

dish_t load_input(const std::string& file){
    dish_t ret;
    std::ifstream fs(file);
    std::string line;

    int y = 0;
    while (std::getline(fs, line)) {
        for(int x=0; x<line.size(); ++x){
            if(line[x] == 'O'){
                ret.round_rocks.insert({x,y});
            }else if(line[x] == '#'){
                ret.cube_rocks.insert({x,y});
            }
        }
        ret.width = (int)line.size();
        y++;
    }
    ret.height = y;

    return ret;
}

enum dir_enum { e_north=0, e_west, e_south, e_east }; 

void tilt(dish_t& dish, dir_enum dir)
{
    if(dir == e_north){
        for(int y=0; y<dish.height; ++y){
           for(int x=0; x<dish.width; ++x){
                if(dish.round_rocks.count({x,y})){
                    int y2 = y-1;
                    while(y2!=-1 && !dish.round_rocks.count({x,y2}) && !dish.cube_rocks.count({x,y2})){
                        y2--;
                    }

                    if(y2 != y-1){ // we've moved
                        dish.round_rocks.erase({x,y});
                        dish.round_rocks.insert({x,std::clamp(y2+1,0,dish.height-1)});
                    }
                }
            } 
        }
    }else if(dir == e_west){
        for(int x=0; x<dish.width; ++x){
            for(int y=0; y<dish.height; ++y){
                if(dish.round_rocks.count({x,y})){
                    int x2 = x-1;
                    while(x2!=-1 && !dish.round_rocks.count({x2,y}) && !dish.cube_rocks.count({x2,y})){
                        x2--;
                    }

                    if(x2 != x-1){ // we've moved
                        dish.round_rocks.erase({x,y});
                        dish.round_rocks.insert({std::clamp(x2+1,0,dish.width-1),y});
                    }
                }
            } 
        }
    }else if(dir == e_south){
        for(int y=dish.height-1; y>=0; --y){
           for(int x=0; x<dish.width; ++x){
                if(dish.round_rocks.count({x,y})){
                    int y2 = y+1;
                    while(y2!=dish.height && !dish.round_rocks.count({x,y2}) && !dish.cube_rocks.count({x,y2})){
                        y2++;
                    }

                    if(y2 != y+1){ // we've moved
                        dish.round_rocks.erase({x,y});
                        dish.round_rocks.insert({x,std::clamp(y2-1,0,dish.height-1)});
                    }
                }
            } 
        }
    }else if(dir == e_east){
        for(int x=dish.width-1; x>=0; --x){
            for(int y=0; y<dish.height; ++y){
                if(dish.round_rocks.count({x,y})){
                    int x2 = x+1;
                    while(x2!=dish.width && !dish.round_rocks.count({x2,y}) && !dish.cube_rocks.count({x2,y})){
                        x2++;
                    }

                    if(x2 != x+1){ // we've moved
                        dish.round_rocks.erase({x,y});
                        dish.round_rocks.insert({std::clamp(x2-1,0,dish.width-1),y});
                    }
                }
            } 
        }
    }
}

size_t load(const dish_t& dish){
    size_t sum = 0;
    for(auto& round_rock : dish.round_rocks){
        sum += dish.height - round_rock.y;
    }
    return sum;
}

size_t part1(dish_t dish) 
{    
    tilt(dish, e_north);
    return load(dish);
}

struct dish_load_t{
    size_t idx;
    size_t load;
};

size_t get_idx(size_t a, size_t b){
    return (((4000000000 - a) % (b-a)) + a) - 1;
}

size_t part2(dish_t dish) 
{    
    scoped_timer t;

    std::unordered_map<dish_t,dish_load_t,dish_hash> cache;

    size_t i = 0;
    while(true){
        tilt(dish, dir_enum(i % 4));
        if(cache.count(dish)){
            break;
        }
        cache[dish] = { i, load(dish) };
        i++;
    }

    size_t idx = get_idx(cache[dish].idx, i); 
    auto it = std::find_if(cache.begin(), cache.end(), [&](auto& c){
        return c.second.idx == idx;
    });
    return it->second.load;
}

void main()
{
    auto test_values = load_input("../src/day14/test_input.txt");
    auto actual_values = load_input("../src/day14/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}