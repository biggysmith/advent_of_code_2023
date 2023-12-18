#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <regex>

std::vector<std::string> parse(const std::string& line, std::regex& reg) {
   return std::vector<std::string>(std::sregex_token_iterator(line.begin(), line.end(), reg), std::sregex_token_iterator());
}

size_t from_hex(const std::string& hex) {
    size_t ret;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> ret;
    return ret;
}

char get_dir(char dir){
    if(dir == '0'){
        return 'R';
    }else if(dir == '1'){
        return 'D';
    }else if(dir == '2'){
        return 'L';
    }else /*if(dir == 3)*/{
        return 'U';
    }
}

struct dig_t{
    char dir0, dir1;
    size_t meters0, meters1;
};

using digs_t = std::vector<dig_t>;

digs_t load_input(const std::string& file){
    digs_t ret;
    std::ifstream fs(file);
    std::string line;

    while (std::getline(fs, line)) {
        dig_t dig;
        auto strs = parse(line, std::regex("([0-9a-fA-F]{6})|[\\w]+"));
        dig.dir0 = strs[0][0];
        dig.meters0 = std::stoll(strs[1]);
        dig.dir1 = get_dir(strs[2].back());
        dig.meters1 = from_hex(strs[2].substr(0,5));
        ret.push_back(dig);
    }

    return ret;
}

struct pos_t{
    int64_t x, y;
};

pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y }; }
pos_t operator*(const pos_t& a, int64_t b){ return { a.x * b, a.y * b }; }

int64_t shoelace(const std::vector<pos_t>& points) {
    int64_t left_sum = 0;
    int64_t right_sum = 0;

    for (int i=0; i<points.size(); ++i) {
        int j = (i + 1) % points.size();
        left_sum  += points[i].x * points[j].y;
        right_sum += points[j].x * points[i].y;
    }

    return std::abs(left_sum - right_sum) / 2;
}

size_t lava_capacity(const digs_t& digs, bool part1) 
{    
    pos_t pos { 0, 0 };
    std::vector<pos_t> verts;

    for(int i=0; i<digs.size(); ++i)
    {
        auto& curr_dig = digs[i];
        auto& next_dig = digs[(i+1) % digs.size()];

        char curr_dir = part1 ? curr_dig.dir0 : curr_dig.dir1;
        char next_dir = part1 ? next_dig.dir0 : next_dig.dir1;

        pos_t dir;
        if(curr_dir == 'R'){
            dir = { 1, 0 };
        }else if(curr_dir == 'D'){
            dir = { 0, 1 };
        }else if(curr_dir == 'L'){
            dir = { -1, 0 };
        }else if(curr_dir == 'U'){
            dir = { 0, -1 };
        }

        pos = pos + (dir * (part1 ? curr_dig.meters0 : curr_dig.meters1));

        if((curr_dir == 'U' && next_dir == 'R') || (curr_dir == 'R' && next_dir == 'U')){
            verts.push_back(pos);
        }else if((curr_dir == 'R' && next_dir == 'D') || (curr_dir == 'D' && next_dir == 'R')){
            verts.push_back(pos + pos_t{ 1, 0 });
        }else if((curr_dir == 'D' && next_dir == 'L') || (curr_dir == 'L' && next_dir == 'D')){
            verts.push_back(pos + pos_t{ 1, 1 });
        }else if((curr_dir == 'L' && next_dir == 'U') || (curr_dir == 'U' && next_dir == 'L')){
            verts.push_back(pos + pos_t{ 0, 1 });
        }
    }

    return shoelace(verts);
}

void main()
{
    auto test_values = load_input("../src/day18/test_input.txt");
    auto actual_values = load_input("../src/day18/input.txt");

    std::cout << "part1: " << lava_capacity(test_values, true) << std::endl;
    std::cout << "part1: " << lava_capacity(actual_values, true) << std::endl;

    std::cout << "part2: " << lava_capacity(test_values, false) << std::endl;
    std::cout << "part2: " << lava_capacity(actual_values, false) << std::endl;
}