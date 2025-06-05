from ortools.constraint_solver import pywrapcp, routing_enums_pb2
import math

def read_c101_file(filename):
    coords = []
    with open(filename, 'r') as f:
        lines = f.readlines()

        customer_section = False
        for line in lines:
            line = line.strip()
            if not line:
                continue
            if line.startswith("CUSTOMER"):
                customer_section = True
                continue
            if customer_section:
                if line.startswith("CUST") or line.startswith("CUST NO"):
                    continue
                parts = line.split()
                if len(parts) >= 3:
                    try:
                        x = int(parts[1])
                        y = int(parts[2])
                        coords.append((x, y))
                    except ValueError:
                        continue
    return coords

def compute_euclidean_matrix(locations):
    size = len(locations)
    matrix = []
    for from_node in locations:
        row = []
        for to_node in locations:
            dist = math.hypot(from_node[0] - to_node[0], from_node[1] - to_node[1])
            row.append(int(dist))
        matrix.append(row)
    return matrix

def create_data_model(coords, num_vehicles):
    return {
        "distance_matrix": compute_euclidean_matrix(coords),
        "num_vehicles": num_vehicles,
        "depot": 0,
    }

def solve_vrp(coords, num_vehicles=25):
    data = create_data_model(coords, num_vehicles)
    manager = pywrapcp.RoutingIndexManager(len(data["distance_matrix"]), data["num_vehicles"], data["depot"])
    routing = pywrapcp.RoutingModel(manager)

    def distance_callback(from_index, to_index):
        return data["distance_matrix"][manager.IndexToNode(from_index)][manager.IndexToNode(to_index)]

    transit_callback_index = routing.RegisterTransitCallback(distance_callback)
    routing.SetArcCostEvaluatorOfAllVehicles(transit_callback_index)

    search_parameters = pywrapcp.DefaultRoutingSearchParameters()
    search_parameters.first_solution_strategy = routing_enums_pb2.FirstSolutionStrategy.PATH_CHEAPEST_ARC

    solution = routing.SolveWithParameters(search_parameters)

    if solution:
        total_distance = 0
        for vehicle_id in range(data["num_vehicles"]):
            index = routing.Start(vehicle_id)
            route = []
            route_distance = 0
            while not routing.IsEnd(index):
                node_index = manager.IndexToNode(index)
                route.append(node_index)
                previous_index = index
                index = solution.Value(routing.NextVar(index))
                route_distance += routing.GetArcCostForVehicle(previous_index, index, vehicle_id)
            route.append(manager.IndexToNode(index))
            if len(route) > 2:
                print(f"Vehicle {vehicle_id}: route {route}, distance {route_distance}")
                total_distance += route_distance
        print(f"Total distance: {total_distance}")
    else:
        print("No solution found.")

if __name__ == "__main__":
    coords = read_c101_file("C101.txt")
    solve_vrp(coords, num_vehicles=25)
