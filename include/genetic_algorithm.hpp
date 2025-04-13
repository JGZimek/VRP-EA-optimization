#ifndef GENETIC_ALGORITHM_HPP
#define GENETIC_ALGORITHM_HPP

#include "vrp.hpp"
#include <vector>
#include <random>

/**
 * @brief Enum to specify the selection method.
 */
enum class SelectionMethod
{
    Tournament,
    Roulette
};

/**
 * @brief Class implementing a basic Genetic Algorithm for VRP.
 *
 * This class initializes a population of candidate solutions, evaluates them,
 * and evolves the population using selection, mutation, and elitism.
 */
class GeneticAlgorithm
{
public:
    /**
     * @brief Constructs a GeneticAlgorithm instance.
     *
     * @param vrp A reference to the VRP instance containing problem data.
     * @param selMethod Selection method to be used (default Tournament).
     * @param tourSize Tournament size for tournament selection (default 3).
     */
    explicit GeneticAlgorithm(VRP &vrp, SelectionMethod selMethod = SelectionMethod::Tournament, int tourSize = 3);

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
    mutable std::mt19937 rng;                 ///< Mersenne Twister random number generator (mutable to allow use in const methods).

    SelectionMethod selectionMethod; ///< Current selection method.
    int tournamentSize;              ///< Tournament size for tournament selection.
    /**
     * @brief Evaluates the cost of a given solution.
     *
     * @param solution A vector of node indices representing a route.
     * @return The computed cost of the route.
     */
    double evaluateSolution(const std::vector<int> &solution) const;

    /**
     * @brief Performs reproduction to generate a new generation.
     *
     * Uses the current selection method to choose parents, applies mutation,
     * and incorporates elitism by preserving the best solution.
     */
    void reproduce();

    /**
     * @brief Selects one parent solution using the configured selection method.
     *
     * @return A vector of node indices representing the selected parent.
     */
    std::vector<int> selectParent() const;

    /**
     * @brief Implements tournament selection.
     *
     * Randomly picks 'tournamentSize' individuals from the population and returns the best.
     *
     * @return The selected parent's gene (route).
     */
    std::vector<int> tournamentSelection() const;

    /**
     * @brief Implements roulette selection.
     *
     * Calculates selection probabilities based on fitness (defined as 1/cost)
     * and returns the selected parent's gene (route).
     *
     * @return The selected parent's gene (route).
     */
    std::vector<int> rouletteSelection() const;
};

#endif // GENETIC_ALGORITHM_HPP
