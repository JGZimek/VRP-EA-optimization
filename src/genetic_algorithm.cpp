#include "genetic_algorithm.hpp"
#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>
#include <limits>
#include <numeric>
#include <stdexcept>

GeneticAlgorithm::GeneticAlgorithm(VRP &vrp, SelectionMethod selMethod, int tourSize)
    : vrp(vrp), bestCost(std::numeric_limits<double>::max()),
      selectionMethod(selMethod), tournamentSize(tourSize)
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

    // Create the initial population
    for (int i = 0; i < populationSize; ++i)
    {
        int numVehicles = vrp.getNumVehicles(); 
        std::vector<std::vector<int>> routes(numVehicles); // Routes for each vehicle
        std::vector<int> unassignedCustomers;

        // Prepare a list of customers (excluding the depot)
        for (int j = 1; j < numNodes; ++j)
        {
            unassignedCustomers.push_back(j);
        }

        // Shuffle the customers randomly
        std::shuffle(unassignedCustomers.begin(), unassignedCustomers.end(), rng);

        // Assign customers to vehicles in a round-robin manner
        int vehicleIndex = 0;
        for (int customer : unassignedCustomers)
        {
            routes[vehicleIndex].push_back(customer);
            vehicleIndex = (vehicleIndex + 1) % numVehicles;
        }

        // Build the solution as a single list with depots (0) between routes
        std::vector<int> flattenedSolution;
        for (const auto &route : routes)
        {
            if (!route.empty())
            {
                flattenedSolution.push_back(0); // Start depot
                flattenedSolution.insert(flattenedSolution.end(), route.begin(), route.end());
                flattenedSolution.push_back(0); // End depot
            }
        }

        population.push_back(flattenedSolution);
        double cost = evaluateSolution(flattenedSolution);
        if (cost < bestCost)
        {
            bestCost = cost;
            bestSolution = flattenedSolution;
        }
    }
}

// Evaluates the total cost of a solution by summing the costs of all routes
double GeneticAlgorithm::evaluateSolution(const std::vector<int> &solution) const
{
    double totalCost = 0.0;
    std::vector<int> currentRoute;

    for (int node : solution)
    {
        if (node == 0)
        {
            if (!currentRoute.empty())
            {
                // Compute the cost of the current route
                totalCost += vrp.computeRouteCost(currentRoute);
                currentRoute.clear();
            }
        }
        else
        {
            currentRoute.push_back(node);
        }
    }

    // Add the cost of the final route if it exists
    if (!currentRoute.empty())
    {
        totalCost += vrp.computeRouteCost(currentRoute);
    }
}

std::vector<int> GeneticAlgorithm::tournamentSelection() const
{
    int popSize = population.size();
    if (popSize == 0)
        throw std::runtime_error("Population is empty during tournament selection.");

    std::uniform_int_distribution<int> dist(0, popSize - 1);
    int bestIndex = dist(rng);
    double bestCostLocal = evaluateSolution(population[bestIndex]);
    for (int k = 1; k < tournamentSize; ++k)
    {
        int idx = dist(rng);
        double cost = evaluateSolution(population[idx]);
        if (cost < bestCostLocal)
        {
            bestCostLocal = cost;
            bestIndex = idx;
        }
    }
    return population[bestIndex];
}

std::vector<int> GeneticAlgorithm::rouletteSelection() const
{
    int popSize = population.size();
    if (popSize == 0)
        throw std::runtime_error("Population is empty during roulette selection.");

    // Calculate fitness values (lower cost => higher fitness)
    std::vector<double> fitness(popSize);
    double totalFitness = 0.0;
    for (int i = 0; i < popSize; ++i)
    {
        double cost = evaluateSolution(population[i]);
        // Adding a small value to avoid division by zero.
        double fit = 1.0 / (cost + 1e-6);
        fitness[i] = fit;
        totalFitness += fit;
    }
    std::uniform_real_distribution<double> dist(0.0, totalFitness);
    double r = dist(rng);
    double cumulative = 0.0;
    for (int i = 0; i < popSize; ++i)
    {
        cumulative += fitness[i];
        if (cumulative >= r)
        {
            return population[i];
        }
    }
    return population.back(); // fallback
}

std::vector<int> GeneticAlgorithm::selectParent() const
{
    if (selectionMethod == SelectionMethod::Tournament)
        return tournamentSelection();
    else
        return rouletteSelection();
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

    // Map elements from parent2 to offspring.
    for (int i = cut1; i <= cut2; ++i)
    {
        int elem = parent2[i];
        if (std::find(offspring.begin() + cut1, offspring.begin() + cut2 + 1, elem) != offspring.end())
            continue;

        int pos = i;
        while (true)
        {
            int mappedElem = parent1[pos];
            auto it = std::find(parent2.begin(), parent2.end(), mappedElem);
            pos = std::distance(parent2.begin(), it);
            if (pos < cut1 || pos > cut2)
            {
                offspring[pos] = elem;
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
