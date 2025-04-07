#include "genetic_algorithm.hpp"
#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>
#include <limits>

GeneticAlgorithm::GeneticAlgorithm(VRP &vrp)
    : vrp(vrp), bestCost(std::numeric_limits<double>::max())
{
    // Seed the random number generator with the current time.
    rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
}

void GeneticAlgorithm::initializePopulation(int populationSize)
{
    population.clear();
    const std::vector<Node> &nodes = vrp.getNodes();
    int numNodes = static_cast<int>(nodes.size());
    if (numNodes == 0)
    {
        std::cerr << "No VRP data loaded!" << std::endl;
        return;
    }
    // Assume node 0 is the depot; create a list of customer indices.
    std::vector<int> baseRoute;
    for (int i = 1; i < numNodes; ++i)
    {
        baseRoute.push_back(i);
    }
    // Create the initial population.
    for (int i = 0; i < populationSize; ++i)
    {
        std::vector<int> route = baseRoute;
        std::shuffle(route.begin(), route.end(), rng);
        // Insert depot at the beginning and end of the route.
        route.insert(route.begin(), 0);
        route.push_back(0);
        population.push_back(route);
        double cost = evaluateSolution(route);
        if (cost < bestCost)
        {
            bestCost = cost;
            bestSolution = route;
        }
    }
}

double GeneticAlgorithm::evaluateSolution(const std::vector<int> &solution) const
{
    return vrp.computeRouteCost(solution);
}

void GeneticAlgorithm::evolve()
{
    // Mutate each solution in the population by swapping two random customer nodes.
    for (auto &route : population)
    {
        if (route.size() <= 3)
            continue; // Not enough nodes for mutation.
        // Generate indices between 1 and route.size()-2 (to keep depot fixed).
        std::uniform_int_distribution<int> dist(1, static_cast<int>(route.size()) - 2);
        int i = dist(rng);
        int j = dist(rng);
        std::swap(route[i], route[j]);
        double cost = evaluateSolution(route);
        if (cost < bestCost)
        {
            bestCost = cost;
            bestSolution = route;
        }
    }
}

void GeneticAlgorithm::run(int generations)
{
    if (population.empty())
    {
        initializePopulation(50); // Default population size if not initialized.
    }
    for (int gen = 0; gen < generations; ++gen)
    {
        evolve();
        std::cout << "Generation " << gen << ": Best cost = " << bestCost << std::endl;
    }
}

std::vector<int> GeneticAlgorithm::getBestSolution() const
{
    return bestSolution;
}
