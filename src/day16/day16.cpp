#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <queue>
#include <set>

#include "timer.hpp"

struct pos_t{
    int x, y;
};

bool operator==(const pos_t& a, const pos_t& b){ return std::tuple(a.x, a.y) == std::tuple(b.x, b.y); }
bool operator<(const pos_t& a, const pos_t& b){ return std::tuple(a.x, a.y) < std::tuple(b.x, b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }
pos_t& operator+=(pos_t& a, const pos_t& b){ a.x += b.x; a.y += b.y; return a; }

struct layout_t{
    std::string grid;
    int width = 0;
    int height = 0;

    char& get(const pos_t& p) { return grid[p.y*width + p.x]; }
    char get(const pos_t& p) const{ return grid[p.y*width + p.x]; }
    bool in_grid(const pos_t& pos) const { return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height; }
};

layout_t load_input(const std::string& file){
    layout_t ret;
    std::ifstream fs(file);
    std::string line;

    while (std::getline(fs, line)) {
        ret.grid += line;
        ret.width = (int)line.size();
        ret.height++;
    }

    return ret;
}

pos_t up    {  0, -1 };
pos_t down  {  0,  1 };
pos_t left  { -1,  0 };
pos_t right {  1,  0 };

struct beam_t{
    pos_t pos;
    pos_t dir;
};

using set_t = std::set<pos_t>;

#if 0
size_t energize(const layout_t& layout, const beam_t& start) 
{    
    auto energised = layout;

    set_t visited;
    std::queue<beam_t> q;
    q.push(start);

    while (!q.empty()) 
    {
        auto curr = q.front();
        q.pop();

        if(visited.count(curr.pos)){
            continue;
        }

        set_t local_visited;

        while(true)
        {
            if(local_visited.count(curr.pos)){
                break;
            }

            if(!layout.in_grid(curr.pos)){
                break;
            }

            if(layout.get(curr.pos) == '.'){
                energised.get(curr.pos) = '#';
                curr.pos = curr.pos + curr.dir;
            }
            else if(layout.get(curr.pos) == '/')
            {
                if(curr.dir == up){
                    curr.dir = right;
                }else if(curr.dir == down){
                    curr.dir = left;
                }else if(curr.dir == left){
                    curr.dir = down;
                }else if(curr.dir == right){
                    curr.dir = up;
                }
                local_visited.insert(curr.pos);
                energised.get(curr.pos) = '#';
                curr.pos = curr.pos + curr.dir;
            }
            else if(layout.get(curr.pos) == '\\')
            {
                if(curr.dir == up){
                    curr.dir = left;
                }else if(curr.dir == down){
                    curr.dir = right;
                }else if(curr.dir == left){
                    curr.dir = up;
                }else if(curr.dir == right){
                    curr.dir = down;
                }
                local_visited.insert(curr.pos);
                energised.get(curr.pos) = '#';
                curr.pos = curr.pos + curr.dir;
            }
            else if(layout.get(curr.pos) == '-')
            {
                if(curr.dir == left || curr.dir == right){
                    local_visited.insert(curr.pos);
                    energised.get(curr.pos) = '#';
                    curr.pos = curr.pos + curr.dir;
                }else if(curr.dir == up || curr.dir == down){
                    energised.get(curr.pos) = '#';
                    if(!visited.count(curr.pos)){
                        q.push({ curr.pos + left, left });
                        q.push({ curr.pos + right, right });
                        visited.insert(curr.pos);
                    }
                    local_visited.insert(curr.pos);
                    break;
                }
            }
            else if(layout.get(curr.pos) == '|')
            {
                if(curr.dir == up || curr.dir == down){
                    local_visited.insert(curr.pos);
                    energised.get(curr.pos) = '#';
                    curr.pos = curr.pos + curr.dir;
                }else if(curr.dir == left || curr.dir == right){
                    energised.get(curr.pos) = '#';
                    if(!visited.count(curr.pos)){
                        q.push({ curr.pos + up, up });
                        q.push({ curr.pos + down, down });
                        visited.insert(curr.pos);
                    }
                    local_visited.insert(curr.pos);
                    break;
                }
            }
        }

    }

    return std::accumulate(energised.grid.begin(), energised.grid.end(), 0ull, [](auto& sum, auto& c){
        return sum + (c == '#');    
    });
}
#endif

void trace_beam(beam_t curr, const layout_t& layout, layout_t& energised, set_t& visited) 
{    
    if(visited.count(curr.pos)){
        return; // already traced this
    }

    set_t local_visited;

    while(true)
    {
        if(local_visited.count(curr.pos)){
            return; // got a loop
        }

        if(!layout.in_grid(curr.pos)){
            return;
        }

        if(layout.get(curr.pos) == '.'){
            energised.get(curr.pos) = '#';
            curr.pos += curr.dir;
        }
        else if(layout.get(curr.pos) == '/')
        {
            if(curr.dir == up){
                curr.dir = right;
            }else if(curr.dir == down){
                curr.dir = left;
            }else if(curr.dir == left){
                curr.dir = down;
            }else if(curr.dir == right){
                curr.dir = up;
            }
            local_visited.insert(curr.pos);
            energised.get(curr.pos) = '#';
            curr.pos += curr.dir;
        }
        else if(layout.get(curr.pos) == '\\')
        {
            if(curr.dir == up){
                curr.dir = left;
            }else if(curr.dir == down){
                curr.dir = right;
            }else if(curr.dir == left){
                curr.dir = up;
            }else if(curr.dir == right){
                curr.dir = down;
            }
            local_visited.insert(curr.pos);
            energised.get(curr.pos) = '#';
            curr.pos += curr.dir;
        }
        else if(layout.get(curr.pos) == '-')
        {
            if(curr.dir == left || curr.dir == right){
                local_visited.insert(curr.pos);
                energised.get(curr.pos) = '#';
                curr.pos += curr.dir;
            }else if(curr.dir == up || curr.dir == down){
                energised.get(curr.pos) = '#';
                if(!visited.count(curr.pos)){
                    visited.insert(curr.pos);
                    trace_beam({ curr.pos + left, left }, layout, energised, visited); 
                    trace_beam({ curr.pos + right, right }, layout, energised, visited); 
                }
                local_visited.insert(curr.pos);
                return;
            }
        }
        else if(layout.get(curr.pos) == '|')
        {
            if(curr.dir == up || curr.dir == down){
                local_visited.insert(curr.pos);
                energised.get(curr.pos) = '#';
                curr.pos += curr.dir;
            }else if(curr.dir == left || curr.dir == right){
                energised.get(curr.pos) = '#';
                if(!visited.count(curr.pos)){
                    visited.insert(curr.pos);
                    trace_beam({ curr.pos + up, up }, layout, energised, visited); 
                    trace_beam({ curr.pos + down, down }, layout, energised, visited); 
                }
                local_visited.insert(curr.pos);
                return;
            }
        }
    }
}

size_t energize(const layout_t& layout, const beam_t& start)
{
    set_t visited;
    auto energized = layout;
    trace_beam(start, layout, energized, visited); 
    return std::accumulate(energized.grid.begin(), energized.grid.end(), 0ull, [](auto& sum, auto& c){
        return sum + (c == '#');    
    });
}

size_t part1(const layout_t& layout) 
{    
    pos_t start { 0, 0 };
    beam_t start_beam = { start, right };
    return energize(layout, start_beam);
}

size_t part2(const layout_t& layout) 
{    
    scoped_timer t;

    size_t max_energize = 0ull;
    for(int i=0; i<layout.width; ++i){
        max_energize = std::max(max_energize, energize(layout, { { 0, i }, right }));
        max_energize = std::max(max_energize, energize(layout, { { layout.width-1, i }, left }));
        max_energize = std::max(max_energize, energize(layout, { { i, 0 }, down }));
        max_energize = std::max(max_energize, energize(layout, { { i, layout.height-1 }, up }));
    }

    return max_energize;
}

void main()
{
    auto test_values = load_input("../src/day16/test_input.txt");
    auto actual_values = load_input("../src/day16/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}