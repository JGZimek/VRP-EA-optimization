#include "genetic_algorithm.hpp"
#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>

GeneticAlgorithm::GeneticAlgorithm(VRP &vrp,
                                   SelectionMethod selMethod,
                                   int tourSize,
                                   double crossoverProb,
                                   double mutationProb)
    : vrp(vrp), bestCost(std::numeric_limits<double>::max()),
      selectionMethod(selMethod), tournamentSize(tourSize),
      crossoverProb(crossoverProb), mutationProb(mutationProb)
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

    for (int i = 0; i < populationSize; ++i)
    {
        int numVehicles = vrp.getNumVehicles();
        std::vector<std::vector<int>> routes(numVehicles);
        std::vector<int> unassignedCustomers;

        for (int j = 1; j < numNodes; ++j)
        {
            unassignedCustomers.push_back(j);
        }

        std::shuffle(unassignedCustomers.begin(), unassignedCustomers.end(), rng);

        int vehicleIndex = 0;
        for (int customer : unassignedCustomers)
        {
            routes[vehicleIndex].push_back(customer);
            vehicleIndex = (vehicleIndex + 1) % numVehicles;
        }

        population.push_back(routes);
        double cost = evaluateSolution(routes);
        if (cost < bestCost)
        {
            bestCost = cost;
            bestSolution = routes;
        }
    }
}
double GeneticAlgorithm::evaluateSolution(const std::vector<std::vector<int>> &routes) const
{
    double totalCost = 0.0;
    for (const auto &route : routes)
    {
        if (!route.empty())
        {
            totalCost += vrp.computeRouteCost(route);
        }
    }
    return totalCost;
}

std::vector<std::vector<int>> GeneticAlgorithm::tournamentSelection() const
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
std::vector<std::vector<int>> GeneticAlgorithm::rouletteSelection() const
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
        double fit = 1.0 / (cost + 1e-6); // Avoid division by zero
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
    return population.back(); // Fallback
}

std::vector<std::vector<int>> GeneticAlgorithm::selectParent() const
{
    if (selectionMethod == SelectionMethod::Tournament)
        return tournamentSelection();
    else
        return rouletteSelection();
}

void GeneticAlgorithm::mutate(std::vector<std::vector<int>> &routes) const
{
    std::uniform_int_distribution<int> vehicleDist(0, routes.size() - 1);
    int vehicle = vehicleDist(rng);

    if (routes[vehicle].size() <= 1)
        return; // Not enough nodes to mutate

    std::uniform_int_distribution<int> nodeDist(0, routes[vehicle].size() - 1);
    int i = nodeDist(rng);
    int j = nodeDist(rng);
    std::swap(routes[vehicle][i], routes[vehicle][j]);
}

std::vector<std::vector<int>> GeneticAlgorithm::pmxCrossover(
    const std::vector<std::vector<int>> &parent1,
    const std::vector<std::vector<int>> &parent2) const
{
    int maxVehicles = vrp.getNumVehicles();
    bool balancedDistribution = true;

    std::vector<int> flat1, flat2;
    for (const auto &route : parent1)
        flat1.insert(flat1.end(), route.begin(), route.end());
    for (const auto &route : parent2)
        flat2.insert(flat2.end(), route.begin(), route.end());

    std::vector<int>::size_type size = flat1.size();
    std::vector<int> child(size, -1);

    std::uniform_int_distribution<int> dist(0, size - 1);
    std::vector<int>::size_type cut1 = dist(rng), cut2 = dist(rng);
    if (cut1 > cut2)
        std::swap(cut1, cut2);

    for (std::vector<int>::size_type i = cut1; i <= cut2; ++i)
        child[i] = flat1[i];

    std::unordered_map<int, int> mapping;
    for (std::vector<int>::size_type i = cut1; i <= cut2; ++i)
    {
        mapping[flat2[i]] = flat1[i];
    }

    std::unordered_set<int> used(child.begin(), child.end());
    for (std::vector<int>::size_type i = 0; i < size; ++i)
    {
        if (i >= cut1 && i <= cut2)
            continue;

        int candidate = flat2[i];
        std::unordered_set<int> visited;
        while (mapping.find(candidate) != mapping.end())
        {
            if (visited.find(candidate) != visited.end())
            {
                std::cerr << "Cycle detected in mapping for candidate: " << candidate << std::endl;
                break;
            }
            visited.insert(candidate);
            candidate = mapping[candidate];
        }

        if (used.find(candidate) != used.end())
            continue;

        child[i] = candidate;
        used.insert(candidate);
    }

    for (int i = 1; i <= static_cast<int>(size); ++i)
    {
        if (used.find(i) == used.end())
        {
            for (std::vector<int>::size_type j = 0; j < size; ++j)
            {
                if (child[j] == -1)
                {
                    child[j] = i;
                    used.insert(i);
                    break;
                }
            }
        }
    }

    std::vector<std::vector<int>> offspring(maxVehicles);
    std::vector<int>::size_type idx = 0;

    if (balancedDistribution)
    {
        std::uniform_int_distribution<int> emptyVehiclesDist(0, maxVehicles - 1);
        int emptyVehicles = emptyVehiclesDist(rng);

        std::vector<bool> isVehicleEmpty(maxVehicles, false);
        for (int i = 0; i < emptyVehicles; ++i)
        {
            isVehicleEmpty[i] = true;
        }
        std::shuffle(isVehicleEmpty.begin(), isVehicleEmpty.end(), rng);

        int nonEmptyVehicles = maxVehicles - emptyVehicles;
        if (nonEmptyVehicles <= 0)
        {
            std::cerr << "Error: All vehicles are empty. Adjusting empty vehicles count." << std::endl;
            nonEmptyVehicles = 1;
            emptyVehicles = maxVehicles - 1;
            isVehicleEmpty.assign(maxVehicles, false);
            for (int i = 0; i < emptyVehicles; ++i)
            {
                isVehicleEmpty[i] = true;
            }
            std::shuffle(isVehicleEmpty.begin(), isVehicleEmpty.end(), rng);
        }

        int avgClientsPerVehicle = std::ceil(static_cast<double>(child.size()) / nonEmptyVehicles);
        int assignedClients = 0;

        for (int v = 0; v < maxVehicles && idx < child.size(); ++v)
        {
            if (isVehicleEmpty[v])
                continue;

            for (int i = 0; i < avgClientsPerVehicle && idx < child.size(); ++i)
            {
                offspring[v].push_back(child[idx++]);
                assignedClients++;
            }
        }

        int vehicleIdx = 0;
        while (idx < child.size())
        {
            if (!isVehicleEmpty[vehicleIdx % maxVehicles])
            {
                offspring[vehicleIdx % maxVehicles].push_back(child[idx++]);
            }
            vehicleIdx++;
        }
    }
    else
    {
        std::uniform_real_distribution<double> probDist(0.0, 1.0);
        double emptyVehicleProbability = 0.3;

        int avgClientsPerVehicle = std::ceil(static_cast<double>(child.size()) / maxVehicles);
        int maxClientsForVehicle = 2 * avgClientsPerVehicle;

        for (int v = 0; v < maxVehicles && idx < child.size(); ++v)
        {
            if (probDist(rng) < emptyVehicleProbability && maxVehicles - v > 1)
            {
                continue;
            }

            std::uniform_int_distribution<int> routeLengthDist(1, std::min(static_cast<int>(child.size() - idx), maxClientsForVehicle));
            int routeLength = routeLengthDist(rng);

            for (int i = 0; i < routeLength && idx < child.size(); ++i)
            {
                offspring[v].push_back(child[idx++]);
            }
        }

        while (idx < child.size())
        {
            offspring[maxVehicles - 1].push_back(child[idx++]);
        }
    }

    return offspring;
}

void GeneticAlgorithm::twoOpt(std::vector<int> &route) const
{
    if (route.size() < 2)
        return;

    bool improved = true;
    double currentCost = vrp.computeRouteCost(route);
    while (improved)
    {
        improved = false;
        for (size_t i = 1; i < route.size() - 2; ++i)
        {
            for (size_t j = i + 1; j < route.size() - 1; ++j)
            {
                std::reverse(route.begin() + i, route.begin() + j + 1);
                double newCost = vrp.computeRouteCost(route);
                if (newCost < currentCost)
                {
                    currentCost = newCost;
                    improved = true;
                }
                else
                {
                    std::reverse(route.begin() + i, route.begin() + j + 1);
                }
            }
        }
    }
}

void GeneticAlgorithm::reproduce()
{
    double crossoverProbability = crossoverProb;
    double mutationProbability = mutationProb;
    double twoOptProbability = 0.1;
    double forBestSolutiuonsTwoOptProbability = 0.5;

    std::vector<std::vector<std::vector<int>>> newPopulation;

    std::vector<std::pair<double, std::vector<std::vector<int>>>> bestSolutions;

    for (const auto &ind : population)
    {
        double cost = evaluateSolution(ind);
        if (bestSolutions.size() < 10)
        {
            bestSolutions.emplace_back(cost, ind);
            std::sort(bestSolutions.begin(), bestSolutions.end());
        }
        else if (cost < bestSolutions.back().first)
        {
            bestSolutions.back() = {cost, ind};
            std::sort(bestSolutions.begin(), bestSolutions.end());
        }
    }

    std::uniform_real_distribution<double> probDist(0.0, 1.0);

    for (auto &solution : bestSolutions)
    {
        if (probDist(rng) < forBestSolutiuonsTwoOptProbability)
        {
            for (auto &route : solution.second)
            {
                twoOpt(route);
            }
            solution.first = evaluateSolution(solution.second);
        }
    }

    for (const auto &solution : bestSolutions)
    {
        newPopulation.push_back(solution.second);
    }

    while (newPopulation.size() < population.size())
    {
        auto parent1 = selectParent();
        auto parent2 = selectParent();
        while (parent1 == parent2)
        {
            parent2 = selectParent();
        }

        std::vector<std::vector<int>> offspring;
        if (probDist(rng) < crossoverProbability)
        {
            offspring = pmxCrossover(parent1, parent2);
        }
        else
        {
            offspring = parent1;
        }

        if (probDist(rng) < mutationProbability)
        {
            mutate(offspring);
        }

        for (auto &route : offspring)
        {
            if (probDist(rng) < twoOptProbability)
            {
                twoOpt(route);
            }
        }

        newPopulation.push_back(offspring);
    }

    population = newPopulation;

    for (const auto &solution : bestSolutions)
    {
        if (solution.first < bestCost)
        {
            bestCost = solution.first;
            bestSolution = solution.second;
        }
    }
}
void GeneticAlgorithm::run(int generations)
{
    if (population.empty())
    {
        initializePopulation(50);
    }
    for (int gen = 0; gen < generations; ++gen)
    {
        reproduce();
    }
}

std::vector<std::vector<int>> GeneticAlgorithm::getBestSolution() const
{
    return bestSolution;
}

double GeneticAlgorithm::getBestSolutionCost() const
{
    return bestCost;
}
