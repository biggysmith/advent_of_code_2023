#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <regex>
#include <unordered_map>
#include <numeric>

struct record_t{
    std::string springs;
    std::vector<size_t> groups;
};

struct pos_t{
    size_t spring_n, run_n, group_n;
};

bool operator==(const pos_t& a,const pos_t& b){
    return std::tuple(a.spring_n, a.run_n, a.group_n) == std::tuple(b.spring_n, b.run_n, b.group_n);
}

struct pos_hash{
    size_t operator()(const pos_t& pos) const {
        auto cantor = [](size_t a, size_t b){ return (a + b + 1) * (a + b) / 2 + b; };
        return cantor(pos.spring_n, cantor(pos.run_n, pos.group_n));
    }
};

using records_t = std::vector<record_t>;
using cache_t = std::unordered_map<pos_t,size_t,pos_hash>;

std::vector<std::string> parse(const std::string& line, std::regex& reg) {
   return std::vector<std::string>(std::sregex_token_iterator(line.begin(), line.end(), reg), std::sregex_token_iterator());
}

records_t load_input(const std::string& file){
    records_t records;
    std::ifstream fs(file);
    std::string line;

    while (std::getline(fs, line)) {
        records.push_back(record_t());
        records.back().springs = line.substr(0, line.find_first_of(' '));
        for(auto& group : parse(line.substr(line.find_first_of(' ')+1), std::regex("[\\d]+"))){
            size_t num = std::stoll(group);
            records.back().groups.push_back(num);
        }
    }

    return records;
}

bool damaged(char s) { return s == '#' || s == '?'; }
bool operational(char s) { return s == '.' || s == '?'; }

size_t arrangements(const record_t& record, size_t spring_n=0, size_t run_n=0, size_t group_n=0, cache_t& cache=cache_t())
{
    if(cache.count({spring_n, run_n, group_n})){
        return cache[{spring_n, run_n, group_n}];
    }

    if(spring_n == record.springs.size()){ // end of record
        bool valid_arrangement = (run_n == 0 && group_n == record.groups.size()) || (group_n == record.groups.size()-1 && record.groups[group_n] == run_n);
        return valid_arrangement;
    }

    size_t num_arrangements = 0;

    if(damaged(record.springs[spring_n])){ // continue run
        num_arrangements += arrangements(record, spring_n+1, run_n+1, group_n, cache);
    }

    if(operational(record.springs[spring_n])){
        if(run_n == 0){ // new run
            num_arrangements += arrangements(record, spring_n+1, 0, group_n, cache);
        }else if(group_n < record.groups.size() && record.groups[group_n] == run_n){ // valid new group
            num_arrangements += arrangements(record, spring_n+1, 0, group_n+1, cache);
        }
    }

    cache[{spring_n, run_n, group_n}] = num_arrangements;

    return num_arrangements;
}

size_t part1(const records_t& records) 
{    
    return std::accumulate(records.begin(), records.end(), 0ull, [](auto& sum, auto& record){
        return sum + arrangements(record);
    });
}

size_t part2(const records_t& records) 
{    
    records_t unfolded_records = records;
    for(int i=0; i<records.size(); ++i){
        for(int j=0; j<4; ++j){
            unfolded_records[i].springs.append(std::string("?") + records[i].springs);
            unfolded_records[i].groups.insert(unfolded_records[i].groups.end(), records[i].groups.begin(), records[i].groups.end());
        }
    }

    return std::accumulate(unfolded_records.begin(), unfolded_records.end(), 0ull, [](auto& sum, auto& record){
        return sum + arrangements(record);
    });
}

void main()
{
    auto test_values = load_input("../src/day12/test_input.txt");
    auto actual_values = load_input("../src/day12/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}