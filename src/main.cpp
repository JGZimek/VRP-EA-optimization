#include "vrp.hpp"
#include "genetic_algorithm.hpp"
#include <iostream>
#include <vector>
#include <numeric> // For std::accumulate
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <limits> // For std::numeric_limits
#include <sstream>

namespace fs = std::filesystem;

static void runScenario(VRP &vrp,
                        int generations,
                        int population,
                        SelectionMethod method,
                        double crossoverProb,
                        double mutationProb,
                        const std::string &dirName)
{
    fs::create_directories(dirName);

    std::ofstream cfg(dirName + "/config.json");
    cfg << "{\n"
        << "  \"generations\": " << generations << ",\n"
        << "  \"population\": " << population << ",\n"
        << "  \"selection\": \"" << (method == SelectionMethod::Tournament ? "tournament" : "roulette") << "\",\n"
        << "  \"crossover\": " << crossoverProb << ",\n"
        << "  \"mutation\": " << mutationProb << "\n";
    cfg << "}\n";
    cfg.close();

    std::ofstream resultsFile(dirName + "/results.csv");
    resultsFile << "run,cost\n";

    const int runs = 10;
    std::vector<double> results;
    double bestCost = std::numeric_limits<double>::max();
    std::vector<std::vector<int>> bestSolutionOverall;

    for (int i = 0; i < runs; ++i)
    {
        GeneticAlgorithm ga(vrp, method, 3, crossoverProb, mutationProb);
        ga.initializePopulation(population);
        ga.run(generations);

        double cost = ga.getBestSolutionCost();
        results.push_back(cost);
        resultsFile << (i + 1) << "," << cost << "\n";

        auto bestSolution = ga.getBestSolution();
        std::ofstream routeFile(dirName + "/run_" + std::to_string(i + 1) + "_routes.txt");
        for (size_t vehicle = 0; vehicle < bestSolution.size(); ++vehicle)
        {
            const auto &route = bestSolution[vehicle];
            routeFile << "Vehicle " << (vehicle + 1) << ":";
            for (int node : route)
            {
                routeFile << ' ' << node;
            }
            routeFile << "\n";
        }
        routeFile.close();

        if (cost < bestCost)
        {
            bestCost = cost;
            bestSolutionOverall = bestSolution;
        }
    }

    double averageCost = std::accumulate(results.begin(), results.end(), 0.0) / runs;
    resultsFile << "average," << averageCost << "\n";
    resultsFile << "best," << bestCost << "\n";
    resultsFile.close();
}

int main()
{
    VRP vrp;
    vrp.loadData("data/C101.txt");

    if (vrp.getNodes().empty())
    {
        std::cerr << "No VRP data loaded. Exiting..." << std::endl;
        return 1;
    }

    fs::create_directories("output");

    for (int gens : {50, 100, 150})
    {
        runScenario(vrp, gens, 50, SelectionMethod::Tournament, 0.85, 0.1,
                    "output/generations_" + std::to_string(gens));
    }

    for (int pop : {30, 50, 80})
    {
        runScenario(vrp, 100, pop, SelectionMethod::Tournament, 0.85, 0.1,
                    "output/population_" + std::to_string(pop));
    }

    runScenario(vrp, 100, 50, SelectionMethod::Tournament, 0.85, 0.1,
                "output/selection_tournament");
    runScenario(vrp, 100, 50, SelectionMethod::Roulette, 0.85, 0.1,
                "output/selection_roulette");

    std::vector<std::pair<double, double>> probs = {
        {0.05, 0.7}, {0.1, 0.85}, {0.2, 0.95}};
    for (const auto &pr : probs)
    {
        std::ostringstream dir;
        dir << "output/mutation_" << pr.first << "_crossover_" << pr.second;
        runScenario(vrp, 100, 50, SelectionMethod::Tournament, pr.second, pr.first,
                    dir.str());
    }

    return 0;
}
