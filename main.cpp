#include <string>
#include "tim_sort.hpp"
#include "avl_tree.hpp"
#include "sets_sys.hpp"

typedef unsigned int value_type;

// need to change timSort function (it must use only < and ==)
struct Edge
{
    std::string from;
    std::string to;
    value_type weight;

    Edge(std::string&& from, std::string&& to, value_type weight):
        from(std::move(from)), to(std::move(to)), weight(weight)
    {}
    Edge() = default;
    Edge(const Edge&) = default;
    Edge(Edge&&) = default;
    Edge& operator=(const Edge& other) = default;
    bool operator==(const Edge& other) const
    {
        return weight == other.weight;
    }
    bool operator!=(const Edge& other) const
    {
        return !(*this == other);
    }
    bool operator<(const Edge& other) const
    {
        return weight < other.weight;
    }
    bool operator<=(const Edge& other) const
    {
        return (*this < other) || (*this == other);
    }
    bool operator>(const Edge& other) const
    {
        return !(*this <= other);
    }
    bool operator>=(const Edge& other) const
    {
        return !(*this < other);
    }
};

std::ostream& operator<<(std::ostream& os, const Edge& e)
{
    os << e.from << " " << e.to << " " << e.weight;
    return os;
}

int main()
{
    DynArr<Edge> graph;
    // get input
    for (;;)
    {
        std::string edge;
        std::getline(std::cin, edge);
        if (edge.size() == 0)
            break;

        std::string::size_type pos = 0;
        std::string::size_type count = edge.find(" ", 0) - pos;
        auto from = edge.substr(pos, count);
        pos += count + 1;
        count = edge.find(" ", pos) - pos;
        auto to = edge.substr(pos, count);
        pos += count + 1;
        count = edge.find(" ", pos) - pos;
        auto weightStr = edge.substr(pos, count);
        int weight;
        try
        {
            weight = stoi(weightStr);   
        }
        catch(const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << '\n';
        }
        graph.emplace_back(std::move(from), std::move(to), weight);
    }
    // std::cout << graph << "\n";
    // sort edges
    auto graphSorted(graph);
    timSort(graphSorted.begin(), graphSorted.end());
    // std::cout << graphSorted << "\n";

    // get array of names of tops
    AVLTree<std::string> names;   
    for (int i = 0; i < graphSorted.size(); i++)
    {
        names.insert(graphSorted[i].from);
        names.insert(graphSorted[i].to);
    }
    DynArr<std::string> tops;
    for (AVLTree<std::string>::left_root_right_iterator i = names.begin(); i != names.end(); i++)
        tops.push_back(*i);

    // std::cout << "tops: " << tops << "\n";
    SetsSys setsSys(std::move(tops));
    DynArr<Edge> treeTops;
    for (int i = 0; i < graphSorted.size(); i++)
    {
        if (setsSys.findSet(graphSorted[i].from) != setsSys.findSet(graphSorted[i].to))
        {
            // not cycle, adding this edge to tree
            treeTops.push_back(graphSorted[i]);
            setsSys.unionSets(graphSorted[i].from, graphSorted[i].to);
        }
    }
    
    // print output
    unsigned int weightSum = 0;
    for (int i = 0; i < treeTops.size(); i++)
    {
        std::cout << treeTops[i].from << " " << treeTops[i].to << "\n";
        weightSum += treeTops[i].weight;
    }
    std::cout << weightSum;
}