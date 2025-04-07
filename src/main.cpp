#include "vrp.hpp"
#include "genetic_algorithm.hpp"
#include <iostream>
#include <vector>

int main()
{
    VRP vrp;
    // Load VRP data from a sample file (ensure the file exists in the 'data' directory)
    vrp.loadData("data/sample_vrp.txt");

    GeneticAlgorithm ga(vrp);
    ga.initializePopulation(50);
    ga.run(100);

    std::vector<int> best = ga.getBestSolution();
    std::cout << "Best solution found: ";
    for (int node : best)
    {
        std::cout << node << " ";
    }
    std::cout << std::endl;

    return 0;
}
