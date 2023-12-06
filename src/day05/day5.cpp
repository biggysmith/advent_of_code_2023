#include <vector>
#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <optional>

#include <boost/icl/interval_set.hpp>

typedef boost::icl::interval_set<size_t> set_t;
typedef set_t::interval_type ival;

struct range_t{
    ival dst;
    ival src;
};

struct map_t{
    std::string dst_type;
    std::string src_type;
    std::vector<range_t> ranges;
};

struct almanac_t{
    std::vector<size_t> seeds;
    std::vector<map_t> maps;
};

std::vector<std::string> parse(const std::string& line, std::regex& reg) {
   return std::vector<std::string>(std::sregex_token_iterator(line.begin(), line.end(), reg), std::sregex_token_iterator());
}

almanac_t load_input(const std::string& file){
    almanac_t almanac;
    std::ifstream fs(file);
    std::string line;
    int y = 0;
    while (std::getline(fs, line)) {
        if(y == 0){
            auto strs = parse(line, std::regex("[\\d]+"));
            for(auto& str : strs){
                almanac.seeds.push_back(std::stoull(str));
            }
        }else{
            if(!line.empty()){
                if(::isalpha(line[0])){
                    map_t map;
                    auto strs = parse(line, std::regex("[\\w]+"));
                    map.dst_type = strs[2];
                    map.src_type = strs[0];
                    almanac.maps.push_back(map);
                }else{
                    range_t ranges;
                    auto strs = parse(line, std::regex("[\\d]+"));
                    size_t dst = std::stoull(strs[0]);
                    size_t src = std::stoull(strs[1]);
                    size_t range = std::stoull(strs[2]);
                    ranges.dst = ival::right_open(dst, dst+range);
                    ranges.src = ival::right_open(src, src+range);
                    almanac.maps.back().ranges.push_back(ranges);
                }
            }
        }
        y++;
    }
    return almanac;
}

size_t part1(const almanac_t& almanac)
{
    size_t min_val = UINT64_MAX;
    for(auto& seed : almanac.seeds){
        size_t next = seed;
        for(auto& map : almanac.maps){
            for(auto& range : map.ranges){
                if(boost::icl::contains(range.src, next)){
                    next = range.dst.lower() + (next - range.src.lower());
                    break;
                }
            }
            
        }
        min_val = std::min(min_val, next);
    }

    return min_val;
}

size_t part2(const almanac_t& almanac)
{
    set_t interval_set;
    for(int s=0; s<almanac.seeds.size(); s+=2){
        auto seed_src = almanac.seeds[s];
        auto seed_range = almanac.seeds[s+1];
        ival range(ival::right_open(seed_src, seed_src+seed_range));
        interval_set.insert(range);
    }

    size_t min_v = UINT64_MAX;

    for(auto& map : almanac.maps){
        set_t mapped_set;
        set_t unmapped_set;
            
        for(auto& range : interval_set){
            unmapped_set.insert(range);
        }

        for(auto& range : map.ranges){
            for(auto& interval : interval_set){
                ival inter = interval & range.src;
                if(boost::icl::size(inter)){
                    ival dst_range(ival::right_open(
                        range.dst.lower() + (inter.lower() - range.src.lower()),
                        range.dst.lower() + (inter.upper() - range.src.lower())
                    ));
                    mapped_set.insert(dst_range);
                    unmapped_set.erase(inter);
                }
            }

        }

        interval_set.clear();
        for(auto& range : mapped_set){
            interval_set.insert(range);
        }
        for(auto& range : unmapped_set){
            interval_set.insert(range);
        }
    }

    for(auto& range : interval_set){
        min_v = std::min(min_v, range.lower());
    }

    return min_v;
}

void main()
{
    auto test_values = load_input("../src/day05/test_input.txt");
    auto actual_values = load_input("../src/day05/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}