#include <vector>
#include <iostream>
#include <fstream>
#include <regex>
#include <map>

struct score_t{
    size_t id;
    size_t matches;
};

using scores_t = std::vector<score_t>;

scores_t load_input(const std::string& file){
    scores_t scores;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        std::vector<int> my_numbers;
        std::vector<int> winning_numbers;
        std::regex word_regex("[\\d]+|[|]|[:]");
        bool reading_my_nums = false;
        bool reading_winning_nums = false;
        for(auto it=std::sregex_iterator(line.begin(), line.end(), word_regex); it!=std::sregex_iterator(); it++){
            if(it->str() == ":"){
                reading_my_nums = true;
            }else if(it->str() == "|"){
                reading_winning_nums = true;
                reading_my_nums = false;
            }else if(reading_my_nums){
                my_numbers.push_back(std::stoi(it->str()));
            }else if(reading_winning_nums){
                winning_numbers.push_back(std::stoi(it->str()));
            }
        }

        size_t matches = 0;
        for(auto& my_number : my_numbers){
            matches += std::count_if(winning_numbers.begin(), winning_numbers.end(), [&](auto& winning_number){
                return my_number == winning_number;
            });
        }
        scores.push_back({ scores.size(), matches });
    }
    return scores;
}

size_t get_score(const score_t& score, const std::vector<score_t>& orig_scores, std::map<size_t,size_t>& record){
    if(record.find(score.id) != record.end()){
        return record[score.id]; // already have this
    }

    size_t total_count = 0;
    size_t start = score.id + 1;
    for(size_t i=start; i<std::min(start+score.matches, orig_scores.size()); ++i) {
        total_count += get_score(orig_scores[i], orig_scores, record);
    }

    size_t total = total_count + score.matches;
    record[score.id] = total;
    return total;
}

size_t part1(const scores_t& scores)
{
    size_t sum = 0;
    for(auto& score : scores){
        sum += (1ull << score.matches) >> 1ull;
    }
    return sum;
}

size_t part2(const scores_t& scores)
{
    size_t total_count = 0;
    std::map<size_t, size_t> record;
    for(auto& score : scores){
        total_count += get_score(score, scores, record);
    }
    return total_count + scores.size();
}

void main()
{
    auto test_values = load_input("../src/day04/test_input.txt");
    auto actual_values = load_input("../src/day04/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}