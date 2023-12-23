#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <regex>
#include <unordered_map>

std::vector<std::string> parse(const std::string& line, std::regex& reg) {
   return std::vector<std::string>(std::sregex_token_iterator(line.begin(), line.end(), reg), std::sregex_token_iterator());
}

struct pos_t {
    int x, y, z;
};

bool operator==(const pos_t& a, const pos_t& b){ return std::tuple(a.x, a.y) == std::tuple(b.x, b.y); }

struct pos_hash {
    size_t operator()(const pos_t& pos) const {
        auto cantor = [](size_t a, size_t b){ return (a + b + 1) * (a + b) / 2 + b; };
        return cantor(pos.x, pos.y);
    }
};

struct box_t {
    pos_t mn, mx;
};

using boxes_t = std::vector<box_t>;

boxes_t load_input(const std::string& file){
    boxes_t ret;
    std::ifstream fs(file);
    std::string line;

    while (std::getline(fs, line)) {
        auto strs = parse(line, std::regex("[\\d]+"));
        ret.push_back({{ std::stoi(strs[0]), std::stoi(strs[1]), std::stoi(strs[2]) }, { std::stoi(strs[3]), std::stoi(strs[4]), std::stoi(strs[5]) }});
    }

    std::sort(ret.begin(), ret.end(), [](auto& a, auto& b){
        return a.mn.z < b.mn.z; 
    });

    return ret;
}

using height_map_t = std::unordered_map<pos_t, int, pos_hash>;

void move_box(box_t& box, height_map_t& height_map)
{
    int max_height_seen = 0;
    for(int y=box.mn.y; y<=box.mx.y; ++y){
        for(int x=box.mn.x; x<=box.mx.x; ++x){
            max_height_seen = std::max(height_map[{x,y}], max_height_seen);
        }
    }
    int move = std::max(box.mn.z - max_height_seen - 1, 0);
    box.mn.z -= move;
    box.mx.z -= move;
}

boxes_t move_boxes(boxes_t boxes, int exclude_box=-1)
{
    height_map_t height_map;

    for(int i=0; i<boxes.size(); ++i)
    {
        if(i != exclude_box)
        {
            auto& box = boxes[i];
            move_box(box, height_map);
            for(int y=box.mn.y; y<=box.mx.y; ++y){
                for(int x=box.mn.x; x<=box.mx.x; ++x){
                    height_map[{x,y}] = box.mx.z;
                }
            }
        }
    }

    return boxes;
};

size_t part1(const boxes_t& boxes) 
{    
    boxes_t settled_boxes = move_boxes(boxes);

    size_t stable = boxes.size();
    for(int i=0; i<boxes.size(); ++i)
    {
        auto new_settled_boxes = move_boxes(settled_boxes, i);

        for(int j=0; j<settled_boxes.size(); ++j){
            if(j != i && new_settled_boxes[j].mn.z != settled_boxes[j].mn.z){
                stable--; // we have a diff therefore not stable
                break;
            }
        }
    }

    return stable;
}

size_t part2(const boxes_t& boxes) 
{    
    boxes_t settled_boxes = move_boxes(boxes);

    size_t fall_sum = 0;
    for(int i=0; i<boxes.size(); ++i)
    {
        boxes_t new_settled_boxes = move_boxes(settled_boxes, i);

        for(int j=0; j<settled_boxes.size(); ++j){
            if(j != i && new_settled_boxes[j].mn.z != settled_boxes[j].mn.z){
                fall_sum++; // we have a diff therefore increment full sum
            }
        }
    }

    return fall_sum;
}

void main()
{
    auto test_values = load_input("../src/day22/test_input.txt");
    auto actual_values = load_input("../src/day22/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}