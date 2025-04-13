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
    double cost = ga.getBestSolutionCost(); // Assuming this method exists to get the cost.
    std::cout << "Best solution found:" << std::endl;

    std::vector<int> currentRoute;
    int routeNumber = 1;

    for (int node : best)
    {
        if (node == 0)
        {
            if (!currentRoute.empty())
            {
                // Print the current route with the route number.
                std::cout << "Route #" << routeNumber << ": ";
                for (int n : currentRoute)
                {
                    std::cout << n << " ";
                }
                std::cout << std::endl;
                currentRoute.clear();
                routeNumber++;
            }
        }
        else
        {
            currentRoute.push_back(node);
        }
    }

    // Print the total cost of the solution.
    std::cout << "Cost " << cost << std::endl;

    return 0;
}
