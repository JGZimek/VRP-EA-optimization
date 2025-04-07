#ifndef VRP_HPP
#define VRP_HPP

#include <vector>
#include <string>

/**
 * @brief Structure representing a node (depot or customer).
 */
struct Node
{
    int id;     ///< Unique identifier for the node
    double x;   ///< X coordinate
    double y;   ///< Y coordinate
    int demand; ///< Demand at the node (0 for depot)
};

/**
 * @brief Class representing the Vehicle Routing Problem (VRP).
 *
 * This class handles data loading and provides utility functions for
 * computing distances and route costs.
 */
class VRP
{
public:
    /// Default constructor.
    VRP() = default;

    /// Default destructor.
    ~VRP() = default;

    /**
     * @brief Loads VRP data from a file.
     *
     * The file should have one node per line in the format:
     * id x y demand
     *
     * @param filename Path to the data file.
     */
    void loadData(const std::string &filename);

    /**
     * @brief Computes the Euclidean distance between two nodes.
     *
     * @param a The first node.
     * @param b The second node.
     * @return The Euclidean distance.
     */
    double distance(const Node &a, const Node &b) const;

    /**
     * @brief Computes the total cost of a given route.
     *
     * The route is represented as a sequence of node indices.
     *
     * @param route A vector of node indices representing the route.
     * @return The total cost (distance) of the route.
     */
    double computeRouteCost(const std::vector<int> &route) const;

    /**
     * @brief Returns the list of nodes.
     *
     * @return A constant reference to the vector of nodes.
     */
    const std::vector<Node> &getNodes() const;

private:
    std::vector<Node> nodes; ///< List of nodes.
};

#endif // VRP_HPP
