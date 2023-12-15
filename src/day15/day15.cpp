#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <regex>

struct step_t{
    std::string str;
    std::string label;
    char operation;
    size_t box_id;
    size_t focal_length;
};

using steps_t = std::vector<step_t>;

size_t hash(const std::string& str){
    return std::accumulate(str.begin(), str.end(), 0ull, [](auto& value, char c){ return ((value + c) * 17) % 256; });
}

steps_t load_input(const std::string& file){
    steps_t ret;
    std::ifstream fs(file);
    std::string line;
    std::regex word_regex("[^,]+");

    while (std::getline(fs, line)) {
        for(auto it=std::sregex_iterator(line.begin(), line.end(), word_regex); it!=std::sregex_iterator(); it++){
            step_t step;
            step.str = it->str();
            size_t eq_pos = step.str.find_first_of('=');
            if(eq_pos != std::string::npos){
                step.label = step.str.substr(0, eq_pos);
                step.operation = '=';
                step.focal_length = std::stoull(step.str.substr(eq_pos+1));
            }else{
                step.label = step.str.substr(0, step.str.size()-1);
                step.operation = '-';
            }
            step.box_id = hash(step.label);
            ret.push_back(step);
        }
    }

    return ret;
}

size_t part1(const steps_t& steps) {    
    return std::accumulate(steps.begin(), steps.end(), 0ull, [](auto& sum, auto& step){ return sum + hash(step.str); });
}

size_t focusing_power(const steps_t& steps, size_t box_id){
    size_t slot_n = 1;
    return std::accumulate(steps.begin(), steps.end(), 0ull, [&](auto& sum, auto& step){ return sum + (box_id * slot_n++ * step.focal_length); });
}

size_t part2(const steps_t& steps) 
{    
    std::vector<steps_t> boxes(256);

    for(auto& step : steps){
        if(step.operation == '=') {    
            auto it = std::find_if(boxes[step.box_id].begin(), boxes[step.box_id].end(), [&](auto& s){
                return s.label == step.label; 
            });
            if(it != boxes[step.box_id].end()){
                *it = step;
            }else{
                boxes[step.box_id].push_back(step);
            }
        }else{
            boxes[step.box_id].erase(std::remove_if(boxes[step.box_id].begin(), boxes[step.box_id].end(), [&](auto& s){
                return s.label == step.label; 
            }), boxes[step.box_id].end());
        }
    }

    size_t step_n = 1;
    return std::accumulate(boxes.begin(), boxes.end(), 0ull, [&](auto& sum, auto& steps){ return sum + focusing_power(steps, step_n++); });
}

void main()
{
    auto test_values = load_input("../src/day15/test_input.txt");
    auto actual_values = load_input("../src/day15/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}