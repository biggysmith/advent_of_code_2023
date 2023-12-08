#include <vector>
#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <numeric>

struct node_t{
    std::string node;
    std::string left;
    std::string right; 
};

struct maps_t{
    std::string dirs;
    std::map<std::string,node_t> nodes;
};

std::vector<std::string> parse(const std::string& line, std::regex& reg) {
   return std::vector<std::string>(std::sregex_token_iterator(line.begin(), line.end(), reg), std::sregex_token_iterator());
}

maps_t load_input(const std::string& file){
    maps_t maps;
    std::ifstream fs(file);
    std::string line;
    int y = 0;
    while (std::getline(fs, line)) {
        if(!line.empty()){
            if(y==0){
                maps.dirs = line;
            }else{
                auto strs = parse(line, std::regex("[\\w]+"));
                maps.nodes[strs[0]] = { strs[0], strs[1], strs[2] };
            }
        }
        y++;
    }
    return maps;
}

size_t part1(const maps_t& maps)
{    
    int pos = 0;
    int steps = 0;
    auto curr = maps.nodes.at("AAA");
    while(curr.node != "ZZZ"){
        curr = maps.nodes.at(maps.dirs[pos] == 'L' ? curr.left : curr.right);
        pos = (pos+1) % maps.dirs.size();
        steps++;
    }
    return steps;
}

size_t part2(const maps_t& maps)
{
    int pos = 0;
    int steps = 1;
    std::vector<node_t> curr;
    std::vector<int> repeats;
    for(auto&& [start, node] : maps.nodes){
        if(start[2] == 'A'){
            curr.push_back(node);
            repeats.push_back(0);
        }
    }
    while(std::any_of(repeats.begin(), repeats.end(), [](auto& r){ return r == 0; })){
        for(int i=0; i<curr.size(); ++i){
            curr[i] = maps.nodes.at(maps.dirs[pos] == 'L' ? curr[i].left : curr[i].right);
            if(curr[i].node[2] == 'Z' && repeats[i]==0){
                repeats[i] = steps;
            }
        }
        pos = (pos+1) % maps.dirs.size();
        steps++;
    }

    return std::accumulate(repeats.begin(), repeats.end(), 1ull, [](auto& a, auto& b){
        return std::lcm(a, b);    
    });
}

void main()
{
    auto test_values0 = load_input("../src/day08/test_input0.txt");
    auto test_values1 = load_input("../src/day08/test_input1.txt");
    auto test_values2 = load_input("../src/day08/test_input2.txt");
    auto actual_values = load_input("../src/day08/input.txt");

    std::cout << "part1: " << part1(test_values0) << std::endl;
    std::cout << "part1: " << part1(test_values1) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values2) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}