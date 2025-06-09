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
    // Load VRP instance data from the Solomon instance file.
    vrp.loadData("data/C101.txt");

    // Check if data has been loaded.
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

    for (int i = 0; i < runs; ++i)
    {
        // Run the genetic algorithm.
        GeneticAlgorithm ga(vrp);
        ga.initializePopulation(50);
        ga.run(100);

        // Retrieve the best solution cost.
        double cost = ga.getBestSolutionCost(); // Assuming this method exists to get the cost.
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
        }
        routeFile.close();
    }

    // Calculate the average cost.
    double averageCost = std::accumulate(results.begin(), results.end(), 0.0) / runs;
    std::cout << "Average cost over " << runs << " runs: " << averageCost << std::endl;
    double bestCost = *std::min_element(results.begin(), results.end());
    std::cout << "Best cost over " << runs << " runs: " << bestCost << std::endl;

    resultsFile << "average," << averageCost << "\n";
    resultsFile << "best," << bestCost << "\n";
    resultsFile.close();

    return 0;
}
