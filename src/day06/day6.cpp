#include <vector>
#include <iostream>
#include <fstream>
#include <regex>

std::vector<std::string> parse(const std::string& line, std::regex& reg) {
   return std::vector<std::string>(std::sregex_token_iterator(line.begin(), line.end(), reg), std::sregex_token_iterator());
}

struct race_t{
    int time;
    int dist;
};

using races_t = std::vector<race_t>;

races_t load_input(const std::string& file){
    races_t races;
    std::ifstream fs(file);
    std::string line;

    std::getline(fs, line);
    auto time_strs = parse(line, std::regex("[\\d]+"));
    std::getline(fs, line);
    auto dist_strs = parse(line, std::regex("[\\d]+"));

    for(int i=0; i<time_strs.size(); ++i){
        races.push_back({ std::stoi(time_strs[i]), std::stoi(dist_strs[i]) });
    }

    return races;
}

size_t number_of_ways_to_win(size_t time, size_t dist){
    size_t winning_ways = 0;
    for(size_t velocity=0; velocity<time; ++velocity){
        size_t time_left = time - velocity;
        if(time_left * velocity > dist){
            winning_ways++;
        }
    }
    return winning_ways;
}

size_t part1(const races_t& races)
{
    size_t product = 1;
    for(auto& race : races){
        product *= number_of_ways_to_win(race.time, race.dist);
    }
    return product;
}

size_t part2(const races_t& races)
{
    std::string time_str;
    std::string dist_str;
    for(auto& race : races){
        time_str += std::to_string(race.time);
        dist_str += std::to_string(race.dist);
    } 
    int64_t time = std::stoll(time_str);
    int64_t dist = std::stoll(dist_str);

    return number_of_ways_to_win(std::stoll(time_str), std::stoll(dist_str));
    //return (size_t)sqrt((time*time) - 4*(dist+1)); // could return quadratic solution, but I like CPU GO BRRRRRRRR
}

void main()
{
    auto test_values = load_input("../src/day06/test_input.txt");
    auto actual_values = load_input("../src/day06/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}