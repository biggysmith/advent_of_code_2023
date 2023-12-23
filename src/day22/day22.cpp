#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <regex>

std::vector<std::string> parse(const std::string& line, std::regex& reg) {
   return std::vector<std::string>(std::sregex_token_iterator(line.begin(), line.end(), reg), std::sregex_token_iterator());
}

struct pos_t {
    //float x, y, z;
    float x() const { return d[0]; }
    float y() const { return d[1]; }
    float z() const { return d[2]; }
    float operator[](int i) const { return d[i]; };
    float d[3];
};

/*
bool operator==(const pos_t& a, const pos_t& b){ 
    return std::tuple(std::abs(a.x()-b.x()), std::abs(a.y()-b.y()), std::abs(a.z()-b.z())) < std::tuple(0.001f, 0.001f, 0.001f); 
}*/
bool operator==(const pos_t& a, const pos_t& b){ return std::tuple(a.x(), a.y(), a.z()) == std::tuple(b.x(), b.y(), b.z()); }
pos_t operator+(const pos_t& a, const pos_t& b){ return { a.x() + b.x(), a.y() + b.y(), a.z() + b.z() }; }
pos_t operator-(const pos_t& a, const pos_t& b){ return { a.x() - b.x(), a.y() - b.y(), a.z() - b.z() }; }
pos_t operator*(const pos_t& a, int b){ return { a.x() * b, a.y() * b, a.z() * b }; }

struct box_t {
    pos_t mn, mx;
};

bool operator==(const box_t& a, const box_t& b){ return std::tuple(a.mn, a.mx) == std::tuple(b.mn, b.mx); }

/*
bool intersecting(const box_t& a, const box_t& b){
    if (a.mx.x < b.mn.x || a.mn.x > b.mx.x) return false;
    if (a.mx.y < b.mn.y || a.mn.y > b.mx.y) return false;
    if (a.mx.z < b.mn.z || a.mn.z > b.mx.z) return false;
    return true;
}*/

bool moving_intersect(const box_t& a, const box_t& b, const pos_t& va, const pos_t& vb, float& tfirst, float& tlast)
{
    // Exit early if ‘a’ and ‘b’ initially overlapping
    /*if (intersecting(a, b)) {
        tfirst = tlast = 0;
        return true;
    }*/

    /*bool overlap_x = !(a.mx.x < b.mn.x || a.mn.x > b.mx.x);
    bool overlap_y = !(a.mx.y < b.mn.y || a.mn.y > b.mx.y);
    bool overlap_z = !(a.mx.z < b.mn.z || a.mn.z > b.mx.z);*/

    bool overlap[3] {
        !(a.mx[0] < b.mn[0] || a.mn[0] > b.mx[0]),
        !(a.mx[1] < b.mn[1] || a.mn[1] > b.mx[1]),
        !(a.mx[2] < b.mn[2] || a.mn[2] > b.mx[2])
    };

    if (overlap[0] && overlap[1] && overlap[2]) {
        return true;  // AABBs are already overlapping
    }

    // Use relative velocity; effectively treating ’a’ as stationary
    pos_t v = vb - va;

    // Initialize times of first and last contact
    //tfirst = 0;
    //tlast = 1;

    tfirst = 0;
    tlast = 2000;

    for(int i=0; i<3; ++i){
        if (v[i] < 0) {
            if (b.mx[i] < a.mn[i]) return false;
            if (a.mx[i] < b.mn[i]) tfirst = std::max((a.mx[i] - b.mn[i]) / v[i], tfirst);
            if (b.mx[i] > a.mn[i]) tlast = std::min((a.mn[i] - b.mx[i]) / v[i], tlast);
        }
        else if (v[i] > 0) {
            if (b.mn[i] > a.mx[i]) return false;
            if (b.mx[i] < a.mn[i]) tfirst = std::max((a.mn[i] - b.mx[i]) / v[i], tfirst);
            if (a.mx[i] > b.mn[i]) tlast = std::min((a.mx[i] - b.mn[i]) / v[i], tlast);
        }else {
            if(!overlap[i]){
                return false;
            }
        }
        if (tfirst > tlast) {
            return false;
        }
    }

    /*if (v.x < 0) {
        if (b.mx.x < a.mn.x) return false;
        if (a.mx.x < b.mn.x) tfirst = std::max((a.mx.x - b.mn.x) / v.x, tfirst);
        if (b.mx.x > a.mn.x) tlast = std::min((a.mn.x - b.mx.x) / v.x, tlast);
    }
    else if (v.x > 0) {
        if (b.mn.x > a.mx.x) return false;
        if (b.mx.x < a.mn.x) tfirst = std::max((a.mn.x - b.mx.x) / v.x, tfirst);
        if (a.mx.x > b.mn.x) tlast = std::min((a.mx.x - b.mn.x) / v.x, tlast);
    }else{
        if(!overlap_x){
            return false;
        }
    }
    if (tfirst > tlast) {
        return false;
    }

    if (v.y < 0) {
        if (b.mx.y < a.mn.y) return false;
        if (a.mx.y < b.mn.y) tfirst = std::max((a.mx.y - b.mn.y) / v.y, tfirst);
        if (b.mx.y > a.mn.y) tlast = std::min((a.mn.y - b.mx.y) / v.y, tlast);
    }
    else if (v.y > 0) {
        if (b.mn.y > a.mx.y) return false;
        if (b.mx.y < a.mn.y) tfirst = std::max((a.mn.y - b.mx.y) / v.y, tfirst);
        if (a.mx.y > b.mn.y) tlast = std::min((a.mx.y - b.mn.y) / v.y, tlast);
    }else{
        if(!overlap_y){
            return false;
        }
    }
    if (tfirst > tlast) {
        return false;
    }

    if (v.z < 0) {
        if (b.mx.z < a.mn.z) return false;
        if (a.mx.z < b.mn.z) tfirst = std::max((a.mx.z - b.mn.z) / v.z, tfirst);
        if (b.mx.z > a.mn.z) tlast = std::min((a.mn.z - b.mx.z) / v.z, tlast);
    }
    else if (v.z > 0) {
        if (b.mn.z > a.mx.z) return false;
        if (b.mx.z < a.mn.z) tfirst = std::max((a.mn.z - b.mx.z) / v.z, tfirst);
        if (a.mx.z > b.mn.z) tlast = std::min((a.mx.z - b.mn.z) / v.z, tlast);
    }else{
        if(!overlap_z){
            return false;
        }
    }
    if (tfirst > tlast) {
        return false;
    }*/

    /*if(tfirst < 1.0f){
        return false;
    }*/

    return true;
}


box_t operator+(const box_t& a, const pos_t& b) { return { a.mn + b, a.mx + b }; }

using boxes_t = std::vector<box_t>;

boxes_t load_input(const std::string& file){
    boxes_t ret;
    std::ifstream fs(file);
    std::string line;

    while (std::getline(fs, line)) {
        auto strs = parse(line, std::regex("[\\d]+"));
        ret.push_back({{ (float)std::stoi(strs[0]), (float)std::stoi(strs[1]), (float)std::stoi(strs[2]) }, { (float)std::stoi(strs[3])+0.99f, (float)std::stoi(strs[4])+0.99f, (float)std::stoi(strs[5])+0.99f }});
        //ret.push_back({{ (float)std::stoi(strs[0]), (float)std::stoi(strs[1]), (float)std::stoi(strs[2]) }, { (float)std::stoi(strs[3])+1.0f-FLT_EPSILON, (float)std::stoi(strs[4])+1.0f-FLT_EPSILON, (float)std::stoi(strs[5])+1.0f-FLT_EPSILON }});
    }

    std::sort(ret.begin(), ret.end(), [](auto& a, auto& b){
        //return a.mn.z() < b.mn.z(); 
        //return std::tuple(a.mn.z(), a.mn.y(), a.mn.x()) < std::tuple(b.mn.z(), b.mn.y(), b.mn.x()); 
        return std::tuple(a.mn.z()) < std::tuple(b.mn.z()); 
        //return (a.mn.z() + a.mn.y() + a.mn.x()) < (b.mn.z() + b.mn.y() + b.mn.x()); 
    });

    return ret;
}

std::ostream& operator<<(std::ostream& os, const pos_t& b){
    os << b.x() << ", " << b.y() << ", " << b.z();
    return os;
}

std::ostream& operator<<(std::ostream& os, const box_t& b){
    os << b.mn << " - " << b.mx;
    return os;
}

void print(const boxes_t& boxes){
    std::vector<char> xz(400*400, '.');
    std::vector<char> yz(400*400, '.');

    auto set = [](std::vector<char>& grid, int x, int y, char c){
        grid[y*400+x] = c;
    };

    char letter = 'a';
    for(auto& box : boxes){
        for(int z=box.mx.z(); z>=box.mn.z(); --z){
            for(int y=box.mn.y(); y<=box.mx.y(); ++y){
                set(yz, y, z, letter);
            }

            for(int x=box.mn.x(); x<=box.mx.x(); ++x){
                set(xz, x, z, letter);
            }
        }
        letter++;
    }

    for(int z=399; z>=0; --z){
        for(int x=0; x<400; ++x){
            std::cout << xz[z*400 + x];
        }
        std::cout << " " << z << std::endl;
    }

    std::cout << std::endl;
    for(int z=399; z>=0; --z){
        for(int y=0; y<400; ++y){
            std::cout << yz[z*400 + y];
        }
        std::cout << " " << z << std::endl;
    }
    std::cout << std::endl;


}

pos_t down = { 0, 0, -1 };
pos_t zero = { 0, 0, 0 };

size_t part1(const boxes_t& orig_boxes) 
{    

    //std::cout << std::endl;
    //print(boxes);

    //int z = 0;

    auto fall = [&](const boxes_t& boxes)
    {
        boxes_t new_boxes = boxes;
        //new_boxes.push_back(boxes.front() + pos_t{ 0, 0, boxes.front().mn.z });

        for(int i=0; i<boxes.size(); ++i)
        {
            auto& box_a = boxes[i];
            bool collision = false;
            //for(int j=0; j<i; ++j)
            for(int j=i-1; j>=0; --j)
            //for(int j=boxes.size()-1; j>=0; --j)
            {
                /*if(box_a.mn.z() == new_boxes[j].mn.z()){
                    continue;
                }*/

                float first, last;
                if(moving_intersect(new_boxes[j], box_a, zero, down, first, last)){
                    //collision = true;

                    //std::cout << (char)('A'+i) << " ( " << box_a << " ) x " << (char)('A'+j) << " ( " << new_boxes[j] << " ) " << " -->> ";
                    //new_boxes[i] = box_a + down * std::round(first);
                    //new_boxes[i] = box_a + down * std::floor(first);

                    /*auto& box_b =  new_boxes[j];
                    if(first != box_a.mn.z() - new_boxes[j].mx.z()){
                        std::cout << first << " - " << std::floor(first) << " - " << box_a.mn.z() - box_b.mx.z() + 1 << std::endl;
                        continue;
                    }*/

                    /*if(first > 0){
                        continue;
                    }*/

                    new_boxes[i] = box_a + down * std::floor(std::max(0.0f, first));

                    collision = true;

                    //std::cout << new_boxes[i] << std::endl;
                    //print(new_boxes);
                    break;
                }
            }

            if(!collision){
                //std::cout << (char)('A'+i) << " ( " << box_a << " ) " << " -->> ";
                new_boxes[i] = box_a + pos_t{ 0, 0, -new_boxes[i].mn.z()+1 };
                //std::cout << new_boxes[i] << std::endl;
            }
        }

        return new_boxes;
    };

    auto settled_bricks = fall(orig_boxes);

    //std::cout << std::endl;
    //print(settled_bricks);

    size_t sum = 0;
    for(int i=0; i<settled_bricks.size(); ++i){
        boxes_t new_boxes = settled_bricks;
        new_boxes.erase(new_boxes.begin() + i);

        //print(new_boxes);
        boxes_t new_settled_boxes = fall(new_boxes);
        //print(new_settled_boxes);

        bool same = true;
        for(int j=0; j<new_boxes.size(); ++j){
            auto& a = new_settled_boxes[j];
            auto& b = new_boxes[j];
            same &= a == b;
        }

        bool stable = same;
        sum += stable;
    }

    return sum;
}

size_t part2(const boxes_t& boxes) 
{    
    return 0;
}

void main()
{
    auto test_values = load_input("../src/day22/test_input.txt");
    auto actual_values = load_input("../src/day22/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    //std::cout << "part2: " << part2(test_values) << std::endl;
    //std::cout << "part2: " << part2(actual_values) << std::endl;
}

// 415 too high
// 402 ~
// 411 ~
// 406 ~
// 405 ~

// 395 is correct

// 116 too low
