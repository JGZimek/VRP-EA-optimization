# VRP-EA-optimization
Repository dedicated to solving the Vehicle Routing Problem (VRP) using an Evolutionary Algorithm (EA). Includes heuristic-based optimization approaches and implementation details for efficient route planning.

## Output Files

Running the program now saves results to the `output` directory:

* `output/results.csv` &ndash; contains a CSV table of run number and best cost, followed by the average and best cost across all runs.
* `output/run_<n>_routes.txt` &ndash; for each run `n`, lists the routes of all vehicles.

These files can be parsed by external scripts (for example, in Python) to generate graphs or further analyses.
