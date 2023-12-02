#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <cctype>
#include <map>

using calibration_value_t = std::vector<std::string>;

calibration_value_t load_input(const std::string& file){
    calibration_value_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(line);
    }
    return ret;
}

int part1(const calibration_value_t& calibrations)
{
    int sum = 0;
    for(auto& cal : calibrations){
        std::string numbers;
        std::copy_if(cal.begin(), cal.end(), std::back_inserter(numbers), [](auto c){
            return !std::isalpha(c); 
        });
        std::string number = std::string(1, numbers.front()) + std::string(1, numbers.back());
        sum += std::stoi(number);
    }
    return sum;
}

int part2(const calibration_value_t& calibrations)
{
    std::vector<std::string> num_strs = {
        "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"
    };

    std::map<std::string,std::string> num_map = {
        {"1","1"}, {"2","2"}, {"3","3"}, {"4","4"}, {"5","5"}, {"6","6"}, {"7","7"}, {"8","8"}, {"9","9"},
        {"one","1"}, {"two","2"}, {"three","3"}, {"four","4"}, {"five","5"}, {"six","6"}, {"seven","7"}, {"eight","8"}, {"nine","9"}
    };

    int sum = 0;
    for(auto& cal : calibrations){
        std::pair<std::size_t, std::string> lowest { UINT64_MAX, "" };
        std::pair<std::size_t, std::string> rhighest { 0ull, "" };
        for(auto& str : num_strs){
            std::size_t found = cal.find(str);
            if (found != std::string::npos) {
                if(found <= lowest.first){
                    lowest = { found, str };
                }
            }

            std::size_t rfound = cal.rfind(str);
            if (rfound != std::string::npos) {
                if(rfound >= rhighest.first){
                    rhighest = { rfound, str };
                }
            }
        }
        std::string number = num_map[lowest.second] + num_map[rhighest.second];
        sum += std::stoi(number);
    }
    return sum;
}

void main()
{
    auto test_values = load_input("../src/day01/test_input.txt");
    auto test_values2 = load_input("../src/day01/test_input2.txt");
    auto actual_values = load_input("../src/day01/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values2) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}