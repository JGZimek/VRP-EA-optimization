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
 * This class handles data loading from benchmark files (e.g., Solomon instances)
 * and provides utility functions for computing distances and route costs.
 */
class VRP
{
public:
    /// Default constructor.
    VRP() = default;

    /// Default destructor.
    ~VRP() = default;

    /**
     * @brief Loads VRP data from a file in the Solomon benchmark format.
     *
     * The file is expected to contain a header section (with VEHICLE information)
     * followed by a CUSTOMER section. The parser skips all lines until the line containing
     * "CUSTOMER" is encountered, then skips the header line (which contains "CUST NO") and
     * finally processes each customer line. Only the first four fields (customer number, X coordinate,
     * Y coordinate, demand) are parsed; other columns are ignored.
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
    int getNumVehicles() const { return numVehicles; }
private:
    std::vector<Node> nodes; ///< List of nodes.
    int numVehicles = 0;     ///< Number of vehicles.
};

#endif // VRP_HPP
