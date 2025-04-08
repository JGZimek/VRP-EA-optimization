#include "vrp.hpp"
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <string>

// Helper function to trim whitespace from both ends of a string.
static inline std::string trim(const std::string &s)
{
    const std::string whitespace = " \t\n\r\f\v";
    size_t start = s.find_first_not_of(whitespace);
    if (start == std::string::npos)
        return "";
    size_t end = s.find_last_not_of(whitespace);
    return s.substr(start, end - start + 1);
}

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
    bool customerSection = false;
    bool headerSkipped = false;

    // Process file line by line.
    while (std::getline(infile, line))
    {
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty())
        {
            continue; // Skip empty lines.
        }

        // Detect the start of the CUSTOMER section.
        if (!customerSection)
        {
            if (trimmedLine.find("CUSTOMER") != std::string::npos)
            {
                customerSection = true;
            }
            continue; // Skip lines until CUSTOMER section.
        }

        // Skip header line in CUSTOMER section.
        if (!headerSkipped)
        {
            if (trimmedLine.find("CUST NO") != std::string::npos)
            {
                headerSkipped = true;
            }
            continue;
        }

        // Process customer data.
        // Expected format: CUST NO.  XCOORD.   YCOORD.    DEMAND   ... (additional fields are ignored)
        std::istringstream iss(trimmedLine);
        int custNo;
        double x, y;
        int demand;

        if (!(iss >> custNo >> x >> y >> demand))
        {
            // If essential fields cannot be parsed, skip the line.
            continue;
        }

        Node node;
        node.id = custNo;
        node.x = x;
        node.y = y;
        node.demand = demand;
        nodes.push_back(node);
    }
    infile.close();

    // Optional: sort nodes by id in case the file order is not sequential.
    std::sort(nodes.begin(), nodes.end(), [](const Node &a, const Node &b)
              { return a.id < b.id; });

    std::cout << "Loaded " << nodes.size() << " customer nodes from " << filename << std::endl;
}

std::vector<int> VRP::loadSolution(const std::string &filename) const
{
    std::vector<int> solution;
    std::ifstream infile(filename);
    if (!infile)
    {
        std::cerr << "Error opening solution file: " << filename << std::endl;
        return solution;
    }
    std::string line;
    // Process file line by line.
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        int num;
        while (iss >> num)
        {
            solution.push_back(num);
        }
    }
    infile.close();
    std::cout << "Loaded known solution with " << solution.size() << " nodes from " << filename << std::endl;
    return solution;
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
