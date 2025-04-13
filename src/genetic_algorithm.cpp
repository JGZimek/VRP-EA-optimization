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

void GeneticAlgorithm::reproduce()
{
    // New population using selection and mutation.
    std::vector<std::vector<int>> newPopulation;
    // Elitism: preserve the best solution.
    newPopulation.push_back(bestSolution);
    // Repeatedly select parents, create offspring (via mutation), and add to new population.
    while (newPopulation.size() < population.size())
    {
        std::vector<int> parent = selectParent();
        std::vector<int> offspring = parent; // Clone parent.
        // Mutation: swap two random customer nodes.
        if (offspring.size() > 3)
        { // Ensure enough nodes for mutation.
            std::uniform_int_distribution<int> dist(1, static_cast<int>(offspring.size()) - 2);
            int i = dist(rng);
            int j = dist(rng);
            std::swap(offspring[i], offspring[j]);
        }
        newPopulation.push_back(offspring);
    }
    population = newPopulation;
    // Update bestSolution and bestCost.
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
