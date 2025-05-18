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
 * and evolves the population using selection, crossover, and mutation operators.
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
    std::vector<std::vector<int>> getBestSolution() const;

    /**
     * @brief Returns the cost of the best solution found so far.
     *
     * @return The cost of the best solution.
     */
    double getBestSolutionCost() const;

private:
    VRP &vrp; ///< Reference to the VRP instance.
    std::vector<std::vector<int>> bestSolution;
    std::vector<std::vector<std::vector<int>>> population; ///< Best solution found.
    double bestCost;                                       ///< Cost of the best solution.
    mutable std::mt19937 rng;                              ///< Mersenne Twister random number generator (mutable to allow use in const methods).

    SelectionMethod selectionMethod; ///< Current selection method.
    int tournamentSize;              ///< Tournament size for tournament selection.
    /**
     * @brief Evaluates the cost of a given solution.
     *
     * @param solution A vector of node indices representing a route.
     * @return The computed cost of the route.
     */
    double evaluateSolution(const std::vector<std::vector<int>> &routes) const;

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
    std::vector<std::vector<int>> selectParent() const;

    /**
     * @brief Implements tournament selection.
     *
     * Randomly picks 'tournamentSize' individuals from the population and returns the best.
     *
     * @return The selected parent's gene (route).
     */
    std::vector<std::vector<int>> tournamentSelection() const;

    /**
     * @brief Implements roulette selection.
     *
     * Calculates selection probabilities based on fitness (defined as 1/cost)
     * and returns the selected parent's gene (route).
     *
     * @return The selected parent's gene (route).
     */
    std::vector<std::vector<int>> rouletteSelection() const;

    /**
     * @brief Performs PMX (Partially Mapped Crossover) between two parent solutions.
     *
     * Assumes that both parents represent valid routes with depot at index 0 and at the end.
     * The crossover is applied only on the inner part of the route (i.e. indices [1, size-2]).
     *
     * @param parent1 The first parent solution.
     * @param parent2 The second parent solution.
     * @return An offspring solution generated using PMX.
     */
    std::vector<std::vector<int>> pmxCrossover(const std::vector<std::vector<int>> &parent1, const std::vector<std::vector<int>> &parent2) const;

    /**
     * @brief Performs a simple mutation on a solution by swapping two random customer nodes.
     *
     * The depot at index 0 and at the end remains fixed.
     *
     * @param solution The solution to be mutated.
     */
    void mutate(std::vector<std::vector<int>> &routes) const;

    /**
     * @brief Applies the 2-opt local search algorithm to improve a given route.
     *
     * The algorithm iteratively swaps two edges in the route to reduce the total cost.
     *
     * @param route The route to be improved.
     */
    void twoOpt(std::vector<int> &route) const;
};

#endif // GENETIC_ALGORITHM_HPP
