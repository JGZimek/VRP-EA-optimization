#include "genetic_algorithm.hpp"
#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>
#include <limits>
#include <stdexcept>

// Constructor: seeds the RNG with the current time.
GeneticAlgorithm::GeneticAlgorithm(VRP &vrp)
    : vrp(vrp), bestCost(std::numeric_limits<double>::max())
{
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

// Helper mutation: swap two random customer nodes (depot remains fixed).
void GeneticAlgorithm::mutate(std::vector<int> &solution) const
{
    if (solution.size() <= 3)
        return; // Not enough nodes to perform mutation.
    std::uniform_int_distribution<int> dist(1, static_cast<int>(solution.size()) - 2);
    int i = dist(rng);
    int j = dist(rng);
    std::swap(solution[i], solution[j]);
}

// PMX Crossover implementation for permutations.
// Operates on indices [1, size-2] (excluding depot positions).
std::vector<int> GeneticAlgorithm::pmxCrossover(const std::vector<int> &parent1, const std::vector<int> &parent2) const
{
    int n = parent1.size();
    if (n <= 3)
        return parent1; // Not enough nodes for crossover.

    std::vector<int> offspring(n, -1);
    // Fix depot at beginning and end.
    offspring[0] = 0;
    offspring[n - 1] = 0;

    // Select two crossover points randomly in [1, n-2].
    std::uniform_int_distribution<int> dist(1, n - 2);
    int cut1 = dist(rng);
    int cut2 = dist(rng);
    if (cut1 > cut2)
        std::swap(cut1, cut2);

    // Copy the segment from parent1 to offspring.
    for (int i = cut1; i <= cut2; ++i)
    {
        offspring[i] = parent1[i];
    }

    // Mapping: for each position i in the cut, map the element from parent2.
    for (int i = cut1; i <= cut2; ++i)
    {
        int elem = parent2[i];
        // If element already present in offspring, skip.
        if (std::find(offspring.begin() + cut1, offspring.begin() + cut2 + 1, elem) != offspring.end())
            continue;

        // Find position in parent2.
        int pos = i;
        while (true)
        {
            // Find the element in parent1 at pos.
            int mappedElem = parent1[pos];
            // Find the position of mappedElem in parent2.
            auto it = std::find(parent2.begin(), parent2.end(), mappedElem);
            pos = std::distance(parent2.begin(), it);
            // Stop if pos is outside the crossover segment.
            if (pos < cut1 || pos > cut2)
            {
                if (offspring[pos] == -1)
                {
                    offspring[pos] = elem;
                }
                break;
            }
        }
    }

    // Fill in remaining positions from parent2 that are not yet set.
    for (int i = 1; i < n - 1; ++i)
    {
        if (offspring[i] == -1)
            offspring[i] = parent2[i];
    }

    return offspring;
}

// Reproduction: generates a new population using crossover and mutation with elitism.
void GeneticAlgorithm::reproduce()
{
    double crossoverProbability = 0.85;
    double mutationProbability = 0.1;
    std::vector<std::vector<int>> newPopulation;
    // Elitism: preserve the best solution.
    newPopulation.push_back(bestSolution);

    std::uniform_int_distribution<int> indexDist(0, population.size() - 1);
    std::uniform_real_distribution<double> probDist(0.0, 1.0);

    while (newPopulation.size() < population.size())
    {
        // Select two parents randomly.
        int idx1 = indexDist(rng);
        int idx2 = indexDist(rng);
        while (idx2 == idx1)
        { // ensure different parents.
            idx2 = indexDist(rng);
        }
        std::vector<int> offspring;
        if (probDist(rng) < crossoverProbability)
        {
            offspring = pmxCrossover(population[idx1], population[idx2]);
        }
        else
        {
            // If not performing crossover, clone one parent.
            offspring = population[idx1];
        }
        // Apply mutation with a given probability.
        if (probDist(rng) < mutationProbability)
        {
            mutate(offspring);
        }
        newPopulation.push_back(offspring);
    }
    population = newPopulation;
    // Update best solution in the new population.
    for (const auto &ind : population)
    {
        double c = evaluateSolution(ind);
        if (c < bestCost)
        {
            bestCost = c;
            bestSolution = ind;
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
        reproduce();
        std::cout << "Generation " << gen << ": Best cost = " << bestCost << std::endl;
    }
}

std::vector<int> GeneticAlgorithm::getBestSolution() const
{
    return bestSolution;
}
