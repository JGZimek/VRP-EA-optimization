#include "vrp.hpp"
#include "genetic_algorithm.hpp"
#include <iostream>
#include <vector>

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

    // Run the genetic algorithm.
    GeneticAlgorithm ga(vrp);
    ga.initializePopulation(50);
    ga.run(100);

    // Retrieve and display the best solution found by the algorithm.
    std::vector<int> best = ga.getBestSolution();
    std::cout << "Best solution found: ";
    std::vector<int> currentRoute;
    for (int node : best)
    {
        if (node == 0)
        {
            if (!currentRoute.empty())
            {
                // Print the current route.
                std::cout << "Route: ";
                for (int n : currentRoute)
                {
                    std::cout << n << " ";
                }
                // std::cout << std::endl;
                currentRoute.clear();
            }
        }
        else
        {
            currentRoute.push_back(node);
        }
    }
    std::cout << std::endl;

    return 0;
}
