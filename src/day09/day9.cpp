#include <vector>
#include <iostream>
#include <fstream>
#include <regex>

std::vector<std::string> parse(const std::string& line, std::regex& reg) {
   return std::vector<std::string>(std::sregex_token_iterator(line.begin(), line.end(), reg), std::sregex_token_iterator());
}

using report_t = std::vector<int64_t>;
using reports_t = std::vector<report_t>;

reports_t load_input(const std::string& file){
    reports_t reports;
    std::ifstream fs(file);
    std::string line;

    while (std::getline(fs, line)) {
        reports.push_back(report_t());
        auto strs = parse(line, std::regex("[-\\d]+"));
        for(auto& str : strs){
            reports.back().push_back(std::stoll(str));
        }
    }
    return reports;
}

int64_t process(const reports_t& reports, bool part1)
{
    int64_t sum = 0;
    for(auto& report : reports){
        std::vector<std::vector<int64_t>> lines;
        lines.push_back(report);
        bool all_zero = false;
        while(!all_zero){
            std::vector<int64_t> diffs(lines.back().size()-1);
            for(int i=1; i<lines.back().size(); ++i){
                diffs[i-1] = lines.back()[i] - lines.back()[i-1];
            }
            lines.push_back(diffs);
            all_zero = std::all_of(diffs.begin(), diffs.end(), [](auto& d){ return d==0; });
        }
        int64_t last = 0;
        for(int i=(int)lines.size()-2; i>=0; --i){
            last = part1 ? (last + lines[i].back()) : (lines[i].front() - last);
        }
        sum += last;
    }

    return sum;
}

void main()
{
    auto test_values = load_input("../src/day09/test_input.txt");
    auto actual_values = load_input("../src/day09/input.txt");

    std::cout << "part1: " << process(test_values, true) << std::endl;
    std::cout << "part1: " << process(actual_values, true) << std::endl;

    std::cout << "part2: " << process(test_values, false) << std::endl;
    std::cout << "part2: " << process(actual_values, false) << std::endl;
}