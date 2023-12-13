#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>

struct pos_t{
    int x = 0; 
    int y = 0;
};

bool operator==(const pos_t& a, const pos_t& b){ return std::tuple(a.x,a.y) == std::tuple(b.x,b.y); }
pos_t& operator+=(pos_t& a, const pos_t& b){ a.x += b.x; a.y += b.y; return a; }

struct pos_hash {
    size_t operator()(const pos_t& pos) const {
        return (pos.x + pos.y + 1) * (pos.x + pos.y) / 2 + pos.y; // cantor
    }
};

struct pattern_t{
    std::unordered_set<pos_t,pos_hash> map;
    int width;
    int height;
};

using pattens_t = std::vector<pattern_t>;

pattens_t load_input(const std::string& file){
    pattens_t ret;
    std::ifstream fs(file);
    std::string line;

    int y = 0;
    ret.push_back(pattern_t());
    while (std::getline(fs, line)) {
        if(line.empty()){
            ret.push_back(pattern_t());
            y = 0;
            continue;
        }
        for(int x=0; x<line.size(); ++x){
            if(line[x] == '#'){
                ret.back().map.insert({x,y});
            }
        }
        y++;
        ret.back().width = (int)line.size();
        ret.back().height = y;
    }

    return ret;
}

int reflect(int relect_point, int pos){ return relect_point + (relect_point - pos - 1); }
int reflectable(int point, int extent){ return point >= 0 && point < extent; }

pos_t find_reflection(const pattern_t& pattern, int smudge_x, int smudge_y)
{
    pos_t ref;

    auto mirrored_in_y = [](const pattern_t& pattern, int y){
        for(auto& rock : pattern.map){
            pos_t mirrored = { rock.x, reflect(y, rock.y) };
            if(reflectable(mirrored.y, pattern.height) && !pattern.map.count(mirrored)){
                return false;
            }
        }
        return true;
    };

    for(int y=1; y<pattern.height; ++y){
        if(mirrored_in_y(pattern, y)){
            if(smudge_y != -1 && !reflectable(reflect(y, smudge_y), pattern.height)){
                continue; // only counts if smudge_y row is in reflection
            }
            ref.y = y;
            break;
        }
    }

    if(!ref.y)
    {
        auto mirrored_in_x = [](const pattern_t& pattern, int x){
            for(auto& rock : pattern.map){
                pos_t mirrored = { reflect(x, rock.x), rock.y };
                if(reflectable(mirrored.x, pattern.width) && !pattern.map.count(mirrored)){
                    return false;
                }
            }
            return true;
        };

        for(int x=1; x<pattern.width; ++x){
            if(mirrored_in_x(pattern, x)){
                if(smudge_x != -1 && !reflectable(reflect(x, smudge_x), pattern.width)){
                    continue; // only counts if smudge_x col is in reflection
                }
                ref.x = x;
                break;
            }
        }
    }

    return ref;
}

size_t part1(const pattens_t& patterns) 
{    
    pos_t sum;
    for(auto& pattern : patterns){
        sum += find_reflection(pattern, -1, -1);
    }
    return sum.x + 100 * sum.y;
}

size_t part2(pattens_t& patterns) 
{    
    auto find_new_reflection = [](pattern_t& pattern)
    {
        for(int y=0; y<pattern.height; ++y){
            for(int x=0; x<pattern.width; ++x){
                bool removed = false;
                bool inserted = false;
                if(pattern.map.count({x,y})){ 
                    pattern.map.erase({x,y});  // # -> .
                    removed = true;
                }else{
                    pattern.map.insert({x,y}); // . -> #
                    inserted = true;
                }

                pos_t ref = find_reflection(pattern, x, y);
                if(ref.x || ref.y){
                    return ref;
                }

                if(removed){
                    pattern.map.insert({x,y}); // change back . -> #
                }else if(inserted){
                    pattern.map.erase({x,y});  // change back # -> .
                }
            }
        }
        return pos_t{};
    };

    pos_t sum;
    for(auto& pattern : patterns){
        sum += find_new_reflection(pattern);
    }
    return sum.x + 100 * sum.y;
}

void main()
{
    auto test_values = load_input("../src/day13/test_input.txt");
    auto actual_values = load_input("../src/day13/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}