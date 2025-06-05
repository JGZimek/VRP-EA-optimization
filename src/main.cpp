#include "vrp.hpp"
#include "genetic_algorithm.hpp"
#include <iostream>
#include <vector>
#include <numeric> // For std::accumulate
#include <algorithm>

int main()
{
    VRP vrp;
    vrp.loadData("data/C101.txt");

    if (vrp.getNodes().empty())
    {
        std::cerr << "No VRP data loaded. Exiting..." << std::endl;
        return 1;
    }

    const int runs = 10;
    std::vector<double> results;
    double bestCost = std::numeric_limits<double>::max();
    std::vector<std::vector<int>> bestSolutionOverall;

    for (int i = 0; i < runs; ++i)
    {
        GeneticAlgorithm ga(vrp);
        ga.initializePopulation(50);
        ga.run(100);

        double cost = ga.getBestSolutionCost();
        results.push_back(cost);

        if (cost < bestCost)
        {
            bestCost = cost;
            bestSolutionOverall = ga.getBestSolution();
        }
    }

    std::cout << "Best solution (routes):" << std::endl;
    for (size_t vehicle = 0; vehicle < bestSolutionOverall.size(); ++vehicle)
    {
        const auto &route = bestSolutionOverall[vehicle];
        if (!route.empty())
        {
            std::cout << "Vehicle " << (vehicle + 1) << " route: ";
            for (int node : route)
            {
                std::cout << node << " ";
            }
            std::cout << std::endl;
        }
        else
        {
            // std::cout << "Vehicle " << (vehicle + 1) << " has no assigned route." << std::endl;
        }
    }

    double averageCost = std::accumulate(results.begin(), results.end(), 0.0) / runs;
    std::cout << "Average cost over " << runs << " runs: " << averageCost << std::endl;
    std::cout << "Best cost over " << runs << " runs: " << bestCost << std::endl;

    return 0;
}
