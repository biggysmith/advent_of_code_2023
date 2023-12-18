#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <queue>
#include <unordered_set>

struct pos_t{
    int x, y;
};

bool operator==(const pos_t& a, const pos_t& b){ return std::tuple(a.x, a.y) == std::tuple(b.x, b.y); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }
pos_t& operator+=(pos_t& a, const pos_t& b){ a.x += b.x; a.y += b.y; return a; }

struct traffic_map_t{
    std::vector<int> grid;
    int width = 0;
    int height = 0;

    int get(const pos_t& p) const{ return grid[p.y*width + p.x]; }
    bool in_grid(const pos_t& pos) const { return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height; }
};

traffic_map_t load_input(const std::string& file){
    traffic_map_t ret;
    std::ifstream fs(file);
    std::string line;

    while (std::getline(fs, line)) {
        for(auto& c : line){
            ret.grid.push_back(c-'0');
        }
        ret.width = (int)line.size();
        ret.height++;
    }

    return ret;
}

struct move_t{
    pos_t pos;
    pos_t dir;
    int loss = 0;
    int straight = 0;
};

struct move_hash {
    size_t operator()(const move_t& move) const {
        auto cantor = [](size_t a, size_t b){ return (a + b + 1) * (a + b) / 2 + b; };
        return cantor(move.pos.x, cantor(move.pos.y, cantor(move.dir.x, move.dir.y)));
    }
};

bool operator<(const move_t& a, const move_t& b){ return a.loss > b.loss; }
bool operator==(const move_t& a, const move_t& b){ return std::tuple(a.pos, a.dir, a.straight) == std::tuple(b.pos, b.dir, b.straight); }

pos_t rotate_cw(const pos_t& dir){ return { -dir.y, dir.x }; }
pos_t rotate_ccw(const pos_t& dir){ return { dir.y, -dir.x }; }

size_t dijkstra(const traffic_map_t& traffic, int min_travel, int max_travel) 
{    
    std::priority_queue<move_t> q;
    std::unordered_set<move_t,move_hash> visited;

    pos_t src { 0, 0 };
    pos_t dst { traffic.width-1, traffic.height-1 };

    q.push({ src, { 0, 1 } });
    q.push({ src, { 1, 0 } });

    while (!q.empty()) 
    {
        auto curr = q.top();
        q.pop();

        if (visited.count(curr)) {
            continue;
        }

        visited.insert(curr);

        if(curr.pos == dst){
            return curr.loss;
        }

        if(curr.straight >= (min_travel-1)) // left
        {
            move_t move;
            move.dir = rotate_ccw(curr.dir); 
            move.pos = curr.pos + move.dir;
            if(traffic.in_grid(move.pos)) {
                move.loss = curr.loss + traffic.get(move.pos);
                move.straight = 0;
                q.push(move);
            }
        }

        if(curr.straight >= (min_travel-1)) // right
        {
            move_t move;
            move.dir = rotate_cw(curr.dir);  
            move.pos = curr.pos + move.dir;
            if(traffic.in_grid(move.pos)){
                move.loss = curr.loss + traffic.get(move.pos);
                move.straight = 0;
                q.push(move);
            }
        }

        if(curr.straight < (max_travel-1)) // forward
        {
            move_t move;
            move.dir = curr.dir;
            move.pos = curr.pos + move.dir;
            if(traffic.in_grid(move.pos)){
                move.loss = curr.loss + traffic.get(move.pos);
                move.straight = curr.straight + 1;
                q.push(move);
            }
        }
    }

    return 0;
}

void main()
{
    auto test_values = load_input("../src/day17/test_input.txt");
    auto actual_values = load_input("../src/day17/input.txt");

    std::cout << "part1: " << dijkstra(test_values, 1, 3) << std::endl;
    std::cout << "part1: " << dijkstra(actual_values, 1, 3) << std::endl;

    std::cout << "part2: " << dijkstra(test_values, 4, 10) << std::endl;
    std::cout << "part2: " << dijkstra(actual_values, 4, 10) << std::endl;
}