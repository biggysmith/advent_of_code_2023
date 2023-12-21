#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <regex>
#include <map>
#include <queue>
#include <numeric>

std::vector<std::string> parse(const std::string& line, std::regex& reg) {
   return std::vector<std::string>(std::sregex_token_iterator(line.begin(), line.end(), reg), std::sregex_token_iterator());
}

enum power_t { e_off=0, e_on };
enum pulse_t { e_low=0, e_high };

struct in_t{
    std::string name;
    pulse_t pulse;
};

struct out_t {
    std::string name;
};

struct module_t {
    char prefix;
    std::string name;
    std::map<std::string,in_t> in_nodes;
    std::vector<out_t> out_nodes;
    power_t power = e_off;
};

using modules_t = std::map<std::string,module_t>;

modules_t load_input(const std::string& file){
    modules_t ret;
    std::ifstream fs(file);
    std::string line;

    while (std::getline(fs, line)) {
        auto strs = parse(line, std::regex("[%]|[&]|[\\w]+"));
        char prefix = strs[0][0];
        int i = 1;
        std::string name = prefix != 'b' ? strs[i++] : "broadcast";
        ret[name].name = name;
        ret[name].prefix = strs[0][0];
        for(; i<strs.size(); ++i){
            std::string out = strs[i];
            ret[name].out_nodes.push_back({out});
            ret[out].in_nodes[name] = { name, e_low };
        }
    }
    return ret;
}

struct queue_t{
    std::string prev_name;
    std::string name;
    pulse_t pulse;
};

size_t process(modules_t modules, bool part2) 
{    
    size_t low_pulses = 0;
    size_t high_pulses = 0;

    std::string node_to_rx;
    std::map<std::string, size_t> cycles;
    if(part2) {
        node_to_rx = modules.at("rx").in_nodes.begin()->first;
        for(auto& node : modules.at(node_to_rx).in_nodes){ // find the counter nodes that run into the node that runs into rx
            cycles[node.first] = 0;
        }
    }

    int limit = part2 ? INT_MAX : 1000;
    for(int b=0; b<limit; ++b)
    {
        std::queue<queue_t> q;
        q.push({ "button", "broadcast", e_low });
        low_pulses++;

        while (!q.empty()) 
        {
            auto curr = q.front();
            q.pop();

            if(part2)
            {
                for(auto& [in_node, count] : cycles){
                    if(curr.prev_name == in_node && curr.name == node_to_rx && curr.pulse == e_high){
                        count = b+1 - count; // found a cycle
                    }
                }
            }

            auto& module = modules.at(curr.name);

            if(module.name == "broadcast"){
                for(auto& out : module.out_nodes){
                    q.push({ curr.name, out.name, curr.pulse });
                    curr.pulse == e_low ? low_pulses++ : high_pulses++;
                }
            }else if(module.prefix == '%'){
                if(curr.pulse == e_low){
                    module.power = module.power == e_on ? e_off : e_on;
                    for(auto& out : module.out_nodes){
                        q.push({ curr.name, out.name, module.power == e_on ? e_high : e_low });
                        module.power == e_on ? high_pulses++ : low_pulses++;
                    }
                }
            }else if(module.prefix == '&'){
                module.in_nodes[curr.prev_name].pulse = curr.pulse;
                bool all_high = std::all_of(module.in_nodes.begin(), module.in_nodes.end(), [](auto& node){ return node.second.pulse == e_high; });
                for(auto& out : module.out_nodes){
                    q.push({ curr.name, out.name, all_high ? e_low : e_high });
                    all_high ? low_pulses++ : high_pulses++;
                }
            }
        }

        if(part2) {
            bool found_all_cycles = std::none_of(cycles.begin(), cycles.end(), [](auto& cycle) { return cycle.second == 0; });
            if(found_all_cycles){
                return std::accumulate(cycles.begin(), cycles.end(), 1ull, [](auto& a, auto& b){
                    return std::lcm(a, b.second);    
                });
            }
        }
    }

    return low_pulses * high_pulses;
}

void main()
{
    auto test_values0 = load_input("../src/day20/test_input0.txt");
    auto test_values1 = load_input("../src/day20/test_input1.txt");
    auto actual_values = load_input("../src/day20/input.txt");

    std::cout << "part1: " << process(test_values0, false) << std::endl;
    std::cout << "part1: " << process(test_values1, false) << std::endl;
    std::cout << "part1: " << process(actual_values, false) << std::endl;
    std::cout << "part2: " << process(actual_values, true) << std::endl;
}