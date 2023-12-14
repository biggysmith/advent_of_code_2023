#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <unordered_map>

struct dish_t{
    std::string map;
    int width = 0;
    int height = 0;

    char& get(int x,int y){ 
        return map[y*width+x]; 
    }
};

bool operator==(const dish_t& a, const dish_t& b){ return a.map == b.map; }

struct dish_hash {
    size_t operator()(const dish_t& dish) const {
        return std::hash<std::string>()(dish.map);
    }
};

struct dish_load_t{
    size_t idx;
    size_t load;
};

using cache_t = std::unordered_map<dish_t, dish_load_t, dish_hash>;

dish_t load_input(const std::string& file){
    dish_t ret;
    std::ifstream fs(file);
    std::string line;

    while (std::getline(fs, line)) {
        ret.map += line;
        ret.width = (int)line.size();
        ret.height++;
    }

    return ret;
}

enum dir_enum { e_north=0, e_west, e_south, e_east }; 

void tilt(dish_t& dish, dir_enum dir)
{
    if(dir == e_north){
        for(int y=0; y<dish.height; ++y){
           for(int x=0; x<dish.width; ++x){
                if(dish.get(x,y) == 'O'){
                    int start_y = y - 1;
                    int new_y = start_y;
                    while(new_y >= 0 && dish.get(x, new_y) == '.'){
                        new_y--;
                    }

                    if(new_y != start_y){ // we've moved
                        dish.get(x, y) = '.';
                        dish.get(x, new_y+1) = 'O';
                    }
                }
            } 
        }
    }else if(dir == e_west){
        for(int x=0; x<dish.width; ++x){
            for(int y=0; y<dish.height; ++y){
                if(dish.get(x,y) == 'O'){
                    int start_x = x - 1;
                    int new_x = start_x;
                    while(new_x >= 0 && dish.get(new_x, y) == '.'){
                        new_x--;
                    }

                    if(new_x != start_x){ // we've moved
                        dish.get(x, y) = '.';
                        dish.get(new_x+1, y) = 'O';
                    }
                }
            } 
        }
    }else if(dir == e_south){
        for(int y=dish.height-1; y>=0; --y){
           for(int x=0; x<dish.width; ++x){
                if(dish.get(x,y) == 'O'){
                    int start_y = y + 1;
                    int new_y = start_y;
                    while(new_y < dish.height && dish.get(x, new_y) == '.'){
                        new_y++;
                    }

                    if(new_y != start_y){ // we've moved
                        dish.get(x, y) = '.';
                        dish.get(x, new_y-1) = 'O';
                    }
                }
            } 
        }
    }else if(dir == e_east){
        for(int x=dish.width-1; x>=0; --x){
            for(int y=0; y<dish.height; ++y){
                if(dish.get(x,y) == 'O'){
                    int start_x = x + 1;
                    int new_x = start_x;
                    while(new_x < dish.width && dish.get(new_x, y) == '.'){
                        new_x++;
                    }

                    if(new_x != start_x){ // we've moved
                        dish.get(x, y) = '.';
                        dish.get(new_x-1, y) = 'O';
                    }
                }
            } 
        }
    }
}

size_t calc_load(dish_t& dish){
    size_t sum = 0;
    for(int y=0; y<dish.height; ++y){
        for(int x=0; x<dish.width; ++x){
            if(dish.get(x,y) == 'O'){
                sum += dish.height - y;
            }
        }
    }
    return sum;
}

size_t get_idx(size_t a, size_t b){
    return (((4'000'000'000 - a) % (b-a)) + a) - 1;
}

size_t part1(dish_t dish) 
{    
    tilt(dish, e_north);
    return calc_load(dish);
}

size_t part2(dish_t dish) 
{    
    cache_t cache;

    size_t i = 0;
    while(true){
        tilt(dish, dir_enum(i % 4));
        if(cache.count(dish)){
            break; // oh we got a cycle!
        }
        cache[dish] = { i, calc_load(dish) };
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