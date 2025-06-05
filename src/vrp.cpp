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
    bool vehicleSection = false;
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

        // Detect the start of the VEHICLE section.
        if (!vehicleSection)
        {
            if (trimmedLine.find("VEHICLE") != std::string::npos)
            {
                vehicleSection = true;
                continue;
            }
        }

        // Read the number of vehicles from the VEHICLE section.
        if (vehicleSection && !customerSection)
        {
            std::istringstream iss(trimmedLine);
            if (iss >> numVehicles)
            {
                vehicleSection = false; // Done reading VEHICLE section.
                continue;
            }
        }

        // Detect the start of the CUSTOMER section.
        if (!customerSection)
        {
            if (trimmedLine.find("CUSTOMER") != std::string::npos)
            {
                customerSection = true;
                continue;
            }
        }

        // Skip header line in CUSTOMER section.
        if (customerSection && !headerSkipped)
        {
            if (trimmedLine.find("CUST NO") != std::string::npos)
            {
                headerSkipped = true;
                continue;
            }
        }

        // Process customer data.
        if (customerSection && headerSkipped)
        {
            std::istringstream iss(trimmedLine);
            int custNo;
            double x, y;
            int demand;

            if (!(iss >> custNo >> x >> y >> demand))
            {
                std::cerr << "Error parsing customer data: " << trimmedLine << std::endl;
                continue;
            }

            Node node;
            node.id = custNo;
            node.x = x;
            node.y = y;
            node.demand = demand;
            nodes.push_back(node);
        }
    }
    infile.close();

    std::cout << "Loaded " << nodes.size() << " customer nodes from " << filename << std::endl;
    std::cout << "Number of vehicles: " << numVehicles << std::endl;
}

double VRP::distance(const Node &a, const Node &b) const
{
    return std::sqrt((a.x - b.x) * (a.x - b.x) +
                     (a.y - b.y) * (a.y - b.y));
}

double VRP::computeRouteCost(const std::vector<int> &route) const
{
    if (route.empty())
        return 0.0;

    double totalCost = 0.0;
    totalCost += distance(nodes[0], nodes[route[0]]);

    for (std::size_t i = 1; i < route.size(); ++i)
    {
        const Node &from = nodes[route[i - 1]];
        const Node &to = nodes[route[i]];
        totalCost += distance(from, to);
    }
    totalCost += distance(nodes[route.back()], nodes[0]);

    return totalCost;
}

const std::vector<Node> &VRP::getNodes() const
{
    return nodes;
}
