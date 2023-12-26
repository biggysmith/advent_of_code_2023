#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <regex>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/stoer_wagner_min_cut.hpp>

typedef boost::property<boost::edge_weight_t, size_t> edge_weight_t;
typedef boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::undirectedS,
    boost::no_property,
    edge_weight_t,
    boost::no_property
> graph_t;

std::vector<std::string> parse(const std::string& line, std::regex& reg) {
   return std::vector<std::string>(std::sregex_token_iterator(line.begin(), line.end(), reg), std::sregex_token_iterator());
}

graph_t load_input(const std::string& file){
    graph_t ret;
    std::ifstream fs(file);
    std::string line;

    int n = 0;
    std::map<std::string,int> name_map; // name xyz -> int

    while (std::getline(fs, line)) {
        auto strs = parse(line, std::regex("[\\w]+"));
        if(!name_map.count(strs[0])){
            name_map[strs[0]] = n++;
        }
        for(int i=1; i<strs.size(); ++i){
            if(!name_map.count(strs[i])){
                name_map[strs[i]] = n++;
            }
            boost::add_edge(name_map[strs[0]], name_map[strs[i]], edge_weight_t(1), ret);
        }
    }

    return ret;
}

size_t part1(graph_t graph) 
{    
    // parity_map stores parity value for each vertex. vertices with different parity will be on different side of min cut 
    auto parity = boost::make_one_bit_color_map(boost::num_vertices(graph), boost::get(boost::vertex_index, graph));

    auto weights = boost::get(boost::edge_weight, graph);
    boost::stoer_wagner_min_cut(graph, weights, boost::parity_map(parity));

    for(auto& ed : boost::make_iterator_range(boost::edges(graph))) {
        auto src = boost::source(ed, graph);
        auto dst = boost::target(ed, graph);
        if (boost::get(parity, src) != boost::get(parity, dst)){
            boost::remove_edge(src, dst, graph);
        }
    }

    std::vector<int> component(boost::num_vertices(graph));
    boost::connected_components(graph, component.data());

    return std::count(component.begin(), component.end(), 0) * std::count(component.begin(), component.end(), 1);
}

void main()
{
    auto test_values = load_input("../src/day25/test_input.txt");
    auto actual_values = load_input("../src/day25/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << "Merry Christmas!" << std::endl;
    std::cout << "part2: " << "PUSH THE BUTTON!" << std::endl;
}