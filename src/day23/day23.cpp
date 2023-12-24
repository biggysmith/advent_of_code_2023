#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <timer.hpp>

struct pos_t{
    int x, y;
};

bool operator==(const pos_t& a, const pos_t& b){ return std::tuple(a.x, a.y) == std::tuple(b.x, b.y); }
bool operator!=(const pos_t& a, const pos_t& b){ return std::tuple(a.x, a.y) != std::tuple(b.x, b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }
pos_t& operator+=(pos_t& a, const pos_t& b){ a.x += b.x; a.y += b.y; return a; }

size_t cantor(size_t a, size_t b){ return (a + b + 1) * (a + b) / 2 + b; };
struct pos_hash {
    size_t operator()(const pos_t& pos) const {
        return cantor(pos.x, pos.y);
    }
};

struct trail_map_t{
    std::string grid;
    int width = 0;
    int height = 0;

    char get(const pos_t& p) const { return grid[p.y * width + p.x]; }
    bool in_grid(const pos_t& pos) const { return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height; }

    bool is_path(const pos_t& pos) const { return get(pos) == '.'; }
    bool is_wall(const pos_t& pos) const { return get(pos) == '#'; }
    bool is_slope(const pos_t& pos) const { return !is_path(pos) && !is_wall(pos); }
};

trail_map_t load_input(const std::string& file){
    trail_map_t ret;
    std::ifstream fs(file);
    std::string line;

    while (std::getline(fs, line)) {
        ret.grid += line;
        ret.height++;
        ret.width = (int)line.size();
    }

    return ret;
}

pos_t right {  1,  0 };
pos_t down  {  0,  1 };
pos_t left  { -1,  0 };
pos_t up    {  0, -1 };

struct node_t {
    pos_t pos;
    int steps;
};

bool operator==(const node_t& a, const node_t& b){ return a.pos == b.pos; }

struct node_hash {
    size_t operator()(const node_t& e) const {
        return cantor(e.pos.x, e.pos.y);
    }
};

using visited_t = std::unordered_set<pos_t, pos_hash>;
using out_nodes_t = std::unordered_set<node_t, node_hash>;
using graph_t = std::unordered_map<pos_t, out_nodes_t, pos_hash>;

node_t find_neighbour(const trail_map_t& trail_map, const pos_t& start, const pos_t& dir)
{
    int steps = 0;
    pos_t prev = start;
    pos_t pos = start + dir;

    while(true) // step along path until we find junction neighbor
    {
        auto can_move = [&](const pos_t& next_pos){
            return next_pos != prev && trail_map.in_grid(next_pos) && !trail_map.is_wall(next_pos);
        };

        bool can_move_right = can_move(pos + right);
        bool can_move_down  = can_move(pos + down);
        bool can_move_left  = can_move(pos + left);
        bool can_move_up    = can_move(pos + up);

        int possible_moves = can_move_right + can_move_down + can_move_left + can_move_up;

        if(possible_moves == 1) 
        {
            prev = pos;
            if(can_move_right)       pos += right;
            else if(can_move_down)   pos += down;
            else if(can_move_left)   pos += left;
            else if(can_move_up)     pos += up;
            steps++;
        }  
        else 
        {
            return { pos, steps+1 }; // found neighbor
        }

        if(pos == pos_t{ trail_map.width-2, trail_map.width-1 }){
            return { pos, steps+1 };  // exit node
        }
    }
}

void build_graph(const trail_map_t& trail_map, graph_t& graph, bool part1)
{
    graph[{1,0}].insert(find_neighbour(trail_map, {1,0}, down)); // start node

    for(int y=0; y<trail_map.height; ++y) 
    {
        for(int x=0; x<trail_map.width; ++x) 
        {
            pos_t pos = { x, y };
            if(trail_map.is_path(pos))
            {
                auto is_valid_slope = [&](const pos_t& p, const pos_t& dir){ 
                    pos_t next = pos + dir;
                    if(part1){
                        return trail_map.in_grid(next) && (
                               trail_map.get(next) == '<' && dir == left  || 
                               trail_map.get(next) == '^' && dir == up    || 
                               trail_map.get(next) == '>' && dir == right || 
                               trail_map.get(next) == 'v' && dir == down  );
                    }else{
                        return trail_map.in_grid(next) && trail_map.is_slope(next); 
                    }
                };

                if(is_valid_slope(pos, left))   graph[pos].insert(find_neighbour(trail_map, pos, left));
                if(is_valid_slope(pos, right))  graph[pos].insert(find_neighbour(trail_map, pos, right));
                if(is_valid_slope(pos, up))     graph[pos].insert(find_neighbour(trail_map, pos, up));
                if(is_valid_slope(pos, down))   graph[pos].insert(find_neighbour(trail_map, pos, down));
            }
        }
    }
}

void dfs(graph_t& graph, visited_t& visited, const pos_t& pos, const pos_t& dst, int steps, int& max_steps)
{
    if(pos == dst){
        max_steps = std::max(max_steps, steps);
        return;
    }

    if(visited.count(pos)){
        return;
    }

    visited.insert(pos);

    for(auto& node : graph[pos]) {
        /*if(visited.count(node.pos)){
            continue;
        }*/
        //visited.insert(pos);
        dfs(graph, visited, node.pos, dst, steps + node.steps, max_steps);
        //visited.erase(pos);
    }

    visited.erase(pos);
}

size_t process(const trail_map_t& trail_map, bool part1) 
{
    scoped_timer t;

    graph_t graph;
    build_graph(trail_map, graph, part1);

    int max_steps = 0;
    visited_t visited;
    dfs(graph, visited, { 1, 0 }, { trail_map.width-2, trail_map.height-1 }, 0, max_steps);

    return max_steps;
}

void main()
{
    auto test_values = load_input("../src/day23/test_input.txt");
    auto actual_values = load_input("../src/day23/input.txt");

    std::cout << "part1: " << process(test_values, true) << std::endl;
    std::cout << "part1: " << process(actual_values, true) << std::endl;

    std::cout << "part2: " << process(test_values, false) << std::endl;
    std::cout << "part2: " << process(actual_values, false) << std::endl;
}