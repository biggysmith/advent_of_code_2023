#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>

enum hand_type_enum { e_five_of_a_kind=1, e_four_of_a_kind, e_full_house, e_three_of_a_kind, e_two_pair, e_one_pair, e_high_card };

using count_map_t = std::map<char,int>;
using rank_map_t = std::map<char,int>;

count_map_t get_map(const std::string& cards){
    count_map_t map;
    for(auto& c : cards){
        map[c]++;
    }
    return map;
}

struct hand_t{
    std::string cards;
    std::string orig;
    hand_type_enum rank;
    int bid;
};

bool has_value(const count_map_t& map, int value){
    return std::find_if(map.begin(), map.end(), [&value](auto& kv) {return kv.second == value; }) != map.end();
}

using hands_t = std::vector<hand_t>;

hand_type_enum rank_hand(const std::string& cards){
    auto map = get_map(cards);
    if(map.size() == 1){
        return e_five_of_a_kind;
    }else if(map.size() == 2 && has_value(map, 4)){
        return e_four_of_a_kind;
    }else if(map.size() == 2 && has_value(map, 2)){
        return e_full_house;
    }else if(map.size() == 3 && has_value(map, 3)){
        return e_three_of_a_kind;
    }else if(map.size() == 3 && has_value(map, 1)){
        return e_two_pair;
    }else if(map.size() == 4){
        return e_one_pair;
    }else if(map.size() == 5){
        return e_high_card;
    }else{
        return e_high_card;
    }
}

hands_t load_input(const std::string& file){
    hands_t hands;
    std::ifstream fs(file);
    std::string line;

    while (std::getline(fs, line)) {
        std::string str = line.substr(0, 5);
        hands.push_back({ str, str, rank_hand(str), std::stoi(line.substr(6)) });
    }

    return hands;
}

bool compare(const hand_t& a, const hand_t& b, const rank_map_t& ranks){
    if(a.rank == b.rank){
        for(int i=0; i<5; ++i){
            if(ranks.at(a.cards[i]) != ranks.at(b.cards[i])){
                return ranks.at(a.cards[i]) < ranks.at(b.cards[i]);
            }
        }
    }
    return a.rank > b.rank;    
}

size_t part1(hands_t hands)
{
    rank_map_t card_rank{ { '1', 1 }, { '2', 2 }, { '3', 3 }, { '4', 4 }, { '5', 5 }, { '6', 6 }, { '7', 7 }, { '8', 8 }, { '9', 9 }, { 'T', 10 }, { 'J', 11 }, { 'Q', 12 }, { 'K', 13 }, { 'A', 14 } };

    std::sort(hands.begin(), hands.end(), [&](auto& a, auto& b){
        return compare(a, b, card_rank);
    });

    int sum = 0;
    for(int i=0; i<hands.size(); ++i){
        sum += (i+1) * hands[i].bid;
    }
    return sum;
}


bool compare2(const hand_t& a, const hand_t& b, const rank_map_t& ranks){
    if(a.rank == b.rank){
        for(int i=0; i<5; ++i){
            if(ranks.at(a.orig[i]) != ranks.at(b.orig[i])){
                return ranks.at(a.orig[i]) < ranks.at(b.orig[i]);
            }
        }
    }
    return a.rank > b.rank;    
}

hand_t find_best(const hand_t& hand, int s, hand_t& highest, const rank_map_t& ranks){
    for(int i=s; i<5; ++i){
        if(hand.orig[i] == 'J'){
            for(auto&& [card, _] : ranks){
                hand_t new_hand = hand;
                new_hand.cards[i] = card;
                new_hand.rank = rank_hand(new_hand.cards);
                hand_t local_best = find_best(new_hand, i+1, highest, ranks);
                if(compare(highest, local_best, ranks)){
                    highest = local_best;
                }
            }
        }
    }
    if(compare(highest, hand, ranks)){
        highest = hand;
    }
    return highest;
}

size_t part2(hands_t hands)
{
    rank_map_t card_rank{ { '1', 1 }, { '2', 2 }, { '3', 3 }, { '4', 4 }, { '5', 5 }, { '6', 6 }, { '7', 7 }, { '8', 8 }, { '9', 9 }, { 'T', 10 }, { 'J', 0 }, { 'Q', 12 }, { 'K', 13 }, { 'A', 14 } };

    for(auto& hand : hands){
        hand = find_best(hand, 0, hand, card_rank);
    }

    std::sort(hands.begin(), hands.end(), [&](auto& a, auto& b){
        return compare2(a, b, card_rank); 
    });

    int sum = 0;
    for(int i=0; i<hands.size(); ++i){
        sum += (i+1) * hands[i].bid;
    }
    return sum;
}

void main()
{
    auto test_values = load_input("../src/day07/test_input.txt");
    auto actual_values = load_input("../src/day07/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}