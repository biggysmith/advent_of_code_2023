#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <regex>
#include <optional>
#include "matrix.hpp"

std::vector<std::string> parse(const std::string& line, std::regex& reg) {
   return std::vector<std::string>(std::sregex_token_iterator(line.begin(), line.end(), reg), std::sregex_token_iterator());
}

struct pos_t {
    double x, y, z;
};

pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x + b.x, a.y + b.y, a.z + b.z }; }
pos_t operator-(const pos_t& a, const pos_t& b){ return { a.x - b.x, a.y - b.y, a.z - b.z }; }
pos_t operator*(const pos_t& a, double b){ return { a.x * b, a.y * b, a.z * b }; }

struct hailstone_t {
    pos_t pos;
    pos_t vel;
};

using hailstones_t = std::vector<hailstone_t>;

hailstones_t load_input(const std::string& file){
    hailstones_t ret;
    std::ifstream fs(file);
    std::string line;

    while (std::getline(fs, line)) {
        auto strs = parse(line, std::regex("[-\\d]+"));
        ret.push_back({{ (double)std::stoll(strs[0]), (double)std::stoll(strs[1]), (double)std::stoll(strs[2]) }, { (double)std::stoll(strs[3]), (double)std::stoll(strs[4]), (double)std::stoll(strs[5]) }});
    }

    return ret;
}

double cross_x(const pos_t& a, const pos_t& b){ return a.y*b.z - a.z*b.y; }
double cross_y(const pos_t& a, const pos_t& b){ return a.z*b.x - a.x*b.z; }
double cross_z(const pos_t& a, const pos_t& b){ return a.x*b.y - a.y*b.x; }

std::optional<pos_t> rays_intersection_2d(const hailstone_t& a, const hailstone_t& b) {
    pos_t d = b.pos - a.pos;
    double det = cross_z(b.vel, a.vel);
    if (det != 0) 
    {
        double u = cross_z(b.vel, d) / det; 
        double v = cross_z(a.vel, d) / det; 
        if (u >= 0 && v >= 0) {
            return a.pos + a.vel * u;
        }
    }
    return {};
}

bool within(double a, double mn, double mx){
    return a >= mn && a <= mx;
}

size_t part1(const hailstones_t& hailstones, double mn, double mx) 
{    
    size_t sum = 0;
    for(int i=0; i<hailstones.size(); ++i){
        for(int j=i+1; j<hailstones.size(); ++j){
            auto intersect = rays_intersection_2d(hailstones[i], hailstones[j]);
            if(intersect && within(intersect->x, mn, mx) && within(intersect->y, mn, mx)){
                sum++;
            }
        }
    }
    return sum;
}

size_t part2(const hailstones_t& hailstones) 
{    
    hailstone_t a = hailstones[0];
    hailstone_t b = hailstones[1];
    hailstone_t c = hailstones[2];

    pos_t ab_pos = a.pos - b.pos;
    pos_t ab_vel = a.vel - b.vel;

    pos_t ac_pos = a.pos - c.pos;
    pos_t ac_vel = a.vel - c.vel;

    dmat66 mat {{
        ab_vel.y, -ab_vel.x,  0.0,   -ab_pos.y,  ab_pos.x,  0.0,
        ac_vel.y, -ac_vel.x,  0.0,   -ac_pos.y,  ac_pos.x,  0.0,
        -ab_vel.z, 0.0, ab_vel.x,     ab_pos.z,  0.0, -ab_pos.x,
        -ac_vel.z, 0.0, ac_vel.x,     ac_pos.z,  0.0, -ac_pos.x,
        0.0, ab_vel.z, -ab_vel.y,     0.0, -ab_pos.z,  ab_pos.y,
        0.0, ac_vel.z, -ac_vel.y,     0.0, -ac_pos.z,  ac_pos.y
    }};

    dvec6 vec {
        cross_z(b.vel, b.pos) - cross_z(a.vel, a.pos), 
        cross_z(c.vel, c.pos) - cross_z(a.vel, a.pos), 
        cross_y(b.vel, b.pos) - cross_y(a.vel, a.pos),  
        cross_y(c.vel, c.pos) - cross_y(a.vel, a.pos), 
        cross_x(b.vel, b.pos) - cross_x(a.vel, a.pos), 
        cross_x(c.vel, c.pos) - cross_x(a.vel, a.pos)
    };

    dvec6 rock = inverse(mat) * vec;
    return (size_t)(rock.at(0) + rock.at(1) + rock.at(2));
}

void main()
{
    auto test_values = load_input("../src/day24/test_input.txt");
    auto actual_values = load_input("../src/day24/input.txt");

    std::cout << "part1: " << part1(test_values, 7, 27) << std::endl;
    std::cout << "part1: " << part1(actual_values, 200'000'000'000'000, 400'000'000'000'000) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}