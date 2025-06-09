#include "vrp.hpp"
#include "genetic_algorithm.hpp"
#include <iostream>
#include <vector>
#include <numeric> // For std::accumulate
#include <algorithm>
#include <fstream>
#include <filesystem>

int main()
{
    VRP vrp;
    vrp.loadData("data/C101.txt");

    if (vrp.getNodes().empty())
    {
        std::cerr << "No VRP data loaded. Exiting..." << std::endl;
        return 1;
    }

    // Prepare output directory and file for results
    std::filesystem::create_directories("output");
    std::ofstream resultsFile("output/results.csv");
    resultsFile << "run,cost\n";

    const int runs = 10;         // Number of runs
    std::vector<double> results; // Store the results of each run
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

        resultsFile << (i + 1) << "," << cost << "\n";

        std::cout << "Run #" << (i + 1) << " cost: " << cost << std::endl;
        std::cout << "Best solution (routes):" << std::endl;
        auto bestSolution = ga.getBestSolution();

        std::ofstream routeFile("output/run_" + std::to_string(i + 1) + "_routes.txt");

        for (size_t vehicle = 0; vehicle < bestSolution.size(); ++vehicle)
        {
            const auto &route = bestSolution[vehicle];
            if (!route.empty())
            {
                std::cout << "Vehicle " << (vehicle + 1) << " route: ";
                routeFile << "Vehicle " << (vehicle + 1) << ":";
                for (int node : route)
                {
                    std::cout << node << " ";
                    routeFile << ' ' << node;
                }
                std::cout << std::endl;
                routeFile << "\n";
            }
            else
            {
                std::cout << "Vehicle " << (vehicle + 1) << " has no assigned route." << std::endl;
                routeFile << "Vehicle " << (vehicle + 1) << ":" << "\n";
            }
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
        routeFile.close();
    }

    double averageCost = std::accumulate(results.begin(), results.end(), 0.0) / runs;
    std::cout << "Average cost over " << runs << " runs: " << averageCost << std::endl;
    std::cout << "Best cost over " << runs << " runs: " << bestCost << std::endl;

    resultsFile << "average," << averageCost << "\n";
    resultsFile << "best," << bestCost << "\n";
    resultsFile.close();

    return 0;
}
