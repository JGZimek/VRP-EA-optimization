#ifndef GENETIC_ALGORITHM_HPP
#define GENETIC_ALGORITHM_HPP

#include "vrp.hpp"
#include <vector>
#include <random>

/**
 * @brief Class implementing a basic Genetic Algorithm for VRP.
 *
 * This class initializes a population of candidate solutions, evaluates them,
 * and evolves the population using a simple mutation operator.
 */
class GeneticAlgorithm
{
public:
    /**
     * @brief Constructs a GeneticAlgorithm instance.
     *
     * @param vrp A reference to the VRP instance containing problem data.
     */
    explicit GeneticAlgorithm(VRP &vrp);

    /// Default destructor.
    ~GeneticAlgorithm() = default;

    /**
     * @brief Initializes the population with random solutions.
     *
     * It is assumed that the node with index 0 is the depot. Customer nodes are
     * randomly permuted to generate initial solutions.
     *
     * @param populationSize Number of solutions in the initial population.
     */
    void initializePopulation(int populationSize);

    /**
     * @brief Runs the genetic algorithm for a given number of generations.
     *
     * @param generations The number of generations to evolve.
     */
    void run(int generations);

    /**
     * @brief Returns the best solution found so far.
     *
     * @return A vector of node indices representing the best route.
     */
    std::vector<int> getBestSolution() const;

private:
    VRP &vrp;                                 ///< Reference to the VRP instance.
    std::vector<std::vector<int>> population; ///< Population of candidate solutions.
    std::vector<int> bestSolution;            ///< Best solution found.
    double bestCost;                          ///< Cost of the best solution.
    std::mt19937 rng;                         ///< Mersenne Twister random number generator.

    /**
     * @brief Evaluates the cost of a given solution.
     *
     * @param solution A vector of node indices representing a route.
     * @return The computed cost of the route.
     */
    double evaluateSolution(const std::vector<int> &solution) const;

    /**
     * @brief Performs a simple mutation on the population.
     *
     * Each solution is mutated by swapping two random customer nodes (depot remains fixed).
     */
    void evolve();
};

#endif // GENETIC_ALGORITHM_HPP
