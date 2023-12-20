#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <regex>
#include <map>

std::vector<std::string> parse(const std::string& line, std::regex& reg) {
   return std::vector<std::string>(std::sregex_token_iterator(line.begin(), line.end(), reg), std::sregex_token_iterator());
}

struct rule_t {
    char left;
    char op;
    int right;
    std::string result;
};

struct workflow_t {
    std::string name;
    std::vector<rule_t> rules;
    std::string end;
};

using workflow_map_t = std::map<std::string, workflow_t>;
using rating_map_t = std::map<char,int>;

struct workflows_and_parts_t {
    workflow_map_t workflows;
    std::vector<rating_map_t> ratings;
};

workflows_and_parts_t load_input(const std::string& file){
    workflows_and_parts_t ret;
    std::ifstream fs(file);
    std::string line;
    bool parsing_parts = false;

    while (std::getline(fs, line)) {
        if(line.empty()){
            parsing_parts = true;
            continue;
        }

        if(parsing_parts){
            auto strs = parse(line, std::regex("[\\w]+|[=]"));
            rating_map_t parts;
            for(int i=0; i<strs.size(); i+=3){
                parts[strs[i][0]] = std::stoi(strs[i+2]);
            }
            ret.ratings.push_back(parts);
        }else{
            workflow_t workflow;
            auto strs = parse(line, std::regex("[\\w]+|[<]|[>]"));
            workflow.name = strs[0];
            for(int i=1; i<strs.size()-1; i+=4){
                rule_t rule;
                rule.left = strs[i][0];
                rule.op = strs[i+1][0];
                rule.right = std::stoi(strs[i+2]);
                rule.result = strs[i+3];
                workflow.rules.push_back(rule);
            }
            workflow.end = strs.back();
            ret.workflows[workflow.name] = workflow;
        }

    }

    return ret;
}

bool op(int left, char op, int right){
    return op=='<' ? (left < right) : (left > right);
}

void sort_thru(const workflow_map_t& workflows, const rating_map_t& parts, const std::string& name, int idx, size_t& sum)
{
    if(name == "A"){
        sum += parts.at('x') + parts.at('m') + parts.at('a') + parts.at('s');
        return;
    }else if(name == "R"){
        return;
    }

    if(idx == workflows.at(name).rules.size()){
        sort_thru(workflows, parts, workflows.at(name).end, 0, sum);
    }else{
        auto& rule = workflows.at(name).rules[idx];
        if(op(parts.at(rule.left), rule.op, rule.right)){
            sort_thru(workflows, parts, rule.result, 0, sum);
        }else{
            sort_thru(workflows, parts, name, idx+1, sum);
        }

    }
}

size_t part1(const workflows_and_parts_t& info) 
{    
    size_t sum = 0;
    for(auto& rating_map : info.ratings){
        sort_thru(info.workflows, rating_map, "in", 0, sum);
    }
    return sum;
}

struct interval_t{  
    size_t range() const { return b - a + 1; }
    void intersect(const interval_t& other) { a = std::max(a, other.a); b = std::min(b, other.b); };
    int a, b;
};

using xmas_map_t = std::map<char, interval_t>;

void sort_thru_all(const xmas_map_t& xmas_map, const workflow_map_t& workflows, const std::string& name, int idx, size_t& sum)
{
    if(name == "A"){
        sum += xmas_map.at('x').range() * xmas_map.at('m').range() * xmas_map.at('a').range() * xmas_map.at('s').range();
        return;
    }else if(name == "R"){
        return;
    }

    if(idx == workflows.at(name).rules.size()){
        sort_thru_all(xmas_map, workflows, workflows.at(name).end, 0, sum);
    }else{
        auto& rule = workflows.at(name).rules[idx];
        bool less_op = rule.op == '<';

        xmas_map_t map0 = xmas_map;
        map0[rule.left].intersect(less_op ? interval_t{ 1, rule.right-1 } : interval_t{ rule.right+1, 4000 });
        sort_thru_all(map0, workflows, rule.result, 0, sum);

        xmas_map_t map1 = xmas_map;
        map1[rule.left].intersect(less_op ? interval_t{ rule.right, 4000 } : interval_t{ 1, rule.right });
        sort_thru_all(map1, workflows, name, idx+1, sum);
    }
}

size_t part2(const workflows_and_parts_t& info) 
{    
    size_t sum = 0;

    xmas_map_t xmas_map = { { 'x', { 1, 4000 } }, { 'm', { 1, 4000 } }, { 'a', { 1, 4000 } }, { 's', { 1, 4000 } } };
    sort_thru_all(xmas_map, info.workflows, "in", 0, sum);

    return sum;
}

void main()
{
    auto test_values = load_input("../src/day19/test_input.txt");
    auto actual_values = load_input("../src/day19/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}