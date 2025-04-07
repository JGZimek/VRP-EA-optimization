#include "vrp.hpp"
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>

void VRP::loadData(const std::string &filename)
{
    std::ifstream infile(filename);
    if (!infile)
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    nodes.clear();
    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        Node node;
        if (!(iss >> node.id >> node.x >> node.y >> node.demand))
        {
            // Skip improperly formatted lines
            continue;
        }
        nodes.push_back(node);
    }
}

double VRP::distance(const Node &a, const Node &b) const
{
    return std::sqrt((a.x - b.x) * (a.x - b.x) +
                     (a.y - b.y) * (a.y - b.y));
}

double VRP::computeRouteCost(const std::vector<int> &route) const
{
    double totalCost = 0.0;
    for (std::size_t i = 1; i < route.size(); ++i)
    {
        const Node &from = nodes[route[i - 1]];
        const Node &to = nodes[route[i]];
        totalCost += distance(from, to);
    }
    return totalCost;
}

const std::vector<Node> &VRP::getNodes() const
{
    return nodes;
}
